#include <stddef.h>
#include <string.h>

//reserve 32 MB for malloc
#define MEMSIZE 1024*1024*32
#define ALLOC_STACK_DEPTH 128

static char memory_pool[MEMSIZE];
static size_t unused_mem_idx = 0;
size_t alloc_stack[ALLOC_STACK_DEPTH];
int alloc_stack_top = 0;

char *dclang_malloc(size_t size)
{
    // Is what is being asked too much?
    if(size > (MEMSIZE - unused_mem_idx)) {
        return NULL;
    }
    // record where we are for `free` to use for rewinds
    if (alloc_stack_top < ALLOC_STACK_DEPTH) {
        alloc_stack[alloc_stack_top++] = unused_mem_idx;
    }
    // and all the rest...
    char *mem = &memory_pool[unused_mem_idx];
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
    if (alloc_stack_top &&
        mem == &memory_pool[alloc_stack[alloc_stack_top - 1]]) {
        unused_mem_idx = alloc_stack[--alloc_stack_top];
    }
}

char *dclang_strdup(char *tocopy)
{
    size_t str_size = strlen(tocopy) + 1;
    char *outbuf = dclang_malloc(str_size);
    memcpy(outbuf, tocopy, str_size);
    return outbuf;
}
