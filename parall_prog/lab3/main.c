#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define DIMENSION 2

void setType(int count, int blockLen, int stride, MPI_Datatype *type) {
    long lbTypeDouble, extentDouble;

    MPI_Type_vector(count, blockLen, stride, MPI_DOUBLE, type);
    MPI_Type_get_extent(MPI_DOUBLE, &lbTypeDouble, &extentDouble);
    MPI_Type_create_resized(*type, 0, extentDouble * blockLen, type);
    MPI_Type_commit(type);
}

void mulMatrices(double *result, double *matrix1, double *matrix2, int sizeN, int sizeM, int sizeK) {
    for (int i = 0; i < sizeN; ++i) {
        for (int j = 0; j < sizeK; ++j) {
            for (int k = 0; k < sizeM; ++k) {
                result[i * sizeK + j] += matrix1[i * sizeM + k] * matrix2[sizeK * k + j];
            }
        }
    }
}

void setMatrix(double *matrix, int size1, int size2, int value) {
    for (int i = 0; i < size1; ++i)
        for (int j = 0; j < size2; ++j) {
            matrix[i * size2 + j] = value;
        }
}

void showVector(double *vector, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%.10f ", vector[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    int size, rank;
    double start, end;

    MPI_Datatype typeB, typeC;
    MPI_Comm comm2D = {0}, comm_1D[DIMENSION];

    const int N = 1024, M = 1024, K = 1024;
    int localN, localK;
    int err = 0;
    
    int p[DIMENSION], periods[DIMENSION] = {0}, coords[DIMENSION], remains[DIMENSION] = {0};
    int *offsetB = NULL, *countB = NULL, *offsetC = NULL, *countC = NULL;

    double *A = NULL, *B = NULL, *C = NULL;
    double *partA, *partB, *partC;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc == 3){
        p[0] = atoi(argv[1]);
        p[1] = atoi(argv[2]);

        if (p[0]*p[1] != size){
            printf("Error. Wrong size of lattice");
            MPI_Abort(MPI_COMM_WORLD, err);
            return 1;
        }
    }
    else {
        p[0] = 1;
        p[1] = size;
    }

    if (0 == rank) {
        A = (double *) malloc(N * M * sizeof(double));
        B = (double *) malloc(M * K * sizeof(double));
        C = (double *) malloc(M * K * sizeof(double));

        if (A == NULL || B == NULL || C == NULL) {
            printf("Allocation fail!");
            MPI_Abort(MPI_COMM_WORLD, err);
            return 1;
        }

        setMatrix(A, N, M, 1);
        setMatrix(B, M, K, 2);
        setMatrix(C, N, K, 0);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //Сreation of a lattice p[0] * p[1]
    err = MPI_Cart_create(MPI_COMM_WORLD, DIMENSION, p, periods, 0, &comm2D);

    if (0 != err) {
        printf("MPI_Cart_create error #%d", err);
		MPI_Abort(MPI_COMM_WORLD, err);    
        return 1;
    }

/*
        2*2
        (1 | 3)
        (2 | 4)

        comm_1D[0]:        2*1  1*2
        remains[0] = 0     (1)  (3)
        remains[1] = 1     (2)  (4)

        comm_1D[1]:
        remains[0] = 1     1*2 (1 | 3)
        remains[1] = 0     1*2 (2 | 4)
*/


    for (int i = 0; i < DIMENSION; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            remains[j] = (int) (i == j);
        }

        MPI_Cart_sub(comm2D, remains, &comm_1D[i]);
    }


    localN = N / p[0];
    localK = K / p[1];

    partA = (double *) malloc(localN * M * sizeof(double));
    partB = (double *) malloc(M * localK * sizeof(double));
    partC = (double *) malloc(localN * localK * sizeof(double));

    if (partA == NULL || partB == NULL || partC == NULL) {
        printf("Allocation fail!");
        return 1;
    }

    if (0 == rank) {
        //set type for partB group  with size localN * M
        setType(M, localK, K, &typeB);
        //set type for partC with size localN * localK
        setType(localN, localK, K, &typeC);
    }

    MPI_Comm_rank(comm2D, &rank);
    MPI_Cart_coords(comm2D, rank, DIMENSION, coords);

    //-------------------------MATRIX MULTIPLICATION-------------------------------------
    start = MPI_Wtime();

    // The matrix is ​scattered in horizontal stripes along the coordinates (x, 0)
    if (0 == coords[1]) {
        MPI_Scatter(A, localN * M, MPI_DOUBLE, partA, localN * M, MPI_DOUBLE, 0, comm_1D[0]);
    }

    // The matrix is ​scattered in vertical stripes along the coordinates (0,y)
    if (0 == coords[0]) {
        offsetB = (int *) malloc(p[1] * sizeof(double));
        countB = (int *) malloc(p[1] * sizeof(double));

        for (int i = 0; i < p[1]; ++i) {
            offsetB[i] = i;
            countB[i] = 1;
        }
        MPI_Scatterv(B, countB, offsetB, typeB, partB, localK * M, MPI_DOUBLE, 0, comm_1D[1]);
    }

    //A's lines are distributed in the dimension of X
    MPI_Bcast(partA, localN * M, MPI_DOUBLE, 0, comm_1D[1]);

    //A's lines are distributed in the dimension of Y
    MPI_Bcast(partB, localK * M, MPI_DOUBLE, 0, comm_1D[0]);

    //Multiplication, getting part of the matrix C
    mulMatrices(partC, partA, partB, localN, M, localK);


    MPI_Barrier(comm2D);

    offsetC = (int *) malloc(p[0] * p[1] * sizeof(double));
    countC = (int *) malloc(p[0] * p[1] * sizeof(double));

    for (int i = 0; i < p[0]; ++i) {
        for (int j = 0; j < p[1]; ++j) {
            offsetC[i * p[1] + j] = p[1] * localN * i + j;
            countC[i * p[1] + j] = 1;
        }
    }

    MPI_Barrier(comm2D);
    //Gathering
    MPI_Gatherv(partC, localK * localN, MPI_DOUBLE, C, countC, offsetC, typeC, 0, comm2D);

    end = MPI_Wtime();
    //-------------------------------------------------------------------------------------------------

    if (rank == 0)
        printf("Time: %f \n", end - start);

    if (0 == rank) {
        showVector(C, 10);
    }

    MPI_Barrier(comm2D);

    free(A);
    free(B);
    free(C);

    free(partA);
    free(partB);
    free(partC);

    free(offsetB);
    free(offsetC);
    free(countB);
    free(countC);

    if( 0 == rank){
        MPI_Comm_free(&comm2D);
        MPI_Comm_free(&comm_1D[0]);
        MPI_Comm_free(&comm_1D[1]);

        MPI_Type_free(&typeB);
        MPI_Type_free(&typeC);
    }

    MPI_Finalize();
    return 0;
}
