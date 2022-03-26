#include <sys/time.h>
#include <time.h>

struct timeval tval;


static void clockfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_FLT now = ((DCLANG_FLT) tval.tv_sec) + (((DCLANG_FLT) tval.tv_usec) / 1000000);
    push(now);
}

static void epochfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_FLT now = (tval.tv_sec);
    push(now);
}

static void sleepfunc() {
    if (data_stack_ptr < 1) {
        printf("sleep -- need a time amount in seconds on the stack!\n");
        return;
    }
    DCLANG_FLT sleeptime = dclang_pop();
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
        printf("dt->epoch: need a <date> like \"2020-01-01 12:14:13\" and a <input_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_UINT fmt = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT to_conv = (DCLANG_UINT) dclang_pop();
    if (fmt < MIN_STR || fmt > MAX_STR)
    {
        printf("dt->epoch -- <input_format> string address out-of-range.\n");
        return;
    }
    if (to_conv < MIN_STR || to_conv > MAX_STR)
    {
        printf("dt->epoch -- <date> string address out-of-range.\n");
        return;
    }
    // memset the dt_conv_tm
    struct tm dt_epoch_tm;
    memset(&dt_epoch_tm, 0, sizeof(dt_epoch_tm));
    // convert to broken time
    if (strptime((char *)to_conv, (char *) fmt, &dt_epoch_tm) == NULL)
    {
        printf("Conversion to broken time failed in 'dt->epoch'\n");
        return;
    }
    // do the conversion to seconds since epoch
    time_t res_time = mktime(&dt_epoch_tm);
    push((DCLANG_UINT) res_time);
}

static void epoch_to_dtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("epoch->dt: need a <epoch_int> and an <output_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_UINT fmt = (DCLANG_UINT) dclang_pop();
    if (fmt < MIN_STR || fmt > MAX_STR)
    {
        printf("epoch->dt -- <output_format> string address out-of-range.\n");
        return;
    }
    DCLANG_UINT in_epoch_uint = (DCLANG_UINT) dclang_pop();
    time_t in_epoch = (time_t) in_epoch_uint;
    char tmbuf[256];
    memset(&tmbuf[0], 0, 256);
    struct tm *loctime = localtime(&in_epoch);
    if (strftime(tmbuf, 256, (char *) fmt, loctime) == 0)
    {
        printf("'strftime', a low-level call of 'epoch->dt', returned an error.\n");
        return;
    }
    DCLANG_UINT bufaddr = (DCLANG_UINT) tmbuf;
    if (bufaddr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = bufaddr;
    }
    if (bufaddr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = bufaddr;
    }
    push(bufaddr);
}
