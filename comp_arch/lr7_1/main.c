#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/times.h> // for times
#include <unistd.h> // for sysconf


const int matrixSize = 2048;
const int seriesMembers = 10;


void mulVectors(float* vector1, float* vector2, float* result) {
    asm volatile(
    "vmovups (%0), %%ymm0\n"
    "vmovups (%1), %%ymm1\n"
    "vmulps %%ymm1, %%ymm0, %%ymm0\n"
    "vmovups %%ymm0, (%2)"
    :
    : "r"(vector1), "r"(vector2), "r"(result)
    : "%ymm0", "%ymm1");
}

void addVectors(float* vector1, float* vector2, float* result) {
    asm volatile(
    "vmovups (%0), %%ymm0\n"
    "vmovups (%1), %%ymm1\n"
    "vaddps %%ymm1, %%ymm0, %%ymm0\n"
    "vmovups %%ymm0, (%2)"
    :
    : "r"(vector1), "r"(vector2), "r"(result)
    : "%ymm0", "%ymm1");
}

void subVectors(float* vector1, float* vector2, float* result) {
    asm volatile(
    "vmovups (%0), %%ymm0\n"
    "vmovups (%1), %%ymm1\n"
    "vsubps %%ymm1, %%ymm0, %%ymm0\n"
    "vmovups %%ymm0, (%2)"
    :
    : "r"(vector1), "r"(vector2), "r"(result)
    : "%ymm0", "%ymm1");
}

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
    float arrScalar[8] = {scalar , scalar , scalar , scalar , scalar , scalar , scalar , scalar  };
    for( int i = 0; i< size; i++)
        for (int j = 0; j < size; matrix +=j, j+=8) {
            mulVectors(matrix,arrScalar,matrix);
        }
}

void mulMatrices(float * result , float * matrix2 , float * matrix1,  int size){
    float curSum[8],resultSum;
    int i,j,k;
    float * transpMatrix = malloc(matrixSize*matrixSize*sizeof(float));

    transposeMatrix(transpMatrix, matrix2, matrixSize);

    for( i = 0; i< size; i++){
        for ( j = 0; j < size; j++){
            float sum[8] = {0};
            for ( k = 0; k < size; k+=8){
                mulVectors((matrix1+i*size+k),(transpMatrix+j*size+k),curSum);
                addVectors(sum,curSum,sum);
            }
            for (int r = 0; r< 8; r++ )
                resultSum += sum[r];

            result[i*size + j] = resultSum;
            resultSum = 0;
        }
    }
}

void subMatrices(float* result, float *matrix1, float *matrix2, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j+=8) {
            subVectors(matrix1 + i*size + j, matrix2 + i*size + j, result + i*size + j);
        }
    }
}

void addMatrices(float* result, float *matrix1, float *matrix2, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j+=8) {
            addVectors(matrix1 + i*size + j, matrix2 + i*size + j, result + i*size + j);
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
            printf("%.10f ", matrix[i * size + j]);
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
    showMatrix(matrixA, matrixSize);
    times(&end);

    clocks = end.tms_utime - start.tms_utime;
    printf("Time taken: %lf sec.\n", (double)clocks / clocks_per_sec);

    return 0;
}
