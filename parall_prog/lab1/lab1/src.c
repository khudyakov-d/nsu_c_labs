#include "src.h"

void scalarMulVectors(double * result, double value, double * vector, int size){

    for( int i = 0; i< size; i++){
        result[i] = vector[i]*value;
    }
}

double mulVectors(const double *vector1, const double *vector2, int size){
    double value = 0;

    for( int i = 0; i< size; i++){
        value += vector1[i]*vector2[i];
    }

    return value;

}

void mulMatrAndVec(double * result , const double * matrix , const double * vector,  int size){
    clearVec(result,size);
    for( int i = 0; i< size; i++){
        for ( int j = 0; j < size; j++){
            result[i] += matrix[i*size + j]*vector[j];
        }
    }
}

void subVectors(double* result, const double *vector1, const double *vector2, int size) {
    for( int i = 0; i< size; i++){
        result[i] = vector1[i] - vector2[i];
    }
}

void addVectors(double* result, const double *vector1, const double *vector2, int size) {
    for( int i = 0; i< size; i++){
        result[i] = vector1[i] + vector2[i];
    }
}

void setMatrix(double * matrix, int size){
    for( int i = 0; i< size; i++)
        for (int j = 0; j < size; j++){
            if (i==j) {
                matrix[i * size + j] = 2;
            }
            else
                matrix[i * size + j] = 1;
        }
}

void setVector(double * vector, int size, int value){
    for( int i = 0; i< size; i++){
        vector[i] = value;
    }
}

void showMatrix(double *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%.5f ", matrix[i * size + j]);
        }
        printf("\n");
    }

    printf("\n");
}

void showVector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
            printf("%.10f ", vector[i]);
     }
    printf("\n");
}

double calcAbsValue(const double *vector, int size) {
    double value = 0;

    for( int i = 0; i< size; i++){
        value += vector[i]*vector[i];
    }

    return sqrt(value);
}


void clearVec(double * vector , int size){
    for( int i = 0; i< size; i++){
        vector[i] = 0;
    }
}