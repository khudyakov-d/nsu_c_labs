#include <stdio.h>
#include <math.h>
#include <sys/times.h> // for times
#include <unistd.h> // for sysconf
//exp(x) = 1 + x + (x^2)/2! + (x^3)/3!....+ n*(x^n)/n!+.....

#define N 2800000000
//#define N 4200000000
//#define N 6400000000

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

    struct tms start, end;
    long clocks_per_sec = sysconf(_SC_CLK_TCK);
    long clocks;
    int x = 10;

  /*  printf("Enter a power for the exponent: ");
    if(scanf("%d",&x) == 0){
        printf("Error. Incorrect enter\n");
        return 1;
    };
  */

    times(&start);
    double e = ex(x);
    times(&end);

    clocks = end.tms_utime - start.tms_utime;

    printf("e^%d = %f\nTime taken: %lf sec.\n",x,e, (double)clocks / clocks_per_sec);

    return 0;
}
