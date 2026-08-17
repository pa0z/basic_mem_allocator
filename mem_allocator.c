//#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stddef.h>

typedef char ALIGN[16];
typedef  union header header_t;

union header{
    struct{
        size_t size;
        unsigned is_free;
        union header *next; //union header type pointer to next header
    } s;
    ALIGN stub;
};
header_t *head, *tail; //a union header type pointer, points to start, end of linked list

header_t *get_free_block(size_t size){
    header_t *current = head;
    while(current){
        if (current->s.is_free && current->s.size >=size)
            return current;
        current = current->s.next;
    }
    return NULL;
}

pthread_mutex_t global_malloc_lock;


void *malloc(size_t size){
    size_t total_size;
	void *block;
	header_t *header;
	if (!size)
        return NULL;
	pthread_mutex_lock(&global_malloc_lock);
	header = get_free_block(size);
	if (header){
	header->s.is_free = 0;
	pthread_mutex_unlock(&global_malloc_lock);
	return (void*)(header + 1); // returns mem address at end of header (cuz +1 pointer arith) cast as regular pointer
	}
	total_size = sizeof(header_t) + size;
	block = sbrk(total_size); // allocating block of mem, accounting for header size
	if (block == (void*) -1){
	    pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}
	header = block; //sbrk returns start mem address of allocation, so pointing to start of block here
	header->s.size = size;
	header->s.is_free = 0;
	header->s.next = NULL;
	if (!head)
	    head = header;
	if (tail)
	    tail->s.next = header;
	tail = header;
	pthread_mutex_unlock(&global_malloc_lock);

	return (void*)(header + 1); // dont want to point to header
}
