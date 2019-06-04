#include <iostream>
#include <vector>
#include <stdio.h>


const int offset = (3072*1024)/4;
const int blockSize  = (3072*1024)/4;

unsigned long long rdtsc() {
    volatile unsigned int edx, eax;
    asm("rdtsc":"=d"(edx),"=a"(eax));
    return ((((unsigned long long)edx)<<32) | ((unsigned long long) eax));
}

void setArr(std::vector<int> & arr, const int & N){
    int j;

    for (int i = 0;  i < N; i++ )
    {
        for (j = 0; j < blockSize / N; j++)
        {
            arr[i*offset + j] = (i+1)*offset + j;
        }
    }

    for (j = 0; j < blockSize / N; j++)
    {
        arr[(N-1)*offset + j] = j + 1;
    }
    arr[(N-1)*offset + j] = 0;
}

int main() {
    std::vector<int>  arr;
    const int K = 200;
    unsigned long long counter1, counter2, tacts , tactsMin = 10000;

    for (int N = 1; N <= 32 ; N++)
    {
        auto arr = std::vector<int>(offset*N);
        setArr(arr , N);

        for (int k = 0,  i = 0; i<K; i++){

            counter1 = rdtsc();

            for (int k = 0,  i = 0; i<blockSize; i++)
                k = arr[k];

            counter2 = rdtsc();

            tacts = (counter2-counter1)/ blockSize;

            if (tacts < tactsMin)
                tactsMin = tacts;
        }

        printf(" %lld \n",  tactsMin);
        tactsMin = 10000;

    }
    return 0;
}
