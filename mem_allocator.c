//#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

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

void free(void *block){
    header_t *header, *tmp;
    void *progbreak;

    if (!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;

    progbreak = sbrk(0);
    if ((char*)block + header->s.size == progbreak){
        if (head == tail){
            head = tail = NULL;
        } else {
            tmp = head;
            while (tmp){
    			if (tmp->s.next == tail){
                    tmp->s.next = NULL;
                    tail = tmp;
                }
    			tmp = tmp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    // if block isn't at end of heap, this gotta make do
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize){
    size_t size;
    void *block;
    if (!num || !nsize)
        return NULL;
    size = num * nsize;
    // checks mul overflow
    if (nsize != size / num)
        return NULL;
    block = malloc(size);
    if (!block)
        return NULL;
    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size){
    header_t *header;
    void *ret;
    if (!block || !size)
        return malloc(size);
    header = (header_t*)block -1;
    if (header->s.size >= size)
        return block;
    ret = malloc(size);
    if (ret){
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;
}

/*
 * realloc notes, reallocates to fit greater sizes
 * but doesnt shrink to save memory?
 * order considerations:
 * if malloced before freeing theres a potential short memory overhead?
 * if free before malloc could be issues during run?
 */
