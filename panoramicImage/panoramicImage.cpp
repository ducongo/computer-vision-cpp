#include <iostream>  
#include <stdio.h>  
#include "opencv2/core.hpp"  
#include "opencv2/core/utility.hpp"  
#include "opencv2/core/ocl.hpp"  
#include "opencv2/imgcodecs.hpp"  
#include "opencv2/highgui.hpp"  
#include "opencv2/features2d.hpp"  
#include "opencv2/calib3d.hpp"  
#include "opencv2/imgproc.hpp"  
#include"opencv2/flann.hpp"  
#include"opencv2/xfeatures2d.hpp"  
#include"opencv2/ml.hpp"  
 
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;

 
typedef struct
{
    Point2f left_top;
    Point2f left_bottom;
    Point2f right_top;
    Point2f right_bottom;
}four_corners_t;

four_corners_t corners;

four_corners_t CalcCorners(const Mat& H, const Mat& src)
{
    double v2[] = { 0, 0, 1 }; 
    double v1[3];
    Mat V2 = Mat(3, 1, CV_64FC1, v2);  
    Mat V1 = Mat(3, 1, CV_64FC1, v1);  
    V1 = H * V2;
    //top-left(0,0,1)
    corners.left_top.x = v1[0] / v1[2];
    corners.left_top.y = v1[1] / v1[2];
 
    //botton-left(0,src.rows,1)
    v2[0] = 0;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  
    V1 = Mat(3, 1, CV_64FC1, v1);  
    V1 = H * V2;
    corners.left_bottom.x = v1[0] / v1[2];
    corners.left_bottom.y = v1[1] / v1[2];
 
    //top-right (src.cols,0,1)
    v2[0] = src.cols;
    v2[1] = 0;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  
    V1 = Mat(3, 1, CV_64FC1, v1);  
    V1 = H * V2;
    corners.right_top.x = v1[0] / v1[2];
    corners.right_top.y = v1[1] / v1[2];
 
    //botton-right (src.cols,src.rows,1)
    v2[0] = src.cols;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);  //cal vector 
    V1 = Mat(3, 1, CV_64FC1, v1);  
    V1 = H * V2;
    corners.right_bottom.x = v1[0] / v1[2];
    corners.right_bottom.y = v1[1] / v1[2];

    return corners;
}


int main()
{
    Mat a = imread("keble_b_long.bmp", 1); 
    Mat b = imread("keble_a_half.bmp", 1);
    Mat e = imread("keble_c_half.bmp", 1);

    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors                        
    Ptr<SURF> surf = SURF::create(800);
 
    BFMatcher matcher;         
    Mat c, d, f;
    vector<KeyPoint>key1, key2, key3;
    vector<DMatch> matches, matches2;    
 
    //-- Step 2: Matching descriptor vectors with a brute force matcher
    surf->detectAndCompute(a, Mat(), key1, c);
    surf->detectAndCompute(b, Mat(), key2, d);
    surf->detectAndCompute(e, Mat(), key3, f);
 
    matcher.match(d, c, matches);   
    matcher.match(c, f, matches2); 

    //sort the match point from large to small 
    sort(matches.begin(), matches.end());     
    vector< DMatch > good_matches;
    int ptsPairs = std::min(50, (int)(matches.size() * 0.15));

    for (int i = 0; i < ptsPairs; i++)
    {
        good_matches.push_back(matches[i]);//the fifty minimum distance 
    }
    Mat outimg, outimg2; 

    //-- Draw matches                  
    drawMatches(b, key2, a, key1, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  
 
    sort(matches2.begin(), matches2.end());
    vector< DMatch > good_matches2;
    int ptsPairs2 = std::min(50, (int)(matches2.size() * 0.15));
    for (int i = 0; i < ptsPairs2; i++)
    {
        good_matches2.push_back(matches2[i]);
    }

    //-- Draw matches
    drawMatches(a, key1, e, key3, good_matches2, outimg2, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); 
    
    //-- Show detected matches
    imshow("match_output1", outimg);
    imshow("match_output2", outimg2);

    // image alignment
    vector<Point2f> imagePoints1, imagePoints2, imagePoints3, imagePoints4;
 
    for (int i = 0; i<good_matches.size(); i++)
    {
        imagePoints2.push_back(key2[good_matches[i].queryIdx].pt);
        imagePoints1.push_back(key1[good_matches[i].trainIdx].pt);
    }

    for (int i = 0; i<good_matches2.size(); i++)
    {
        imagePoints3.push_back(key1[good_matches2[i].queryIdx].pt);
        imagePoints4.push_back(key3[good_matches2[i].trainIdx].pt);
    }
    
    // optain the rotation matrix 3 * 3 
    Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
    Mat homo2 = findHomography(imagePoints2, imagePoints1, CV_RANSAC);
    Mat homo3 = findHomography(imagePoints4, imagePoints3, CV_RANSAC);
                                        
    // calculate the coordinate of four vertices 
    four_corners_t corners = CalcCorners(homo, a);
    four_corners_t corners2 = CalcCorners(homo2, b);  
    four_corners_t corners3 = CalcCorners(homo3, e); 

    //image registration                              
    Mat imageTransform1, imageTransform2, imageTransform3, imageTransform4;
    warpPerspective(a, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), b.rows));
    warpPerspective(b, imageTransform3, homo2, Size(MAX(corners2.right_top.x, corners2.right_bottom.x), a.rows));
    warpPerspective(e, imageTransform4, homo3, Size(MAX(corners3.right_top.x, corners3.right_bottom.x), e.rows));

    int dst_width2 = a.cols;  
    int dst_height2 = a.rows;

    Mat dst2(dst_height2, dst_width2, CV_8UC3);
    dst2.setTo(0);
    a.copyTo(dst2(Rect(0, 0, a.cols, a.rows)));
    imageTransform4.copyTo(dst2(Rect(0, 0, imageTransform4.cols, imageTransform4.rows)));
    imageTransform3.copyTo(dst2(Rect(0, 0, imageTransform3.cols, imageTransform3.rows)));
    imshow("result", dst2);
    imwrite("./result.jpg", dst2);
 
    waitKey();
 
    return 0;
}


