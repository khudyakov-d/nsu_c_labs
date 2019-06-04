#include <stdio.h>
#include <math.h>
#include <sys/times.h> // for times
#include <unistd.h> // for sysconf

#define N 500000000

double ex(int x)
{
    size_t i;
    double exVal = 1;
    double el= 1;

    for(i = 1; i < N; i++)
    {
        el = (el*x)/i;
        exVal += el;
    }

    return exVal;
}

int main() {
	int x = 10;
    double e = ex(x);
    return 0;
}
