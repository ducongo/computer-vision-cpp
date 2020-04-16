#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <string>
#include <list>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    // Read 3D points
    double x, y, z;

    // world coordinate
    x = 150; y = 200; z = 400;
    std::vector<cv::Point3d> objectPoints;
    objectPoints.push_back(cv::Point3d(x, y, z));

    // Intrisic matrix
    cv::Mat intrisicMat(3, 3, cv::DataType<double>::type); 

        intrisicMat.at<double>(0, 0) = -500;
	    intrisicMat.at<double>(1, 0) = 0;
	    intrisicMat.at<double>(2, 0) = 0;

	    intrisicMat.at<double>(0, 1) = 0;
	    intrisicMat.at<double>(1, 1) = -500;
	    intrisicMat.at<double>(2, 1) = 0;

	    intrisicMat.at<double>(0, 2) = 320;
	    intrisicMat.at<double>(1, 2) = 240;
	    intrisicMat.at<double>(2, 2) = 1;

    // Rotation vector
    cv::Mat rVec(3, 1, cv::DataType<double>::type); 
    rVec.at<double>(0) = 0;
    rVec.at<double>(1) = 0;
    rVec.at<double>(2) = 0;

    // Translation vector
    cv::Mat tVec(3, 1, cv::DataType<double>::type); 
    tVec.at<double>(0) = -70;
    tVec.at<double>(1) = -95;
    tVec.at<double>(2) = -120;

    // Distortion vector
    cv::Mat distCoeffs(5, 1, cv::DataType<double>::type);   
    distCoeffs.at<double>(0) = 0;
    distCoeffs.at<double>(1) = 0;
    distCoeffs.at<double>(2) = 0;
    distCoeffs.at<double>(3) = 0;
    distCoeffs.at<double>(4) = 0;

    std::cout << "Intrisic matrix: \n" << intrisicMat << std::endl << std::endl;
    std::cout << "Rotation vector: \n" << rVec << std::endl << std::endl;
    std::cout << "Translation vector: \n" << tVec << std::endl << std::endl;
    std::cout << "Distortion coef: \n" << distCoeffs << std::endl << std::endl;

    std::vector<cv::Point2d> projectedPoints;

    cv::projectPoints(objectPoints, rVec, tVec, intrisicMat, distCoeffs, projectedPoints);
    
    for (int i = 0; i < projectedPoints.size(); ++i)
    {
        std::cout << " Projected to " << projectedPoints[i] << std::endl;
    }
    
    return 0;
}














