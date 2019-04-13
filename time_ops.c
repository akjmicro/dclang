#include <sys/time.h>

struct timeval tval;

static void clockfunc()
{  
    gettimeofday(&tval, NULL);
    MYINT now = (tval.tv_sec * 1000000) + tval.tv_usec;
    push(now);
}

static void sleepfunc() {
    MYINT sleeptime = pop();
    struct timespec t1, t2;
    t1.tv_sec = floor(sleeptime);
    t1.tv_nsec = round(fmod(sleeptime, 1) * 1000000000);
    nanosleep(&t1, &t2);
}
