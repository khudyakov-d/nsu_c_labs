#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <mpi.h>

#define DIMENSION 3

#define sizeX 270
#define sizeY 270
#define sizeZ 270


double trueFunction(double x, double y, double z) {
    return (x * x + y * y + z * z);
}

double rightFunction(double x, double y, double z, double a) {
    return 6 - a * trueFunction(x, y, z);
}

double square(double x) {
    return x * x;
}

void fillBorders(double *funcResults, const double *segmentsLength, const double *bordersX,
                 const double *bordersY, const double *bordersZ) {
    double *matrix;

    matrix = funcResults;
    for (int k = 0; k < sizeZ; ++k) {
        for (int j = 0; j < sizeY; ++j) {
            *(matrix + j + k * sizeX * sizeY) = trueFunction(bordersX[0], bordersY[0] + j * segmentsLength[1],
                                                             bordersZ[0] + k * segmentsLength[2]);
        }
    }


    matrix = funcResults + (sizeY - 1) * sizeY;
    for (int k = 0; k < sizeZ; ++k) {
        for (int j = 0; j < sizeY; ++j) {
            *(matrix + j + k * sizeX * sizeY) = trueFunction(bordersX[1], bordersY[0] + j * segmentsLength[1],
                                                             bordersZ[0] + k * segmentsLength[2]);
        }
    }

    matrix = funcResults;
    for (int k = 0; k < sizeZ; ++k) {
        for (int i = 0; i < sizeX; ++i) {
            *(matrix + i * sizeY + k * sizeX * sizeY) = trueFunction(bordersX[0] + i * segmentsLength[0], bordersY[0],
                                                                     bordersZ[0] + k * segmentsLength[2]);
        }
    }

    matrix = funcResults + (sizeY - 1);
    for (int k = 0; k < sizeZ; ++k) {
        for (int i = 0; i < sizeX; ++i) {
            *(matrix + i * sizeY + k * sizeX * sizeY) = trueFunction(bordersX[0] + i * segmentsLength[0], bordersY[1],
                                                                     bordersZ[0] + k * segmentsLength[2]);
        }
    }


    matrix = funcResults;
    for (int i = 0; i < sizeX; ++i) {
        for (int j = 0; j < sizeY; ++j) {
            matrix[i * sizeY + j] = trueFunction(bordersX[0] + i * segmentsLength[0],
                                                 bordersY[0] + j * segmentsLength[1],
                                                 bordersZ[0]);
        }
    }

    matrix = funcResults + (sizeZ - 1) * sizeX * sizeY;
    for (int i = 0; i < sizeX; ++i) {
        for (int j = 0; j < sizeY; ++j) {
            matrix[i * sizeY + j] = trueFunction(bordersX[0] + i * segmentsLength[0],
                                                 bordersY[0] + j * segmentsLength[1],
                                                 bordersZ[1]);
        }
    }
}

double approximationFunction(double x1, double x2, double y1, double y2, double z1, double z2, double a,
                             double p, const double *segmentsLength) {

    double squareX = square(segmentsLength[0]);
    double squareY = square(segmentsLength[1]);
    double squareZ = square(segmentsLength[2]);

    double C = 1 / (2 / squareX + 2 / squareY
                    + 2 / squareZ + a);

    return C * (((x1 + x2) / squareX + (y1 + y2) / squareY + (z1 + z2) / squareZ) - p);
}

double findMax(const double *funcResults, const double *prevFuncResults, int localSize, int rank, int size) {
    double max = 0, C = 0;
    int leftBorder, rightBorder;

    if (size > 1) {
        if (rank == 0) {
            leftBorder = 1;
            rightBorder = localSize;
        } else if (rank == size - 1) {
            leftBorder = 0;
            rightBorder = localSize - 1;
        } else {
            leftBorder = 0;
            rightBorder = localSize;
        }
    } else {
        leftBorder = 1;
        rightBorder = sizeZ - 1;
    }


    for (int k = leftBorder; k < rightBorder; ++k) {
        for (int i = 1; i < sizeX - 1; ++i) {
            for (int j = 1; j < sizeY - 1; ++j) {
                C = fabs(*(funcResults + j + i * sizeY + k * sizeX * sizeY) -
                         *(prevFuncResults + j + i * sizeY + k * sizeX * sizeY));
                if (C > max) {
                    max = C;
                }
            }
        }
    }
    return C;
}


