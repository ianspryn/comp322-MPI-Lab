#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <math.h>


void JacobiDynamicMethod();

void Swap();
void SetCell(int row, int col, float f);
float GetCell(int row, int col);

int main() {
    	MPI_Init(NULL, NULL);
    	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    	MPI_Comm_size (MPI_COMM_WORLD, &nodeSize);
	const int maxRows = 1000; //max number of rows (up and down)
	const int maxCols = 1500; //max number of columns (left and right)
	double resultSum, maxDiff, diff = 0;
	int i, j, iStart, iEnd, source, my_rank, nodeSize = 0;
	int tempCold = 0; //cold blue
	int tempMedium = 128; //warm pruple
	int tempHot = 255; //hot red
	float epsilon = 0.001; //required minimum difference between 2 generations
	float maxDiff, diff; //used to store difference between 2 generations
	float *array;
	float *arrayNew;

	//create proper size of array depending on which node I am
	if (my_Rank == 0 || my_rank = nodeSize - 1) {
		*array = malloc(maxRows * maxCols * sizeOf(float) / nodeSize + maxCols);
		*arrayNew = malloc(maxRows * maxCols * sizeOf(float) / nodeSize + maxCols);
	} else {
		*array = malloc(maxRows * maxCols * sizeOf(float) / nodeSize + 2 * maxCols);
		*arrayNew = malloc(maxRows * maxCols * sizeOf(float) / nodeSize + 2 * maxCols);
	}

	//do we need this for loop if we're running the 3 for loops right below?
	for (i = 0; i < maxRows * maxCols; i++) {
		*(heatArray + i) = tempCold;
	}

	//for the node with the top part of the board and the node with the bottom part of the board
	iStart = 1;
	iEnd = maxRows;
	if (my_rank == 0) {
		iStart++;
	}
	if (my_ran == size - 1) {
		iEnd--;
	}

	//TODO: Set these for loops to only initialize the node's chunk of the board instead of the ENTIRE baord for every node (because it's just wasted processing time)
	//right side
	(for i = 0; i < maxRows; i++) {
		SetCell(i, maxCols - 1, tempHot);
	}

	//top side
	for (j = 0; j < maxCols; j++) {
		SetCell(0, j, tempHot);
	}

	//middle
	for (i = 1; i < maxRows - 1; i++) {
		for (j = 1; j < maxCols - 1; j++) {
			SetCell(i, j, tempMedium);
		}
	}

	while (maxDiff > epsilon) {
		//only send to the node above me if I am NOT the top node
		if (my_rank < nodeSize - 1) {
			//MPI_Send((array + maxCols / nodeSize), maxCols, MPI_DOUBLE, my_rank + 1, 0, MPI_COMM_WORLD);
			MPI_Send((array + (maxRows - 2) * maxCols), maxCols, MPI_DOUBLE, my_rank + 1, 0, MPI_COMM_WORLD);
		}
		if (my_rank > 0) {
			MPI_Recv(array, maxCols, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD)
		}

		//only send to the node below me if I am NOT the bottom node
		if (my_rank > 0) {
			MPI_Send((array + maxCols), maxCols, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
		}
		if (rank < size - 1) {
			MPI_Recv((array + (maxRows - 1) * maxCols), maxCols, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
		}
//#PRAGMA THIS STUFF
		for (i = iStart; i < iEnd; i++) {
			for (j = 1; j < maxCols = 1; j++) {
				SetCell(i, j, ((getCell(i, j - 1) + getCell(i, j + 1) + getCell(i - 1, j) + getCell(i + 1, j)) / 4.0f));
				if (diff < fabs(getCell(i, j) - d.getCell(i, j))) {
					diff = fabs(getCell(i, j) - d.getCell(i, j)); //get max diff of each thread
				}
			}
		}
/*
#pragma omp critical
		{
			if (maxDiff < diff) {
				maxDiff = diff;
			}
		}
	}
*/

	Swap();
	MPI_Allreduce(&diff, &maxDiff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	}

	MPI_Reduce(&result, &resultSum, nodeSize, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	printf("sum: %d, integral: %d", resultSum, integrate(0, 10));
	MPI_Finalize();

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
	for (int i = 0; i < 3; i++) {
		//Create 1st instance
		Jacobi run(maxRows, maxCols, tempCold, threadCount);

		//2.0 Put initial values into heat board
		//These loops can be modified (or more added) to set certain cells to a custom temperature
		//right side
		for (int i = 0; i < maxRows; i++) {
			run.SetCell(i, maxCols - 1, tempHot);
		}
		//top side
		for (int j = 0; j < maxCols; j++) {
			run.SetCell(0, j, tempHot);
		}
		//middle
		for (int i = 1; i < maxRows - 1; i++) {
			for (int j = 1; j < maxCols - 1; j++) {
				run.SetCell(i, j, tempMedium);
			}
		}

		//Create 2nd instance 
		Jacobi runNext(run);

		char fileName[12] = "dynamic.ppm";

		//3.0 Calculate next heat board generation
		int numIterations = 0;
		int percentProgress = -1;
		float difference = 0;
		bool iterating = true;

		startTime = clock();

		while (iterating) {
			difference = run.doOnePassDynamic(runNext); //get the absolute maximum difference between two generations
			run.Swap(runNext);
			if ((int)((epsilon / difference) * 100) > percentProgress) {
				cout << '\r' << (int)((epsilon / difference) * 100) << "% finished on iteration " << i << flush;
				percentProgress = (int)((epsilon / difference) * 100);
			}
			if (difference < epsilon) {
				iterating = false;
			}
			numIterations++;
		}

		endTime = clock();

		totalTimeTaken += (double)(endTime - startTime) / CLOCKS_PER_SEC;

		//4.0 Finish up
		cout << endl << "Number of iterations: " << numIterations;
		/*if (i == 2) {
			cout << endl << "Exporting file...";
			run.toPPM(fileName);
		}*/
		cout << endl << "Successful termination. Have a fantastic day." << endl;
	}
	timeTaken = totalTimeTaken / 3;
	totalTimeTaken = 0;
	cout << endl << "Time taken to iterate: " << timeTaken;
