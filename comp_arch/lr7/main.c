#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/times.h> // for times
#include <unistd.h> // for sysconf


const int matrixSize = 2048;
const int seriesMembers = 10;

void setMatrix(float * matrix, int size){
    for( int i = 0; i< size; i++)
        for (int j = 0; j < size; j++)
            matrix[i * size + j] = (float) rand() / (float) RAND_MAX * 10;
}

float getMaxRowSum(float *matrix, int size) {
    float result = 0, sum = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sum += fabsf(matrix[i * size + j]);
        }
        result = sum > result ? sum : result;
    }

    return result;
}

void transposeMatrix(float *result, float *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[j * size + i] = matrix[i * size + j];
        }
    }
}

void mulScalar(float * matrix, float scalar , int size){
    for( int i = 0; i< size; i++)
        for (int j = 0; j < size; j++)
            matrix[i * size + j] *= scalar ;
}

void mulMatrices(float * result , float * matrix2 , float * matrix1,  int size){
    float sum = 0;
    int i,j,k;
    float * transpMatrix = malloc(matrixSize*matrixSize*sizeof(float));
    transposeMatrix(transpMatrix, matrix2, matrixSize);

    for( i = 0; i< size; i++){
        for ( j = 0; j < size; j++){
            sum = 0;
            for ( k = 0; k < size; k++)
                sum += matrix1[i * size + k] * transpMatrix[j * size + k];
            result[i*size + j] = sum;
        }
    }
}

void subMatrices(float* result, float *matrix1, float *matrix2, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            result[i * size + j] = matrix1[i * size + j] - matrix2[i * size + j];
        }
    }
}

void addMatrices(float* result, float *matrix1, float *matrix2, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            result[i * size + j] = matrix1[i * size + j] + matrix2[i * size + j];
        }
    }
}

void setIdentityMatrix(float* matrix, int size) {
    size_t i, j;
    memset(matrix, 0, size * size * sizeof(float));
    for (i = 0; i < size; ++i) {
        matrix[i * (size + 1)] = 1;
    }
}

void showMatrix(float *matrix, size_t size) {
    size_t i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            float k = matrix[i * size + j];
            printf("%.2f ", matrix[i * size + j]);
        }
        printf("\n");
    }

    printf("\n");
}

int main() {
    float * data = malloc(matrixSize*matrixSize*sizeof(float)*5);
    float * matrixA = data;
    float * matrixB = data + matrixSize*matrixSize;
    float * matrixR = data + matrixSize*matrixSize*2;
    float * matrixI = data + matrixSize*matrixSize*3;
    float * matrixS = data + matrixSize*matrixSize*4;
    float maxRow, maxColumn;

    struct tms start, end;
    long clocks_per_sec = sysconf(_SC_CLK_TCK);
    long clocks;

    times(&start);

    //установить B
    setMatrix(matrixA, matrixSize);
    maxRow = getMaxRowSum(matrixA, matrixSize);
    transposeMatrix(matrixB, matrixA , matrixSize);
    maxColumn = getMaxRowSum(matrixB, matrixSize);
    mulScalar(matrixB, (float)1/(maxRow*maxColumn), matrixSize);

    //установить R
    mulMatrices(matrixR,matrixA,matrixB,matrixSize);
    setIdentityMatrix(matrixI, matrixSize);
    subMatrices(matrixR, matrixI, matrixR, matrixSize);

    //Коипируем R
    memcpy(matrixA, matrixR, matrixSize * matrixSize * sizeof(float));

    //Считаем сумму ряда
    for (int i = 0; i < seriesMembers; i++) {
        addMatrices(matrixI, matrixI, matrixA, matrixSize);
        mulMatrices(matrixS, matrixA, matrixR, matrixSize);
        matrixA = matrixS;
    }
    mulMatrices(matrixA, matrixI, matrixB, matrixSize);

    times(&end);

    clocks = end.tms_utime - start.tms_utime;
    printf("Time taken: %lf sec.\n", (double)clocks / clocks_per_sec);

    return 0;
}
