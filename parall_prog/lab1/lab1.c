#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>



void clearVec(double * vector , int size){
    for( int i = 0; i< size; i++){
        vector[i] = 0;
    }
}

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

void mulMatrAndVecMPI(double * result , const double * matrix , const double * vector,  int size, int row, int blockSize){
    clearVec(result,size);
    int i, j;
    for(i = 0 ; i< blockSize; i++){
        for (j = 0 ; j < size; j++){
            result[row] += matrix[i*size +j]*vector[j];
        }
        row++;
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

int main(int argc, char * argv[]) {
    int size, rank;
    double start,end;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int i = 0;
    const int N = 5000;
    const double e = 1E-8;// 0.0000001;
    /*
    double A[N*N];
    double x[N];
    double xPart[N/size];
    double xRes[N/size];
    double b[N];
    double u[N];
    double r1[N];
    double r2[N];
    double zPart[N];
    double z[N];
    double vec1[N];
    double part[N*N/size];
    */

    double *A = NULL;
    if (rank == 0 ){
        A = (double *) malloc(N * N * sizeof(double));
    }
    //A = (double *) malloc(N * N * sizeof(double));
    double *x = (double *) malloc(N * sizeof(double));
    double *b = (double *) malloc(N * sizeof(double));
    double *u = (double *) malloc(N * sizeof(double));
    double *r1 = (double *) malloc(N * sizeof(double));
    double *r2 = (double *) malloc(N * sizeof(double));
    double *z = (double *) malloc(N * sizeof(double));
    double *vec1 = (double *) malloc(N * sizeof(double));
    double *zPart = (double *) malloc(N * sizeof(double));
    double *xPart = (double *) malloc((N/size)* sizeof(double));
    double *xRes = (double *) malloc((N/size)* sizeof(double));
    double *part = (double *) malloc((N*N/size)* sizeof(double));



    double a = 0;
    double bt = 0;

    const int rowNumber =rank*N/size;
    const int zPartSize = N/size;
    const int zSize = N*N/size;



    start = MPI_Wtime();
    if (rank == 0 )
        setMatrix(A, N);
    setVector(x, N, 0);
    setVector(b, N, N+1);

    /*
    for(i; i < N; i++)
    {
        u[i] = sin(i*2*3,14/N);
    }
    mulMatrAndVec(b, A, u, N);
    */

    MPI_Scatter(A,zSize,MPI_DOUBLE,part,zSize,MPI_DOUBLE,0,MPI_COMM_WORLD);
    //MPI_Scatter(x,zPartSize,MPI_DOUBLE,xPart,zPartSize,MPI_DOUBLE,0,MPI_COMM_WORLD);

    //A*x^0
    mulMatrAndVecMPI(zPart, part, x, N,rowNumber,zPartSize);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(&zPart[rowNumber],zPartSize,MPI_DOUBLE,vec1,zPartSize,MPI_DOUBLE,MPI_COMM_WORLD);

    /*mulMatrAndVec(xRes, part, xPart,zPartSize);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(xRes,zPartSize,MPI_DOUBLE,x,zPartSize,MPI_DOUBLE,MPI_COMM_WORLD);
    */

    //r^0=b-Ax^0
    subVectors(r1, b, vec1, N);
    memcpy(z, r1, N * sizeof(double));
    memcpy(r2, r1, N * sizeof(double));

    while ((calcAbsValue(r2, N) / calcAbsValue(b, N)) > e) {
        //A*z^n
        mulMatrAndVecMPI(zPart, part, z, N,rowNumber,zPartSize);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(&zPart[rowNumber],zPartSize,MPI_DOUBLE,vec1,zPartSize,MPI_DOUBLE,MPI_COMM_WORLD);

        //a^n+1 = "(r^n,r^n)/(A*z^n,z^n)"
        a = mulVectors(r1, r1, N) / mulVectors(vec1, z, N);

        //a^(n+1)*z
        scalarMulVectors(vec1, a, z, N);

        //x^n+1 = x^n + a^(n+1)*z^n
        addVectors(x, x, vec1, N);

        //A*z^n
        mulMatrAndVecMPI(zPart, part, z, N,rowNumber,zPartSize);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(&zPart[rowNumber],zPartSize,MPI_DOUBLE,vec1,zPartSize,MPI_DOUBLE,MPI_COMM_WORLD);

        //a*A*z^n
        scalarMulVectors(vec1, a, vec1, N);

        //r^(n+1) = r^n-a*A*z^n
        subVectors(r2, r1, vec1, N);

        //a^n+1 = "(r^n+1,r^n+1)/(r^n,r^n)"
        bt = mulVectors(r2, r2, N) / mulVectors(r1, r1, N);

        //z^n+1 = r^n+1 +B^(n+1)*z^n
        scalarMulVectors(vec1, bt, z, N);
        addVectors(z, r2, vec1, N);

        memcpy(r1, r2, N * sizeof(double));
    }

    //showVector(x,N);
    end = MPI_Wtime();
    if (rank == 0)
        printf("Time: %f \n ",end - start);
    MPI_Finalize();
    return 0;
}
