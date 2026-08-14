#include <unistd.h>
#include <stddef.h>

struct header_t{
    size_t size;
    unsigned is_free;
};

void *malloc(size_t size){
	void *block;
	block = sbrk(size);
	if (block == (void*) -1)
		return NULL;
	return block;
}
