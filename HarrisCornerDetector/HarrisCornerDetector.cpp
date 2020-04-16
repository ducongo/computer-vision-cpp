#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
#include <list>

using namespace cv;
using namespace std;
Mat src, src_gray;
Mat cornerDetector_dst, cornerDetector_copy, cornerDetector_copy2;

int max_qualityLevel = 30;

double myShiTomasi_minVal, myShiTomasi_maxVal;

RNG rng(12345);

const char* corner_detector_window1 = "Harris Corner Detector with nonMaximumSupression";
const char* corner_detector_window2 = "Harris Corner Detector";

void Harris_corner_detector_nonMaximumSupression( int, void* );

void Harris_corner_detector( int, void* );

int alpha_slider;

double alpha;

int main( int argc, char** argv )
{
    CommandLineParser parser( argc, argv, "{@input | building.jpg | input image}" );
    src = imread( samples::findFile( parser.get<String>( "@input" ) ) );
    if ( src.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        cout << "Usage: " << argv[0] << " <Input image>" << endl;
        return -1;
    }

    cvtColor( src, src_gray, COLOR_BGR2GRAY );

    int blockSize = 3, apertureSize = 3;
    alpha_slider = 0;
    cornerMinEigenVal(src_gray, cornerDetector_dst, blockSize, apertureSize );

    /* Create Window and Trackbar */

    //harris cornor detector with non maximum supression
    namedWindow(corner_detector_window1);
    createTrackbar( "Quality Level:", corner_detector_window1, &alpha_slider, max_qualityLevel, Harris_corner_detector_nonMaximumSupression);
    Harris_corner_detector_nonMaximumSupression(alpha_slider, 0);

    //harris cornor detector without non maximum supression
    namedWindow(corner_detector_window2);
    createTrackbar( "Quality Level:", corner_detector_window2, &alpha_slider, max_qualityLevel, Harris_corner_detector);
    Harris_corner_detector(alpha_slider, 0);
    waitKey();
    return 0;
}


void Harris_corner_detector(int, void*){

    cornerDetector_copy2 = src.clone();
    
    alpha = (double) alpha_slider/500;
    for( int i = 0; i < src_gray.rows; i++ )
    {
        for( int j = 0; j < src_gray.cols; j++ )
        {
            if(cornerDetector_dst.at<float>(i,j) > alpha )
            {
                circle( cornerDetector_copy2, Point(j,i), 4,  Scalar(rng.uniform(0,255), rng.uniform(0, 256), rng.uniform(0, 256)), FILLED);
                
            }
        }
    }

    imshow("Harris Corner Detector", cornerDetector_copy2);

}

void Harris_corner_detector_nonMaximumSupression(int, void*)
{
    cornerDetector_copy = src.clone();

    alpha = (double) alpha_slider/500;
    for( int i = 0; i < src_gray.rows; i++ )
    {
        for( int j = 0; j < src_gray.cols; j++ )
        {
            if( cornerDetector_dst.at<float>(i,j) > alpha )
            {
                
                float tmp1 = cornerDetector_dst.at<float>(i-1, j-1);
                float tmp2 = cornerDetector_dst.at<float>(i+1, j+1);
                float tmp3 = cornerDetector_dst.at<float>(i+1, j-1);
                float tmp4 = cornerDetector_dst.at<float>(i-1, j+1);
                if(tmp1 > cornerDetector_dst.at<float>(i,j) and tmp2 > cornerDetector_dst.at<float>(i,j)){
                    
                    circle( cornerDetector_copy, Point(j,i), 4,  Scalar(rng.uniform(0,255), rng.uniform(0, 256), rng.uniform(0, 256)), FILLED);
                    
                }
            }
        }
    }

    imshow(corner_detector_window1, cornerDetector_copy);
}
























