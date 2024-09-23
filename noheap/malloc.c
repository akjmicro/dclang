#include <stddef.h>
#include <string.h>

//reserve 32 MB for malloc
#define MEMSIZE 1024*1024*32
static char memory_pool[MEMSIZE];
static size_t unused_mem_idx = 0;

char *dclang_malloc(size_t size)
{
    char *mem;
    if(size > (MEMSIZE - unused_mem_idx)) {
        return NULL;
    }
    mem = &memory_pool[unused_mem_idx];
    unused_mem_idx += size;
    unused_mem_idx = (unused_mem_idx + 0x0f) & ~0x0f; // memalign
    return mem;
}

char *dclang_realloc(void *mem, size_t size)
{
    char *new_mem = (char *) dclang_malloc(size);
    if (new_mem != NULL) {
        memcpy(new_mem, mem, size);
    } else {
        exit(1);
    }
    return new_mem;
}

void dclang_free(void *mem)
{
   //do nothing
}
