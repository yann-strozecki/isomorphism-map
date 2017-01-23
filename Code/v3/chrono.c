#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

// Return current time in second-micro/nanosecond
double what_time_is_it () {
//	clock_gettime is on recent linux but not in MacOS
//	We use gettimeofday instead

//	struct timespec tp;
//	clock_gettime(CLOCK_REALTIME,&tp);
//	double current_time = tp.tv_sec + tp.tv_nsec*1e-9;

	struct timeval tv;
	gettimeofday(&tv,NULL);
	double current_time = tv.tv_sec + tv.tv_usec*1e-6;

	return current_time;
}

double chrono() {
  static double start_time;
  static double last_time;
  static int first = 1;
  double current_time = what_time_is_it();;
  if (first) { start_time = last_time = current_time; first = 0; }
  else printf("\r\033[2K   TIME Elapsed %.3lfs Total %.3lfs\n\n",current_time - last_time, current_time - start_time);
  last_time = current_time;
  return current_time - start_time;
}
