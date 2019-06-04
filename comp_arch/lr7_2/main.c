#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cblas.h>
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

void setIdentityMatrix(float* matrix, int size) {
    size_t i, j;
    memset(matrix, 0, size * size * sizeof(float));
    for (i = 0; i < size; ++i) {
        matrix[i * (size + 1)] = 1;
    }
}

int main() {
    float * data = (float *)malloc(matrixSize*matrixSize*sizeof(float)*5);
    float * matrixA = data;
    float * matrixF = data + matrixSize*matrixSize;
    float * matrixR = data + matrixSize*matrixSize*2;
    float * matrixI = data + matrixSize*matrixSize*3;
    float * matrixS = data + matrixSize*matrixSize*4;
    float maxRow, maxColumn;

    struct tms start, end;
    long clocks_per_sec = sysconf(_SC_CLK_TCK);
    long clocks;

    times(&start);

    setMatrix(matrixA, matrixSize);
    maxRow = getMaxRowSum(matrixA, matrixSize);
    transposeMatrix(matrixS, matrixA , matrixSize);
    maxColumn = getMaxRowSum(matrixS, matrixSize);

    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, matrixSize, matrixSize, matrixSize,
                (float)1/(maxRow/maxColumn), matrixA, matrixSize, matrixA, matrixSize, 0, matrixR, matrixSize);


    setIdentityMatrix(matrixI, matrixSize);

    cblas_saxpy(matrixSize * matrixSize, -1.0f, matrixI, 1, matrixR, 1);
    memcpy(matrixS, matrixR, matrixSize * matrixSize * sizeof(float)); // S = R

    for (int i = 0; i < seriesMembers; ++i) {
        cblas_saxpy(matrixSize * matrixSize, 1.0f, matrixS, 1, matrixI, 1); // I = S*1 + I

        // F = S*R
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, matrixSize,
                matrixSize, matrixSize, 1.0f, matrixS, matrixSize, matrixR, matrixSize, 0, matrixF, matrixSize);

        // S = F
        matrixS = matrixF;
    }

    // F = A(T) * I
    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, matrixSize, matrixSize, matrixSize, 1.0f,
            matrixA, matrixSize, matrixI, matrixSize, 0, matrixF, matrixSize);

    times(&end);

    clocks = end.tms_utime - start.tms_utime;

    printf("Time %lf sec.\n", (double)clocks / clocks_per_sec);

    return 0;
}