#include <sys/time.h>

struct timeval tval;

static void clockfunc()
{  
    gettimeofday(&tval, NULL);
    double now = (tval.tv_sec * 1000000) + tval.tv_usec;
    push(now);
}

static void rdtscfunc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtscp"
        : /* output */ "=a" (lo), "=d" (hi)
        : /* no inputs */
        : /* clobbers */ "%rcx");
    push( (uint64_t)lo | (((uint64_t)hi) << 32) );
}
