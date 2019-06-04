#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <omp.h>

void clearVec(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = 0;
    }
}

void scalarMulVectors(double * result, double value, double * vector, int size){
#pragma omp parallel for
    for( int i = 0; i< size; i++){
        result[i] = vector[i]*value;
    }
}


void subVectors(double* result, const double *vector1, const double *vector2, int size) {
#pragma omp parallel for
    for( int i = 0; i< size; i++){
        result[i] = vector1[i] - vector2[i];
    }
}

void addVectors(double* result, const double *vector1, const double *vector2, int size) {
#pragma omp parallel for
    for( int i = 0; i< size; i++){
        result[i] = vector1[i] + vector2[i];
    }
}


double mulVectors(const double *vector1, const double *vector2, int size) {
    double value = 0;

    for (int i = 0; i < size; i++) {
        value += vector1[i] * vector2[i];
    }

    return value;

}


void mulMatrAndVec(double *result, const double *matrix, const double *vector, int size) {
    clearVec(result, size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            result[i] += matrix[i * size + j] * vector[j];
        }
    }
}


void mulMatrAndVecMP(double *result, const double *matrix, const double *vector, int place, int blockSize, int size) {
    for (int i = place; i < (place + blockSize); ++i) {
        for (int j = 0; j < size; ++j) {
            result[i] += matrix[i * size + j] * vector[j];
        }
    }
}


void setMatrix(double *matrix, int size) {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            if (i == j) {
                matrix[i * size + j] = 2;
            } else
                matrix[i * size + j] = 1;
        }
}

void setVector(double *vector, int size, int value) {
    for (int i = 0; i < size; ++i) {
        vector[i] = value;
    }
}

void showVector(double *vector, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%.10f ", vector[i]);
    }
    printf("\n");
}

double calcAbsValue(const double *vector, int size) {
    double value = 0;

    for (int i = 0; i < size; ++i) {
        value += vector[i] * vector[i];
    }

    return sqrt(value);
}


int main(int argc, char *argv[]) {
    struct timespec start, end;

    int N = 10000;
    const double e = 1E-8;

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

    int threadNum, threadCount, place, blocksize;

    setMatrix(A, N);
    setVector(x, N, 0);
    //setVector(b, N, N + 1);

    for (int i = 0; i < N; i++) {
        u[i] = sin((2 * 3.14 * i) / N);
    }
    mulMatrAndVec(b, A, u, N);

    showVector(u, N);

    threadCount = omp_get_max_threads();
    blocksize = N / threadCount;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    clearVec(vec1, N);

    #pragma omp parallel private(threadNum, place) shared(threadCount, vec1, A, z, blocksize)
    {
        threadNum = omp_get_thread_num();
        place = (threadNum * N) / threadCount;
        mulMatrAndVecMP(vec1, A, x, place, blocksize, N);
    }

    subVectors(r1, b, vec1, N);
    clearVec(vec1, N);

    memcpy(z, r1, N * sizeof(double));
    memcpy(r2, r1, N * sizeof(double));


    for(int i=0; i<40; i++) {
        //A*z^n
        #pragma omp parallel private(threadNum, place) shared(threadCount, vec1, A, z, blocksize)
        {
            threadNum = omp_get_thread_num();
            place = (threadNum * N) / threadCount;
            mulMatrAndVecMP(vec1, A, z, place, blocksize, N);
        }
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
        clearVec(vec1,N);
    }

    showVector(x, N);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Time taken: %lf sec.\n", end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));

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
