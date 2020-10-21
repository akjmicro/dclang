#include <sys/time.h>
#include <time.h>

struct timeval tval;
struct tm dt_epoch_tm;


static void clockfunc()
{
    gettimeofday(&tval, NULL);
    MYFLT now = (tval.tv_sec * 1000000) + tval.tv_usec;
    push(now);
}

static void epochfunc()
{
    gettimeofday(&tval, NULL);
    MYFLT now = (tval.tv_sec);
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

static void dt_to_epochfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("dt->epoch: need a <iso_fmt_date> like \"2020-01-01 12:14:13\" and a <input_format> on the stack.\n");
        return;
    }
    // input string setup
    char *fmt = (char *)(MYUINT) pop();
    char *to_conv = (char *)(MYUINT) pop();
    // convert to broken time
    if (strptime(to_conv, fmt, &dt_epoch_tm) == NULL)
    {
        printf("Conversion to broken time failed in 'dt->epoch'\n");
        return;
    }
    // do the conversion to seconds since epoch
    time_t res_time = mktime(&dt_epoch_tm);
    push((MYINT) res_time);
}

static void epoch_to_dtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("epoch->dt: need a <epoch_int> and an <output_format> on the stack.\n");
        return;
    }
    // input string setup
    char *fmt = (char *)(MYUINT) pop();
    time_t in_epoch = (time_t)(MYUINT) pop();
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
    struct tm *loctime = localtime(&in_epoch);
    if (strftime(tmbuf, 64, fmt, loctime) == 0)
    {
        printf("'strftime', a low-level call of 'epoch->dt', returned an error.\n");
        return;
    }
    push(bufaddr);
}
