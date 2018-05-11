#include <sys/time.h>

struct timeval tval;

static void clockfunc()
{  
  gettimeofday(&tval, NULL);
  double now = (tval.tv_sec * 1000000) + tval.tv_usec;
  push(now);
}
