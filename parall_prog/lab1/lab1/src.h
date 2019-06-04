//
// Created by daniel on 08.02.19.
//

#ifndef LAB1_SRC_H
#define LAB1_SRC_H

#include <stdlib.h>
#include <mpich/mpi.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>

void showVector(double *vector, int size);
void mulMatrAndVec(double * result , const double * matrix , const double * vector,  int size);
void setMatrix(double * matrix, int size);
void clearVec(double * vector , int size);
void setVector(double * vector, int size, int value);
void scalarMulVectors(double *result, double value, double * vector, int size);
double mulVectors(const double *vector1, const double *vector2, int size);
void subVectors(double* result, const double *vector1, const double *vector2, int size);
void addVectors(double* result, const double *vector1, const double *vector2, int size);
double calcAbsValue(const double *vector, int size);

#endif //LAB1_SRC_H
