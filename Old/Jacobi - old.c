#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <math.h>


#ifndef _TIMER_H
#define _TIMER_H_

#include <sys/time.h>

#define GET_TIME(now){ \
	struct timeval t; \
	gettimeofday(&t, NULL); \
	now = t.tv_sec + t.tv_usec/1000000.0; \
}

#endif

void JacobiDynamicMethod();
void Swap();
void SetCell(int row, int col, float f);
float GetCell(int row, int col);

const int maxRows = 1000; //max number of rows (up and down)
const int maxCols = 1500; //max number of columns (left and right)
double maxDiff, diff = 0; //used to store difference between 2 generations
double startTime = 0, endTime = 0, timeTaken = 0, totalTimeTaken = 0;
int i, j;
int iStart = 1;
int iEnd = maxCols;
int tempCold = 0; //cold blue
int tempMedium = 128; //warm pruple
int tempHot = 255; //hot red
float nodeSizeInner; //the inner nodes need 2 additonal rows for sharing
float nodeSizeOuter; //the outer nodes need only 1 additional row for sharing
float epsilon = 0.001; //required minimum difference between 2 generations
float *masterArray;
float *array;
float *arrayNew;

MPI_Status status;
	
int rank;
int world_size;

int main() {
    	MPI_Init(NULL, NULL);
    	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    	MPI_Comm_size (MPI_COMM_WORLD, &world_size);
	
	nodeSizeInner = maxRows * maxCols / world_size + maxCols * 2; //the inner nodes need 2 additonal rows for sharing
	nodeSizeOuter = maxRows * maxCols / world_size + maxCols; //the outer nodes need only 1 additional row for sharing

	JacobiDynamicMethod(); //THE HEART OF THE PROGRAM
	printf("Hello from node %d !!", rank);
	MPI_Finalize();
printf("HELLO FROM AFTER EVERYTHING");
	printf("P3\n");
	printf("%d %d\n", maxCols, maxRows);
	printf("#Ian Spryn, Mitchell Harvey, Emily Wasylenko, Kaileigh MacLeod ~ COMP322.A MPP Capstone\n");
	printf("255\n");
	for (i = 0; i < maxRows * maxCols; i++) {
		float red = *(masterArray + i);
		float blue = 255 - *(masterArray + i);
		printf("%f 0 %f", red, blue);
	}
	return 0;

}

//swap 2 Jacobi objects in their entirety
void Swap() {
	float* heatArrayTemp = array;
	array = arrayNew;
	arrayNew = heatArrayTemp;
}

//set a cell in the array to a given value
void SetCell(int row, int col, float f) {
	*(arrayNew + col + row * maxCols) = f;
}

//get cell in the array
float GetCell(int row, int col) {
	return *(array + col + row * maxCols);
}

void JacobiDynamicMethod() {
	for (i = 0; i < 3; i++) {
		GET_TIME(startTime);
		//create proper size of array depending on which node I am
		if (rank == 0) {
			masterArray = (float*)malloc(maxRows * maxCols * sizeof(float));
		}
		if (rank == 0 || rank == world_size - 1) {
			array = (float*)malloc(nodeSizeOuter * sizeof(float));
			arrayNew = (float*)malloc(nodeSizeOuter * sizeof(float));
		} else {
			array = (float*)malloc(nodeSizeInner * sizeof(float));
			arrayNew = (float*)malloc(nodeSizeInner * sizeof(float));
		}

		//do we need this for loop if we're running the 3 for loops right below?
		for (i = 0; i < maxRows * maxCols; i++) {
			*(masterArray + i) = tempCold;
		}

		//for the node with the top part of the board and the node with the bottom part of the board
		iStart = 1;
		iEnd = maxRows;
		if (rank == 0) {
			iStart++;
		}
		if (rank == world_size - 1) {
			iEnd--;
		}

		//right side
		for (i = 0; i < maxRows; i++) {
			*(masterArray + j + i * maxCols) = tempHot;
		}

		//top side
		for (j = 0; j < maxCols; j++) {
			*(masterArray + j + i * maxCols) = tempHot;
		}

		//middle
		for (i = 1; i < maxRows - 1; i++) {
			for (j = 1; j < maxCols - 1; j++) {
				*(masterArray + j + i * maxCols) = tempMedium;
			}
		}
		
		MPI_Scatter(masterArray, world_size, MPI_FLOAT, array, maxRows * maxCols / 4, MPI_FLOAT, 0, MPI_COMM_WORLD);

		//if my plate is not the very top part, then shift down by one row
		if (rank != world_size - 1) {
			float *tempArray = malloc(nodeSizeInner * sizeof(float));
			for (i = 0; i < maxRows * maxCols / world_size; i++) {
				*(tempArray + i) = *(array + i);
			}
			//put everything back in, but now one down
			for (i = 0; i < maxRows * maxCols / world_size; i++) {
				*(array + i + maxCols) = *(tempArray + i);
			}
			free(tempArray);
		}

		while (maxDiff > epsilon) {
			//only send to the node above me if I am NOT the top node
			if (rank < world_size - 1) {
				//MPI_Send((array + maxCols / world_size), maxCols, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
				MPI_Send((array + (maxRows / world_size - 1) * maxCols), maxCols, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
			}
			if (rank > 0) {
				MPI_Recv(array, maxCols, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
			}

			//only send to the node below me if I am NOT the bottom node
			if (rank > 0) {
				MPI_Send((array + maxCols), maxCols, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
			}
			if (rank < world_size - 1) {
				MPI_Recv((array + (maxRows / world_size - 1) * maxCols), maxCols, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);
			}
#pragma omp parallel num_threads(4) shared(maxDiff) firstprivate(diff)
{
#pragma omp for schedule(dynamic, 1)
			for (i = iStart; i < iEnd; i++) {
				for (j = 1; j < maxCols - 1; j++) {
					SetCell(i, j, ((GetCell(i, j - 1) + GetCell(i, j + 1) + GetCell(i - 1, j) + GetCell(i + 1, j)) / 4.0f));
					if (diff < fabs(GetCell(i, j) - *(arrayNew + j + i * maxCols))) {
						diff = fabs(GetCell(i, j) - *(arrayNew + j + i * maxCols)); //get max diff of each thread
					}
				}
			}

	#pragma omp critical
			{
				if (maxDiff < diff) {
					maxDiff = diff;
				}
			}
}

			Swap();
		}

		if (rank != world_size - 1) {
			float *tempArray = malloc(nodeSizeInner * sizeof(float));
			for (i = 0; i < maxRows * maxCols / world_size; i++) {
				*(tempArray + i) = *(array + i + maxCols);
			}
			//put everything back in, but now one up
			for (i = 0; i < maxRows * maxCols / world_size; i++) {
				*(array + i) = *(tempArray + i);
			}
			free(tempArray);
		}
		MPI_Gather(masterArray, world_size, MPI_FLOAT, array, maxRows * maxCols / 4, MPI_FLOAT, 0, MPI_COMM_WORLD);
		GET_TIME(endTime);
		totalTimeTaken += (double)(endTime - startTime);
	}
	timeTaken = totalTimeTaken / 3;
}