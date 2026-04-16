#include "mymalloc.h"
#include <unistd.h>

// Free list node structure
typedef struct node {
    int s;
    struct node *next;
    struct node *prev;
} *Node;

// Single global variable
Node head = NULL;

int cmp_addr();

void *my_malloc(size_t size) {
    // Make size 8-byte aligned
    int mod = size % 8;
    if(mod != 0)
        size += 8 - mod;
    size += 8;

    Node n;
    char *p;
    int rem;

    while(1) {
        // Search for a node big enough in free list
        for(n = head; n != NULL; n = n->next) {
            if(n->s >= size) {
                rem = n->s - size;
                // Only split if remaining size is usable (≥12 bytes)
                if(rem >= 12) {
                    p = (char *)n;
                    p += size;

                    // Set up new free node with remaining space
                    ((Node)p)->s = rem;
                    ((Node)p)->next = n->next;
                    ((Node)p)->prev = n->prev;

                    if(n->next != NULL)
                        n->next->prev = (Node)p;
                    if(n->prev != NULL)
                        n->prev->next = (Node)p;

                    if(n == head)
                        head = (Node)p;
                }
                else {

                    if(n->next != NULL)
                        n->next->prev = n->prev;
                    if(n->prev != NULL)
                        n->prev->next = n->next;

                    if(n == head)
                        head = n->next;
                }
                // Set size and return pointer to user
                n->s = size;
                p = (char *)n;
                return p + 8;
            }
        }

        if(size > 8192) {
            // For large requests, allocate exact size
            p = (char *)sbrk(size);
            *(int *)p = size;
            return p + 8;
        }
        else {
            // Otherwise allocate 8192 bytes and add to free list
            p = (char *)sbrk(8192);
            *(int *)p = 8192;
            my_free(p + 8);
        }
    }
}

void my_free(void *ptr) {
    // Adjust pointer
    ptr -= 8;

    ((Node)ptr)->next = head;
    ((Node)ptr)->prev = NULL;

    if(head == NULL) {
        head = (Node)ptr;
        head->next = NULL;
        head->prev = NULL;
    }
    else {
        head->prev = (Node)ptr;
        head = (Node)ptr;
    }
}

void *free_list_begin() {
    return head;
}

void *free_list_next(void *node) {
    return ((Node)node)->next;
}

// Combine adjacent free chunks
void coalesce_free_list() {
    void **list;
    int i = 0, sz = 0;
    Node n;
    void *c, *temp, *nxt;

    for(n = head; n != NULL; n = n->next)
        sz++;

    // Copy nodes to array for sorting
    list = (void *)malloc(sizeof(void *) * sz);
    for(n = head; n != NULL; n = n->next) {
        list[i] = n;
        i++;
    }
    qsort(list, sz, sizeof(void *), cmp_addr);

    // Combine adjacent nodes
    for(i = sz - 2; i >= 0; i--) {
        c = list[i];
        temp = c;
        nxt = list[i + 1];
        temp = (char *)temp + ((Node)c)->s;  // Find end of current chunk

        if(temp == nxt) {
            ((Node)c)->s += ((Node)nxt)->s;  // Add sizes

            // Remove the absorbed node from the list
            if(((Node)nxt)->prev != NULL)
                ((Node)nxt)->prev->next = ((Node)nxt)->next;
            else
                head = ((Node)nxt)->next;

            if(((Node)nxt)->next != NULL)
                ((Node)nxt)->next->prev = ((Node)nxt)->prev;
        }
    }

    free(list);
}

// Compare function for qsort
int cmp_addr(const void *a, const void *b) {
    return *(void **)a - *(void **)b;
}
