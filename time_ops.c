#include <sys/time.h>

struct timeval tval;
struct tm dt_epoch_tm;


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
    if (data_stack_ptr < 1)
    {
        printf("datetime -- need a format str on the stack\n");
        return;
    }
    char *fmt = (char *) (MYUINT) pop();
    char *tmbuf = malloc(64 * sizeof(char));
    MYUINT bufaddr = (MYUINT) tmbuf;
    if (bufaddr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = bufaddr;
    }
    if (bufaddr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = bufaddr;
    }
    time_t curtime = time(NULL);
    struct tm *loctime = localtime(&curtime);
    if (strftime(tmbuf, 64, fmt, loctime) == 0)
    {
        printf("'strftime', a low-level call of 'datetime', returned an error.\n");
        return;
    }
    push(bufaddr);
    free((char *)fmt);
}


static void dt_to_epochfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("dt->epoch: need a <iso_fmt_date> like \"2020-01-01 12:14:13\" on the stack.\n");
        return;
    }
    // input string setup -- leave it uncast as an unsigned int
    MYUINT to_conv = (MYUINT) pop();
    // set time structure
    struct tm *dp_epoch_tm = malloc(sizeof(struct tm));
    memset((void *)&dt_epoch_tm, 0, sizeof(dt_epoch_tm));
    // convert to broken time
    if (strptime((char *) to_conv, "%Y-%m-%d %H:%M:%S", &dt_epoch_tm) == NULL)
    {
        printf("Conversion to broken time failed in 'dt->epoch'\n");
        return;
    }
    // do the conversion to seconds since epoch
    time_t res_time = mktime(&dt_epoch_tm);
    push((MYUINT) res_time);
    free((char *) to_conv);
    free(dp_epoch_tm);
}