void showGrid(const double *funcResults, const double *segmentsLength, const double *bordersX,
              const double *bordersY, const double *bordersZ) {


    for (int k = 0; k < sizeZ; ++k) {
        for (int i = 0; i < sizeX; ++i) {
            for (int j = 0; j < sizeY; ++j) {
                printf("(%.3f.%.3f.%.3f):%.3f ", bordersX[0] + i * segmentsLength[0],
                       bordersY[0] + j * segmentsLength[0], bordersZ[0] + k * segmentsLength[0],
                       *(funcResults + j + i * sizeY + k * sizeX * sizeY));
            }
            printf("\n");
        }
        printf("\n");
    }
}

void calcLayer(double *funcResults, const double *segmentsLength, const double *bordersX,
               const double *bordersY, const double *bordersZ, double a, int k) {
    double p;

    for (int i = 1; i < sizeX - 1; ++i) {
        for (int j = 1; j < sizeY - 1; ++j) {
            p = rightFunction(
                    bordersX[0] + i * segmentsLength[0],
                    bordersY[0] + j * segmentsLength[1],
                    bordersZ[0] + k * segmentsLength[2],
                    a);


            *(funcResults + j + i * sizeY + k * sizeX * sizeY + sizeX * sizeY) =
                    approximationFunction(
                            *(funcResults + j + (i - 1) * sizeY + k * sizeX * sizeY + sizeX * sizeY),
                            *(funcResults + j + (i + 1) * sizeY + k * sizeX * sizeY + sizeX * sizeY),
                            *(funcResults + (j - 1) + i * sizeY + k * sizeX * sizeY + sizeX * sizeY),
                            *(funcResults + (j + 1) + i * sizeY + k * sizeX * sizeY + sizeX * sizeY),
                            *(funcResults + j + i * sizeY + (k - 1) * sizeX * sizeY + sizeX * sizeY),
                            *(funcResults + j + i * sizeY + (k + 1) * sizeX * sizeY + sizeX * sizeY),
                            a,
                            p,
                            segmentsLength);

        }
    }
}

void calcFunctionValues(double *funcResults, const double *segmentsLength, const double *bordersX,
                        const double *bordersY, const double *bordersZ, double a, double e,
                        const int localSize, int rank, int size) {

    MPI_Request requestsTop[2];
    MPI_Request requestsMiddle[4];
    MPI_Request requestsBottom[2];

    MPI_Status statusesTop[2];
    MPI_Status statusesMiddle[4];
    MPI_Status statusesBottom[2];

    double *prevFuncResults = (double *) malloc(sizeX * sizeY * localSize * sizeof(double));
    double threadMax = 0, max = 0;
    double maxArray[size];

    int leftBorder, rightBorder;

    double *bottomLayer = (double *) malloc(sizeX * sizeY * sizeof(double));
    double *topLayer = (double *) malloc(sizeX * sizeY * sizeof(double));

    leftBorder = 1;
    rightBorder = localSize - 1;

    do {
        max = 0;
        memcpy(prevFuncResults, funcResults + sizeX * sizeY, sizeX * sizeY * sizeof(double) * localSize);

        if (size > 1) {
            if (rank == 0) {
                MPI_Isend(funcResults + sizeX * sizeY * localSize, sizeX * sizeY, MPI_DOUBLE,
                          rank + 1, 1, MPI_COMM_WORLD, &requestsBottom[0]);
                MPI_Irecv(topLayer, sizeX * sizeY, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD, &requestsBottom[1]);


            } else if (rank == size - 1) {
                MPI_Isend(funcResults + sizeX * sizeY, sizeX * sizeY, MPI_DOUBLE,
                          rank - 1, 2, MPI_COMM_WORLD, &requestsTop[0]);
                MPI_Irecv(bottomLayer, sizeX * sizeY, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &requestsTop[1]);

            } else {
                MPI_Isend(funcResults + sizeX * sizeY * localSize, sizeX * sizeY, MPI_DOUBLE,
                          rank + 1, 1, MPI_COMM_WORLD, &requestsMiddle[0]);
                MPI_Irecv(topLayer, sizeX * sizeY, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD, &requestsMiddle[1]);

                MPI_Isend(funcResults + sizeX * sizeY, sizeX * sizeY, MPI_DOUBLE,
                          rank - 1, 2, MPI_COMM_WORLD, &requestsMiddle[2]);
                MPI_Irecv(bottomLayer, sizeX * sizeY, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &requestsMiddle[3]);

                MPI_Waitall(4, requestsMiddle, NULL);

                memcpy(funcResults + (localSize + 1) * sizeX * sizeY, topLayer, sizeX * sizeY * sizeof(double));
                memcpy(funcResults, bottomLayer, sizeX * sizeY * sizeof(double));

            }
        }

        for (int k = leftBorder; k < rightBorder; ++k) {
            calcLayer(funcResults, segmentsLength, bordersX, bordersY, bordersZ, a, k);
        }


        if (size > 1) {

            if (rank == 0) {
                MPI_Waitall(2, requestsBottom, statusesBottom);
                memcpy(funcResults + (localSize + 1) * sizeX * sizeY, topLayer, sizeX * sizeY * sizeof(double));
                calcLayer(funcResults, segmentsLength, bordersX, bordersY, bordersZ, a, 0);

            } else if (rank == size - 1) {
                MPI_Waitall(2, requestsTop, statusesTop);
                memcpy(funcResults, bottomLayer, sizeX * sizeY * sizeof(double));
                calcLayer(funcResults, segmentsLength, bordersX, bordersY, bordersZ, a, localSize);

            } else {
                MPI_Waitall(4, requestsMiddle, statusesMiddle);

                memcpy(funcResults + (localSize + 1) * sizeX * sizeY, topLayer, sizeX * sizeY * sizeof(double));
                memcpy(funcResults, bottomLayer, sizeX * sizeY * sizeof(double));

                calcLayer(funcResults, segmentsLength, bordersX, bordersY, bordersZ, a, 0);
                calcLayer(funcResults, segmentsLength, bordersX, bordersY, bordersZ, a, localSize);
            }
        }

        threadMax = findMax(funcResults + sizeY * sizeX, prevFuncResults, localSize, rank, size);


        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(&threadMax, 1, MPI_DOUBLE, maxArray, 1, MPI_DOUBLE, MPI_COMM_WORLD);

        for (int l = 0; l < size; ++l) {

            if (maxArray[l] > max) {
                max = maxArray[l];
            }
        }

    } while (max >= e);
}


