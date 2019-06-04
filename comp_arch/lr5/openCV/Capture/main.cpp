#include "opencv2/opencv.hpp"
#include <sys/times.h>
using namespace cv;

Mat frame, erosion;
int erosion_elem = 0;
int const max_elem = 2;
int erosion_size = 0;
const int max_kernel_size = 20;
struct timespec start, end;
double inputTime = 0, transformTime = 0, showTime = 0, totalTime;

void Erosion( int, void* )
{
    int erosion_type = 0;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

    //Замер времени, затраченного на преобразование
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    Mat element = getStructuringElement( erosion_type,
                                          Size( erosion_size + 1, erosion_size + 1 ),
                                          Point( -1,-1 ) );
    erode( frame, erosion, element );

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    transformTime += (end.tv_sec-start.tv_sec) + 0.000000001*(end.tv_nsec-start.tv_nsec);

    //Замер времени, затраченного на показ
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    imshow( "Erosion", erosion);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    showTime += (end.tv_sec-start.tv_sec) + 0.000000001*(end.tv_nsec-start.tv_nsec);
}

int main(int argc, char **argv)
{


    VideoCapture cap;

    if (argc>1) cap.open(argv[1]);
        else cap.open(0);

    namedWindow( "Erosion", WINDOW_AUTOSIZE );
    createTrackbar( "Element:\n 0: Rect 1: Cross 2: Ellipse", "Erosion",
                    &erosion_elem, max_elem,
                    Erosion );
    createTrackbar( "Kernel size:\n n +1", "Erosion",
                    &erosion_size, max_kernel_size,
                    Erosion );

    while(cap.isOpened() )
    {
        //Замер времени, затраченного на считывание данных с камеры
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        if(!cap.read(frame)) break;

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        inputTime += (end.tv_sec-start.tv_sec) + 0.000000001*(end.tv_nsec-start.tv_nsec);

        if (waitKey(17)==27) break;
        Erosion(0,0);
    }

    totalTime = inputTime + transformTime + showTime;

    printf("Input time: %lf sec.\nTransform time: %lf sec.\nShow time: %lf sec.\n", inputTime,
                                                                                     transformTime,
                                                                                     showTime);
    printf("Total time: %lf sec. \n", totalTime);
    printf("Input division: %lf %% \n", (inputTime/totalTime)*100 );
    printf("Transform division: %lf %% \n", (transformTime/totalTime)*100 );
    printf("Show division: %lf %%", (showTime/totalTime)*100 );

}
