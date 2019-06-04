#include <time.h>
#include "src.h"

int main(int argc, char * argv[]) {
    int size, rank;

    struct timespec start, end;

    const int N = 3000;
    const double e = 0.000001;
    double *A = (double *) malloc(N * N * sizeof(double));
    double *x = (double *) malloc(N * sizeof(double));
    double *b = (double *) malloc(N * sizeof(double));
    double *u = (double *) malloc(N * sizeof(double));
    double *r1 = (double *) malloc(N * sizeof(double));
    double *r2 = (double *) malloc(N * sizeof(double));
    double *z = (double *) malloc(N * sizeof(double));
    double *vec1 = (double *) malloc(N * sizeof(double));

    double a = 0;
    double bt = 0;

    setMatrix(A, N);
    setVector(x, N, 0);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for(int i = 0; i < N; i++)
    {
        u[i] = sin((2*3.14*i)/N);
    }


    mulMatrAndVec(b, A, u, N);

    mulMatrAndVec(vec1, A, x, N);
    subVectors(r1, b, vec1, N);
    memcpy(z, r1, N * sizeof(double));
    memcpy(r2, r1, N * sizeof(double));




    while ((calcAbsValue(r2, N) / calcAbsValue(b, N)) > e) {

        mulMatrAndVec(vec1, A, z, N); //A*z^n
        a = mulVectors(r1, r1, N) / mulVectors(vec1, z, N); //a = "(r^n,r^n)/(A*z^n,z^n)"

        scalarMulVectors(vec1, a, z, N);

        addVectors(x, x, vec1, N);
        mulMatrAndVec(vec1, A, z, N); //A*z^n
        scalarMulVectors(vec1, a, vec1, N);
        subVectors(r2, r1, vec1, N);

        bt = mulVectors(r2, r2, N) / mulVectors(r1, r1, N);

        scalarMulVectors(vec1, bt, z, N);
        addVectors(z, r2, vec1, N);

        memcpy(r1, r2, N * sizeof(double));
    }
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Time taken: %lf sec.\n", end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec));

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