int main(int argc, char **argv) {

    int size, rank;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double segmentsLength[DIMENSION];

    const double bordersX[2] = {-1.0, 1.0};
    const double bordersY[2] = {-1.0, 1.0};
    const double bordersZ[2] = {-1.0, 1.0};


    const double a = 1E5, e = 1E-8;

    double Dx = bordersX[1] - bordersX[0];
    double Dy = bordersY[1] - bordersY[0];
    double Dz = bordersZ[1] - bordersZ[0];

    segmentsLength[0] = Dx / (sizeX - 1);
    segmentsLength[1] = Dy / (sizeY - 1);
    segmentsLength[2] = Dz / (sizeZ - 1);

    int localSizeZ = sizeZ / size;

    double *funcResults = NULL;
    double *localFuncResults = (double *) malloc((sizeX * sizeY * sizeZ * sizeof(double)) / size + 2 *
                                                                                                   sizeX * sizeY *
                                                                                                   sizeof(double));


    const double localBordersZ[2] = {-1.0 + rank * segmentsLength[0] * localSizeZ,
                                     -1.0 + (rank + 1) * segmentsLength[0] * localSizeZ};

    start = MPI_Wtime();

    if (rank == 0) {
        funcResults = calloc(sizeX * sizeY * sizeZ, sizeof(double));
        fillBorders(funcResults, segmentsLength, bordersX, bordersY, bordersZ);
    }

    MPI_Barrier(MPI_COMM_WORLD);


    MPI_Scatter(funcResults, (sizeX * sizeY * sizeZ) / size, MPI_DOUBLE, localFuncResults + sizeX * sizeY,
                (sizeX * sizeY * sizeZ) / size, MPI_DOUBLE,
                0, MPI_COMM_WORLD);


    calcFunctionValues(localFuncResults, segmentsLength, bordersX, bordersY, localBordersZ, a, e, localSizeZ, rank,
                       size);


    MPI_Gather(localFuncResults + sizeX * sizeY, sizeX * sizeY * localSizeZ, MPI_DOUBLE, funcResults,
               sizeX * sizeZ * localSizeZ, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    end = MPI_Wtime();


    if (rank == 0)
        printf("Time: %f \n ", end - start);


//        if (rank == 0) {
//            showGrid(funcResults, segmentsLength, bordersX, bordersY, bordersZ);
//        }


    MPI_Finalize();
    return 0;
}