#include <iostream>
#include <vector>
#include <stdio.h>


unsigned long long rdtsc() {
    volatile unsigned int edx, eax;
    asm("rdtsc":"=d"(edx),"=a"(eax));
    return ((((unsigned long long)edx)<<32) | ((unsigned long long) eax));
}


void setDirectArr(std::vector<int> & arr, const int & N){
    arr.clear();
    for(int i = 1; i < N - 1; i++){
        arr.push_back(i);
    }
    arr.push_back(0);
}


void setReverseArr(std::vector<int> & arr, const int & N){
    arr.clear();
    arr.push_back(N-1);
    for(int i = 0; i < N - 2; i++){
        arr.push_back(i);
    }
}

void setRandomArr(std::vector<int> & arr, const int & N) {
    arr.clear();
    int pos = 0;
    int cur = 0;

    for (int i = 0; i < N; i++) {
        arr.push_back(-1);
    }

    srand(rdtsc());
    for (int i = 0; i < N - 1; i++) {
        while (true) {
            pos = rand() % N;
            if (arr[pos] != -1 || pos == cur)
                continue;
            else
                break;
        }
        arr[cur] = pos;
        cur = pos;
    }
    arr[pos] = 0;
}



int main() {
    std::vector<int> arr;
    std::vector<int> randomArr;

    const int min = 256; // 1кб
    const int max = 8388608; //32 мб

    const unsigned K = 10;
    unsigned long long tacts1, tacts2, tacts3;
    unsigned long long samples;
    unsigned long long counter1, counter2;

    printf("    Разм | Пр | Об | Сл \n");
    for (unsigned N = min; N <= max; N*=2) {
        samples  = N*K;

        setDirectArr(arr, N);

        for (int k = 0,  i = 0; i<N; i++)
            k = arr[k];

        counter1 = rdtsc();
        for (int k = 0,  i = 0; i<N*K; i++)
            k = arr[k];
        counter2 = rdtsc();

        tacts1 = counter2-counter1;


        setReverseArr(arr, N);
        for (int k = 0,  i = 0; i<N; i++)
            k = arr[k];

        counter1 = rdtsc();
        for (int k = 0,  i = 0; i<N*K; i++)
            k = arr[k];
        counter2 = rdtsc();

        tacts2 = counter2-counter1;

        setRandomArr(randomArr, N);
        for (int k = 1,  i = 0; i<N; i++){
            k = randomArr[k];
        }

        counter1 = rdtsc();

        for (int k = 0,  i = 0; i<N*K; i++)
            k = randomArr[k];

        counter2 = rdtsc();

        tacts3 = counter2-counter1;

        printf("%5.d КБ |", (N*4 )/ 1024);
        printf(" %lld | %lld | %lld \n",tacts1/samples,tacts2/samples, tacts3/samples);
     }

    return 0;
}
