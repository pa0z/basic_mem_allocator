A guided re-implementation of the malloc(), calloc(), realloc() and free functions in C using the sbrk() system call.

Project and guide: https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory

I took on this project to gain a deeper understanding of memory management and allocation at a lower level and gain more
practical experience using pointers, pointer arithmetic and data structures.

- Created a header struct to store size, availability and location to next block for every allocated block of memory and
padded it to ensure uniform header sizes
- implemented a basic locking mechanism to prevent multiple threads from concurrently accessing the same memory
- Added a head and tail to track the linked list of memory blocks, implemented the code for traversing, appending,
and removing members of the linked list throughout project
- Heap is contiguous so only memory at the end of the heap can be freed, “freed” memory not at the end is
instead marked as available and reused, when memory is requested, the linked list is traversed searching with a
first-fit approach
- sbrk() is called with the additional header size on top of requested size, the headers are hidden from the user by
jumping over them with pointer arithmetic

Intend to branch out this project and create a more efficient memory allocator with mmap() and create debugging and
benchmarking utilities for it.
