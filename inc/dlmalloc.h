#ifndef __dlmalloc_h
#define __dlmalloc_h

void dlmalloc_setExhaustedCB(void (*cb)(void));
void init_dlmalloc(char* heapstart, char* heapend);
void* dlmalloc(size_t bytes);
void* dlrealloc(void* oldmem, size_t bytes);
void dlfree(void* mem);


#ifdef USE_DLMALLOC
#define baMalloc(size) dlmalloc(size)
#define baRealloc(ptr, size) dlrealloc(ptr, size)
#define baFree(ptr) dlfree(ptr)
#endif

#endif
