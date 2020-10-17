#include <sys/time.h>

struct timeval tval;


static void clockfunc()
{
    gettimeofday(&tval, NULL);
    MYFLT now = (tval.tv_sec * 1000000) + tval.tv_usec;
    push(now);
}

static void sleepfunc() {
    if (data_stack_ptr < 1) {
        printf("sleep -- need a time amount in seconds on the stack!\n");
        return;
    }
    MYFLT sleeptime = pop();
    struct timespec t1, t2;
    t1.tv_sec = floor(sleeptime);
    t1.tv_nsec = round(fmod(sleeptime, 1) * 1000000000);
    nanosleep(&t1, &t2);
}

// date functions

static void datetimefunc()
{
    struct tm *loctime_in;
    struct tm *loctime;
    char tmbuf[256];
    MYUINT bufaddr = (MYUINT) &tmbuf[0];
    if (bufaddr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = bufaddr;
    }
    if (bufaddr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = bufaddr;
    }
    time_t curtime = time(NULL);
    loctime = localtime(&curtime);
    if (strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", loctime) == 0)
    {
        printf("'strftime', a low-level call of 'datetime', returned an error.\n");
        return;
    }
    push(bufaddr);
}
