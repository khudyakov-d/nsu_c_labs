#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <tgmath.h>

#define EQUATION(x, a, b, c) ((x) * (x) * (x) + (a) * (x) * (x) + (b) * (x) + (c))

void swap(double *first, double *second) {
    double temp;
    temp = *first;
    *first = *second;
    *second = temp;
}

void findSegment(double a, double b, double c, double d, double e,
                 double startPoint, double *leftPoint, double *rightPoint) {

    int sign = (EQUATION(startPoint, a, b, c) > 0 ? 1 : -1);


    while (1) {
        //printf("%.15lf \n", EQUATION(startPoint, a, b, c));

        if (sign * EQUATION(startPoint, a, b, c) < -e) {
            *leftPoint = startPoint - d;
            *rightPoint = startPoint;
            break;
        } else {
            startPoint += d;
        }
    }

}

double findRootInSegment(double a, double b, double c, double e, double leftPoint, double rightPoint) {

    double average = 0;
    double ep = 1e-6;
    int sign1 = (EQUATION(leftPoint, a, b, c) > 0 ? 1 : -1);
    int sign2 = (EQUATION(rightPoint, a, b, c) > 0 ? 1 : -1);

    if (sign1 == 1 || sign2 == -1) {

        while (1) {
            average = (rightPoint + leftPoint) / 2;

            printf("%.15lf \n", EQUATION(average, a, b, c));

            if (fabs(EQUATION(average, a, b, c)) < ep) {
                return average;
            } else if (EQUATION(average, a, b, c) > ep) {
                leftPoint = average;
            } else if (EQUATION(average, a, b, c) < ep) {
                rightPoint = average;
            }
        }

    } else {
        while (1) {
            average = (rightPoint + leftPoint) / 2;

            printf("%.15lf \n", EQUATION(average, a, b, c));

            if (fabs(EQUATION(average, a, b, c)) < ep) {
                return average;
            } else if (EQUATION(average, a, b, c) > ep) {
                rightPoint = average;
            } else if (EQUATION(average, a, b, c) < ep) {
                leftPoint = average;
            }
        }

    }
}


double calcDiscriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

int main(int argc, char **argv) {

    if (argc != 6) {
        printf("Invalid input values\n"
               "Please enter the number in order a, b, c, d, e \n"
               "Where a, b, c are the coefficients of the first degree cubic equation\n"
               "d - iteration step\n"
               "e - approximation\n"
        );

        return 1;
    }

    double a = strtod(argv[1], NULL);
    double b = strtod(argv[2], NULL);
    double c = strtod(argv[3], NULL);
    double d = strtod(argv[4], NULL);
    double e = strtod(argv[5], NULL);

    double discriminant = calcDiscriminant(3, 2 * a, b);

    if (discriminant > e * e) {
        double xd1 = (-2 * a - sqrt(discriminant)) / (2 * 3);
        double xd2 = (-2 * a + sqrt(discriminant)) / (2 * 3);

        if (xd2  < xd1) {
            swap(&xd1, &xd2);
        }

        double leftPoint = 0;
        double rightPoint = 0;

        printf("%lf \n", EQUATION(xd1, a, b, c));
        printf("%lf \n", EQUATION(xd2, a, b, c));

        if (EQUATION(xd1, a, b, c) > e && EQUATION(xd2, a, b, c) > e) {
            printf("Equation has one material root \n");

            findSegment(a, b, c, -d, e, xd1, &leftPoint, &rightPoint);
            double x = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            printf("x* =  %.15lf", x);
        }


        if (EQUATION(xd1, a, b, c) > e && fabs(EQUATION(xd2, a, b, c)) < e) {
            printf("Equation has two different material roots  \n");

            findSegment(a, b, c, -d, e, xd1 - e, &leftPoint, &rightPoint);
            double x = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            printf("x1* = %.15lf x2* = %.15lf", x, xd2);
        }

        if (EQUATION(xd1, a, b, c) > e && EQUATION(xd2, a, b, c) < -e) {
            printf("Equation has three material roots \n");

            findSegment(a, b, c, -d, e, xd1 - e, &leftPoint, &rightPoint);
            double x1 = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            findSegment(a, b, c, d, e, xd1 + e, &leftPoint, &rightPoint);
            double x2 = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            findSegment(a, b, c, d, e, xd2 + e, &leftPoint, &rightPoint);
            double x3 = findRootInSegment(a, b, c, e, leftPoint, rightPoint);


            printf("x1* = %.15lf x2* = %.15lf x3* = %.15lf\"", x1, x2, x3);
        }

        if (fabs(EQUATION(xd1, a, b, c)) < e && EQUATION(xd2, a, b, c) < -e) {
            printf("Equation has two different material roots \n");

            findSegment(a, b, c, d, e, xd2 + e, &leftPoint, &rightPoint);
            double x = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            printf("x1* = %.15lf x2* = %.15lf", xd1, x);
        }


        if (EQUATION(xd1, a, b, c) < -e && EQUATION(xd2, a, b, c) < e) {
            printf("Equation has one material root\n");

            findSegment(a, b, c, d, e, xd2 + e, &leftPoint, &rightPoint);
            double x = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

            printf("x* =  %.15lf", x);
        }

        if (fabs(EQUATION(xd1, a, b, c)) < e && fabs(EQUATION(xd2, a, b, c)) < e) {
            printf("Equation has one material roots\n");

            printf("x* = %.15lf ", xd1);
        }


    } else if (fabs(discriminant) < e * e || discriminant < - e) {
        printf("Equation has one material root \n");
        double x;

        if (fabs(EQUATION(0, a, b, c))  < e
        ) {
            x = 0;
        } else {
            double leftPoint = 0;
            double rightPoint = 0;

            if (EQUATION(0, a, b, c) < e) {
                findSegment(a, b, c, d, e, 0, &leftPoint, &rightPoint);
            } else {
                findSegment(a, b, c, -d, e, 0, &leftPoint, &rightPoint);
            }
            x = findRootInSegment(a, b, c, e, leftPoint, rightPoint);

        }

        printf("x* =  %.15lf", x);
    }

    return 0;
}