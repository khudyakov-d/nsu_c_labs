#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>


void clearVec(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = 0;
    }
}

void scalarMulVectors(double *result, double value, double *vector, int size) {

    for (int i = 0; i < size; i++) {
        result[i] = vector[i] * value;
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
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i * size + j] * vector[j];
        }
    }
}

void subVectors(double *result, const double *vector1, const double *vector2, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = vector1[i] - vector2[i];
    }
}

void addVectors(double *result, const double *vector1, const double *vector2, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = vector1[i] + vector2[i];
    }
}

void setMatrix(double *matrix, int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (i == j) {
                matrix[i * size + j] = 2;
            } else
                matrix[i * size + j] = 1;
        }
}

void setVector(double *vector, int size, int value) {
    for (int i = 0; i < size; i++) {
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

    for (int i = 0; i < size; i++) {
        value += vector[i] * vector[i];
    }

    return sqrt(value);
}

void transposeMatrix(double *matrix, int size) {
    int value;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            value = matrix[i * size + j];
            matrix[i * size + j] = matrix[j * size + i];
            matrix[j * size + i] = value;
        }
    }
}

void mulMatrVecParal(double *result, const double *matrix, const double *vector, int size, int blockSize) {
    clearVec(result, size);
    int i, j;
    for (i = 0; i < blockSize; i++) {
        for (j = 0; j < size; j++) {
            result[j] += matrix[i * size + j] * vector[i];
        }

    }
}

int main(int argc, char *argv[]) {
    int size, rank;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int i = 0;
    const int N = 10000;
    const double e = 1E-8;// 0.0000001;

    double *A = NULL;

    if (rank == 0) {
        A = (double *) malloc(N * N * sizeof(double));
    }

    double *x = (double *) malloc(N * sizeof(double));
    double *b = (double *) malloc(N * sizeof(double));
    double *u = (double *) malloc(N * sizeof(double));
    double *r1 = (double *) malloc(N * sizeof(double));
    double *r2 = (double *) malloc(N * sizeof(double));
    double *z = (double *) malloc(N * sizeof(double));
    double *vec1 = (double *) calloc(N, sizeof(double));
    double *zPart = (double *) malloc((N / size) * sizeof(double));
    double *zRes = (double *) malloc((N) * sizeof(double));
    double *part = (double *) malloc((N * N / size) * sizeof(double));
    double *xPart = (double *) malloc((N / size) * sizeof(double));
    double *xRes = (double *) malloc((N) * sizeof(double));
    double *res = (double *) malloc((N * size) * sizeof(double));

    double a = 0;
    double bt = 0;

    const int partSize = N / size;
    const int matrPartSize = N * N / size;

    if (rank == 0) {
        for (i = 0; i < N; i++) {
            u[i] = sin((i * 2 * 3.14) / N);
        }
        setVector(x, N, 0);
        setMatrix(A, N);

        transposeMatrix(A, N);
        mulMatrAndVec(b, A, u, N);
    }


    start = MPI_Wtime();

    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, matrPartSize, MPI_DOUBLE, part, matrPartSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(x, partSize, MPI_DOUBLE, xPart, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	
    //A*x^0
    mulMatrVecParal(xRes, part, xPart, N, partSize);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(xRes, N, MPI_DOUBLE, res, N, MPI_DOUBLE, MPI_COMM_WORLD);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < size; ++j) {
            vec1[i] += res[i + j * N];
        }        
    }

    //r^0=b-Ax^0
    subVectors(r1, b, vec1, N);
   
    memcpy(z, r1, N * sizeof(double));
    memcpy(r2, r1, N * sizeof(double));

    MPI_Allgather(xPart, partSize, MPI_DOUBLE, x, partSize, MPI_DOUBLE, MPI_COMM_WORLD);
		
   for(int i=0; i<20; i++) {
        MPI_Scatter(z, partSize, MPI_DOUBLE, zPart, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //A*z^n
        mulMatrVecParal(zRes, part, zPart, N, partSize);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(zRes, N, MPI_DOUBLE, res, N, MPI_DOUBLE, MPI_COMM_WORLD);

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < size; ++j) {
                vec1[i] += res[i + j * N];
            }
        }
        //a^n+1 = "(r^n,r^n)/(A*z^n,z^n)"
        a = mulVectors(r1, r1, N) / mulVectors(vec1, z, N);

        //a^(n+1)*z
        scalarMulVectors(vec1, a, z, N);


        addVectors(x, x, vec1, N);
		//showVector(x,N);
        
        //A*z^n

		mulMatrVecParal(zRes, part, zPart, N, partSize);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(zRes, N, MPI_DOUBLE, res, N, MPI_DOUBLE, MPI_COMM_WORLD);
        clearVec(vec1,N);
        
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < size; ++j) {
                vec1[i] += res[i + j * N];
            }
        }


        //a*A*z^n
        scalarMulVectors(vec1, a, vec1, N);

        //r^(n+1) = r^n-a*A*z^n
        subVectors(r2, r1, vec1, N);

        //b^n+1 = "(r^n+1,r^n+1)/(r^n,r^n)"
        bt = mulVectors(r2, r2, N) / mulVectors(r1, r1, N);

        //z^n+1 = r^n+1 +b^(n+1)*z^n
        scalarMulVectors(vec1, bt, z, N);
        addVectors(z, r2, vec1, N);

        memcpy(r1, r2, N * sizeof(double));
    }

    end = MPI_Wtime();

    if (rank == 0)
        printf("Time: %f \n ", end - start);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) free(A);
    free(x);
    free(b);
    free(r1);
    free(r2);
    free(z);
    free(vec1);
    free(zPart);
    free(zRes);
    free(xPart);
    free(xRes);
    free(part);
    free(res);
    free(u);

    MPI_Finalize();
    return 0;
}
