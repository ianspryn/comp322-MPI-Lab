#include <mpi.h>
#include <stdio.h>

#ifndef _TIMER_H
#define _TIMER_H_

#include <sys/time.h>

#define GET_TIME(now){ \
	struct timeval t; \
	gettimeofday(&t, NULL); \
	now = t.tv_sec + t.tv_usec/1000000.0; \
}

#endif


double trapGuidedSchedule(double xLo, double xHi, int numIntervals) {
	double sum; //sum up all of the trapezoid areas
	double width, x; //working variables for Trapezdoial Approximation
	int percentProgress = -1;

	//initialization
	width = (xHi - xLo) / numIntervals;
	sum = (f(xLo) + f(xHi)) / 2.0; //start with endpoints;

	GET_TIME(startTime);

	//calculate the interior f(x)'s
#pragma omp parallel for schedule(guided, 1) num_threads(threadCount) default(none) private(x) reduction(+:sum)
	for (int i = 1; i < numIntervals - 1; i++) {
		if (omp_get_thread_num() == 0 && (int)(((double)i / numIntervals) * 100) > percentProgress) {
			percentProgress = (int)(((double)i / numIntervals) * 100);
			cout << '\r' << percentProgress << "% finished" << flush;
		}
		x = xLo + i * width;
		sum += f(x);
	}

	cout << endl;

	GET_TIME(endTime);
	time = (double)(endTime - startTime);

	//calculate the area of all
	sum *= width; //multiply all by width

	return sum;
} //Trapezoid guided schedule

double f(double x) {
	return exp(-0.1 * x) * sin(5 * x);
}

double integrate(double xLo, double xHi) {
	return ((exp(-xHi / 10) * (10 * sin(5 * xHi) + 500 * cos(5 * xHi))) / -2501) - ((exp(-xLo / 10) * (10 * sin(5 * xLo) + 500 * cos(5 * xLo))) / -2501);
}