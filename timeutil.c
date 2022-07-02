#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int milliSecAtNow() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return (now.tv_sec * 1000) + (now.tv_usec / 1000);
}