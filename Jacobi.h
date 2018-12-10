//Ian Spryn ~ COMP322.A ~ Laplace Heat Distribution

#ifndef JACOBI_H
#define JACOBI_H
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <omp.h>

using namespace std;

float diff;
float maxDiff;

class Jacobi {
private:
	int maxRows, maxCols, initTemperature, threadCount;
	float* heatArray;
public:
	Jacobi(int maxRows, int maxCols, int initTemperature, int threadCount); //constructor
	~Jacobi(); //destructor
	Jacobi(const Jacobi &d); //copy constructor
	Jacobi& operator = (const Jacobi &d); //Assignment Operator overload
	void swap(Jacobi &d); //swap two Jacobi instances
	void setCell(int row, int col, float f); //set a cell in the array to a given value
	float getCell(int row, int col); //get a cell in the array
	float doOnePass(Jacobi &d); //generate the next geneartion of the Jacobi board and return the maximum change
	float doOnePassDefault(Jacobi &d); //generate the next geneartion of the Jacobi board and return the maximum change - DEFAULT
	float doOnePassStatic(Jacobi &d); //generate the next geneartion of the Jacobi board and return the maximum change - STATIC
	float doOnePassDynamic(Jacobi &d); //generate the next geneartion of the Jacobi board and return the maximum change - DYNAMIC
	float doOnePassGuided(Jacobi &d); //generate the next geneartion of the Jacobi board and return the maximum change - GUIDED
	void toPPM(const char* pointer); //generate a .ppm file
};


//default constructor
Jacobi::Jacobi(int maxRows, int maxCols, int initTemperature, int threadCount) {
	this->maxRows = maxRows;
	this->maxCols = maxCols;
	this->initTemperature = initTemperature;
	this->threadCount = threadCount;
	this->heatArray = (float*)malloc(maxCols * maxRows * sizeof(float));
	for (int i = 0; i < maxRows * maxCols; i++) {
		*(heatArray + i) = initTemperature;
	}
}

//destructor
Jacobi::~Jacobi() {
	free(heatArray);
}

//copy constructor
Jacobi::Jacobi(const Jacobi &d) {
	if (this != &d) {
		this->maxRows = d.maxRows;
		this->maxCols = d.maxCols;
		this->heatArray = (float*)malloc(maxCols * maxRows * sizeof(float));
		for (int i = 0; i < maxRows * maxCols; i++) {
			*(this->heatArray + i) = *(d.heatArray + i);
		}
	}
}

//assignment operator overload
Jacobi& Jacobi::operator = (const Jacobi &d) {
	if (this != &d) {
		free(heatArray);
		this->heatArray = (float*)malloc((d.maxRows * d.maxCols * sizeof(float)));
		this->maxRows = d.maxRows;
		this->maxCols = d.maxCols;
		for (int i = 0; i < maxRows * maxCols; i++) {
			*(this->heatArray + i) = *(d.heatArray + i);
		}
	}
	return *this;
}

//swap 2 Jacobi objects in their entirety
void Jacobi::swap(Jacobi &d) {
	float* heatArrayTemp = heatArray;
	heatArray = d.heatArray;
	d.heatArray = heatArrayTemp;

}

//set a cell in the array to a given value
void Jacobi::setCell(int row, int col, float f) {
	*(this->heatArray + col + row * maxCols) = f;
}

//get cell in the 2D array
float Jacobi::getCell(int row, int col) {
	return *(heatArray + col + row * maxCols);
}

//generate next instanace of a board and return maximum change value
float Jacobi::doOnePassDynamic(Jacobi &d) {
	diff = 0;
	maxDiff = 0;
#pragma omp parallel num_threads(threadCount) shared(maxDiff) firstprivate(diff)
	{
#pragma omp for schedule(dynamic, 1)
		for (int i = 1; i < d.maxRows - 1; i++) {
			for (int j = 1; j < d.maxCols - 1; j++) {
				d.setCell(i, j, ((getCell(i, j - 1) + getCell(i, j + 1) + getCell(i - 1, j) + getCell(i + 1, j)) / 4.0f));
				if (diff < fabs(getCell(i, j) - d.getCell(i, j))) {
					diff = fabs(getCell(i, j) - d.getCell(i, j)); //get max diff of each thread
				}
			}
			//MPI stuff here
		}
#pragma omp critical
		{
			if (maxDiff < diff) {
				maxDiff = diff;
			}
		}
	}
	return maxDiff;
}

//generate a .ppm file
void Jacobi::toPPM(const char* fileName) {
	int counter = 0;
	ofstream output(fileName);
	if (!output.is_open()) {
		cerr << "Cannot open file";
	}
	else {
		output << "P3" << endl;
		output << maxCols << " " << maxRows << endl;
		output << "# Ian Spryn ~ COMP322.A ~" << endl;
		output << "100" << endl;
		for (int i = 0; i < maxRows; i++) {
			for (int j = 0; j < maxCols; j++) {
				output << (int)getCell(i, j) << " 0 " << (int)(100 - getCell(i, j)) << endl;
			}
		}
		output.close();
	}
}

#endif