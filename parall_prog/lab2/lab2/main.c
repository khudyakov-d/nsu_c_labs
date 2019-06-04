#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>

struct timeTest{
    double time;
    int count;
};
typedef struct timeTest timeTest;

struct timespec start, end;
timeTest mulMatrAndVecTime = {0,0};
timeTest subVectorsTime = {0,0};
timeTest addVectorsTime = {0,0};
timeTest mulVectorsTime = {0,0};
timeTest scalarMulVectorsTime = {0,0};

void clearVec(double * vector , int size){
    for( int i = 0; i< size; i++){
        vector[i] = 0;
    }
}

void scalarMulVectors(double * result, double value, double * vector, int size){
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for( int i = 0; i< size; i++){
        result[i] = vector[i]*value;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    scalarMulVectorsTime.time += end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec);
    scalarMulVectorsTime.count++;
}

double mulVectors(const double *vector1, const double *vector2, int size){
    double value = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for( int i = 0; i< size; i++){
        value += vector1[i]*vector2[i];
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    mulVectorsTime.time += end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec);
    mulVectorsTime.count++;
    return value;

}

void mulMatrAndVec(double * result , const double * matrix , const double * vector,  int size, int flag){
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    clearVec(result,size);
    for( int i = 0; i< size; i++){
        for ( int j = 0; j < size; j++){
            result[i] += matrix[i*size + j]*vector[j];
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    if(flag!=0){
        mulMatrAndVecTime.time += end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec);
        mulMatrAndVecTime.count++;
    }
}


void subVectors(double* result, const double *vector1, const double *vector2, int size) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for( int i = 0; i< size; i++){
        result[i] = vector1[i] - vector2[i];
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    subVectorsTime.time += end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec);
    subVectorsTime.count++;
}

void addVectors(double* result, const double *vector1, const double *vector2, int size) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for( int i = 0; i< size; i++){
        result[i] = vector1[i] + vector2[i];
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    addVectorsTime.time += end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec);
    addVectorsTime.count++;
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



int main(int argc, char * argv[]) {
    struct timespec start1, end1;

    const int N = 10000;
    const double e = 0.00000001;
    double *A = (double *) malloc(N * N * sizeof(double));
    double *x = (double *) malloc(N * sizeof(double));
    double *b = (double *) malloc(N * sizeof(double));
    double *u = (double *) malloc(N * sizeof(double));
    double *r1 = (double *) malloc(N * sizeof(double));
    double *r2 = (double *) malloc(N * sizeof(double));
    double *z = (double *) malloc(N * sizeof(double));
    double *vec1 = (double *) malloc(N * sizeof(double));
    double *vec2 = (double *) malloc(N * sizeof(double));

    double a = 0;
    double bt = 0;

    setMatrix(A, N);
    setVector(x, N, 0);


    for(int i = 0; i < N; i++)
    {
        u[i] = sin((2*3.14*i)/N);
    }


    mulMatrAndVec(b, A, u, N, 0);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start1);


    mulMatrAndVec(vec1, A, x, N,1);
    subVectors(r1, b, vec1, N);
    memcpy(z, r1, N * sizeof(double));
    memcpy(r2, r1, N * sizeof(double));

    for(int i=0; i<40; i++) {
        //A*z^n
        mulMatrAndVec(vec1, A, z, N, 1);

        //a^n+1 = "(r^n,r^n)/(A*z^n,z^n)"
        a = mulVectors(r1, r1, N) / mulVectors(vec1, z, N);

        //a^(n+1)*z
        scalarMulVectors(vec2, a, z, N);

        //x^n+1=x^n-a^(n+1)*z
        addVectors(x, x, vec2, N);

        //a*A*z^n
        scalarMulVectors(vec1, a, vec1, N);

        //r^(n+1) = r^n-a*A*z^n
        subVectors(r2, r1, vec1, N);

        //a^n+1 = "(r^n+1,r^n+1)/(r^n,r^n)"
        bt = mulVectors(r2, r2, N) / mulVectors(r1, r1, N);

        //z^n+1 = r^n+1 +b^(n+1)*z^n
        scalarMulVectors(vec1, bt, z, N);
        addVectors(z, r2, vec1, N);

        memcpy(r1, r2, N * sizeof(double));
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end1);
    printf("Time taken: %lf sec.\n", end1.tv_sec-start1.tv_sec + 0.000000001*(end1.tv_nsec-start1.tv_nsec));
    printf("mulMartAndVectTime taken: %lf sec. | One operation time: %lf sec.\n", mulMatrAndVecTime.time,mulMatrAndVecTime.time/mulMatrAndVecTime.count);
    printf("subVectorsTime taken: %lf sec. | One operation time: %lf sec.\n", subVectorsTime.time, subVectorsTime.time/subVectorsTime.count);
    printf("addVectorsTime taken: %lf sec. | One operation time: %lf sec.\n", addVectorsTime.time, addVectorsTime.time/addVectorsTime.count);
    printf("mulVectorsTime taken: %lf sec. | One operation time: %lf sec.\n", mulVectorsTime.time, mulVectorsTime.time/mulVectorsTime.count);
    printf("scalarMulVectorsTime taken: %lf sec. | One operation time: %lf sec.\n", scalarMulVectorsTime.time, scalarMulVectorsTime.time/scalarMulVectorsTime.count);

    free(A);
    free(x);
    free(b);
    free(r1);
    free(r2);
    free(z);
    free(vec1);
    free(u);

    return 0;
}