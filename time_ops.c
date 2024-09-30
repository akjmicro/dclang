#include <sys/time.h>
#include <time.h>

struct timeval tval;


void clockfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_LONG now = ((DCLANG_LONG) tval.tv_sec * 1000000) + ((DCLANG_LONG) tval.tv_usec);
    push(now);
}

void epochfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_LONG now = (tval.tv_sec);
    push(now);
}

void sleepfunc() {
    if (data_stack_ptr < 1) {
        printf("sleep -- need a time amount in nanoseconds on the stack!\n");
        return;
    }
    DCLANG_LONG sleeptime = dclang_pop();
    struct timespec t1, t2;
    t1.tv_sec = floor(sleeptime / 1000000000);
    t1.tv_nsec = sleeptime % 1000000000;
    nanosleep(&t1, &t2);
}

// date functions

void dt_to_epochfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("dt->epoch: need a <date> like \"2020-01-01 12:14:13\" and a <input_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_ULONG fmt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG to_conv = (DCLANG_ULONG) dclang_pop();
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
    push((DCLANG_ULONG) res_time);
}

void epoch_to_dtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("epoch->dt: need a <epoch_int> and an <output_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_ULONG fmt = (DCLANG_ULONG) dclang_pop();
    if (fmt < MIN_STR || fmt > MAX_STR)
    {
        printf("epoch->dt -- <output_format> string address out-of-range.\n");
        return;
    }
    DCLANG_ULONG in_epoch_uint = (DCLANG_ULONG) dclang_pop();
    time_t in_epoch = (time_t) in_epoch_uint;
    char tmbuf[256];
    memset(&tmbuf[0], 0, 256);
    struct tm *loctime = localtime(&in_epoch);
    if (strftime(tmbuf, 256, (char *) fmt, loctime) == 0)
    {
        printf("'strftime', a low-level call of 'epoch->dt', returned an error.\n");
        return;
    }
    DCLANG_PTR bufaddr = (DCLANG_PTR) tmbuf;
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
