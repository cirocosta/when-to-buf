#ifndef H__TIMING
#define H__TIMING

#include <stdio.h>
#include <sys/time.h>

typedef struct timing {
	struct timeval start;
	struct timeval end;
} t_timing;

int
timing_start(t_timing* t)
{
	return gettimeofday(&t->start, NULL);
}

int
timing_finish(t_timing* t)
{
	return gettimeofday(&t->end, NULL);
}

int
timing_get_elapsed(t_timing* t)
{
	int elapsed = ((t->end.tv_sec - t->start.tv_sec) * 1000000) +
	              (t->end.tv_usec - t->start.tv_usec);

	return elapsed;
}

#endif // ! H__TIMING
