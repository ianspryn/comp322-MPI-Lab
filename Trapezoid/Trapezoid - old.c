#include <mpi.h>
#include <omp.h>
#include <stdio.h>

int source, my_rank, nodeSize = 0;

double trapGuidedSchedule(double xLo, double xHi, int numIntervals);
double f(double x);
double integrate(double xLo, double xHi);

int main() {
	double resultSum;
    	MPI_Init(NULL, NULL);
    	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    	MPI_Comm_size (MPI_COMM_WORLD, &nodeSize);


	printf("test\n");
	double result = trapGuidedSchedule(1, 10, 10000000); //10 million
	//we're supposed to be reducing result into resultSum
	//but there is only one result
	//what if we commented out line below and instead of resultSum, did result
	MPI_Reduce(&result, &resultSum, nodeSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (my_rank == 0) {
	  printf("test\n");
	  	printf("sum: %f, integral: %f\n", resultSum, integrate(1, 10));
	}
	MPI_Finalize();


}

double trapGuidedSchedule(double xLo, double xHi, int numIntervals) {
	double sum; //sum up all of the trapezoid areas
	double width, x; //working variables for Trapezdoial Approximation
	int i;

	//initialization
	width = (xHi - xLo) / numIntervals;
	sum = (f(xLo) + f(xHi)) / 2.0; //start with endpoints;


	//calculate the interior f(x)'s
#pragma omp parallel for schedule(guided, 1) num_threads(4)  reduction(+:sum) //private(x,i)
	for (i = my_rank; i < numIntervals - 1; i += nodeSize) {
		x = xLo + i * width;
		sum += f(x);
	}

	//calculate the area of all
	sum *= width; //multiply all by width

	return sum;
} //Trapezoid guided schedule

double f(double x) {
	return exp(-0.1 * x) * sin(5 * x);
}

double integrate(double xLo, double xHi) {
  return ((exp(-xHi / 10) * (10 * sin(5 * xHi) + 500 * cos(5 * xHi))) / -2501) - ((exp(-xLo / 10) * (10 * sin(5 * xLo) + 500 * cos(5 * xLo))) / -2501);
  //double a = -0.1;
  //double b = 5;

  //  double answerL = (exp(a*xLo))*(a*sin(b*xLo)-(b*cos(b*xLo)))/(a*a+b*b);
  //  double answerH = (exp(a*xHi))*(a*sin(b*xHi)-(b*cos(b*xHi)))/(a*a+b*b);

  //return answerH - answerL;

}
