# Custom Malloc Allocator (C)

A simple dynamic memory allocator written in C that mimics a subset of the standard `malloc()` / `free()` behavior using the `sbrk()` system call.

This project was built to understand how heap memory management works at a low level, including free lists, block splitting, and coalescing.

## Features

- Custom implementation of:
  - `my_malloc(size_t size)`
  - `my_free(void *ptr)`
- Free list-based memory management
- First-fit allocation strategy
- Block splitting for efficient reuse of memory
- Coalescing of adjacent free blocks
- 8-byte memory alignment
- Uses `sbrk()` for heap expansion

### Memory Layout
Each allocated block includes an 8-byte header storing the block size. The user pointer returned by `my_malloc` points just after this header.

### Allocation (`my_malloc`)
1. Requested size is aligned to 8 bytes.
2. The free list is searched for a suitable block (first-fit).
3. If a block is found:
   - It may be split if the remainder is large enough.
   - Otherwise, the entire block is removed from the free list.
4. If no block is found:
   - The heap is expanded using `sbrk()`.
   - A new block is added to the free list or returned directly (for large allocations).

### Deallocation (`my_free`)
- The block is inserted back into the front of the free list.
- Metadata is preserved for future reuse.

### Coalescing (`coalesce_free_list`)
- Free blocks are sorted by address.
- Adjacent blocks are merged to reduce fragmentation.
- Uses pointer arithmetic to detect contiguous memory regions.

## ⚙️ Build Instructions
```bash
gcc -o test mymalloc.c test.c
