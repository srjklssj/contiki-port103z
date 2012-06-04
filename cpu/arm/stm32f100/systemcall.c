#include <sys/types.h>

extern int __HEAP_START;

caddr_t _sbrk(int incr) {
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (NULL == heap)
		heap = (unsigned char *)&__HEAP_START;
	prev_heap = heap;
	heap += incr;
	return (caddr_t)prev_heap;
}

