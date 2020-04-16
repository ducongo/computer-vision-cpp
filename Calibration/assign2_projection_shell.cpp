#include "opencv2/opencv.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#define NUM_POINTS    10
#define RANGE         100.00

#define MAX_CAMERAS   100 
#define MAX_POINTS    3000


double projection[3][4] = {
0.902701, 0.051530, 0.427171, 10.0,
0.182987, 0.852568, -0.489535, 15.0,
-0.389418, 0.520070, 0.760184, 20.0,
};

double intrinsic[3][3] = {
-1000.000000, 0.000000, 0.000000, 
0.000000, -2000.000000, 0.000000, 
0.000000, 0.000000, 1.000000,
};

double all_object_points[10][3] = {
71.0501, 51.3535, 30.3995,
1.4985, 9.1403, 36.4452,
14.7313, 16.5899, 98.8525,
44.5692, 11.9083, 0.4669,
0.8911, 37.7880, 53.1663,
0.1251, 56.3585, 19.3304, 
80.8741, 58.5009, 47.9873,
35.0291, 89.5962, 82.2840,
74.6605, 17.4108, 85.8943,
57.1184, 60.1764, 60.7166,
};

// you write this routine
void decomposeprojectionmatrix(CvMat* projection_matrix, CvMat* rotation_matrix, CvMat* translation, CvMat* camera_matrix)
{



}


// you write this routine
void computeprojectionmatrix(CvMat* image_points, CvMat* object_points, CvMat* projection_matrix)
{

	cv::Mat dst2(RANGE, RANGE, CV_8UC3);

	std::vector<cv::Point2f> twoDPoints;
	for (int i=0; i<NUM_POINTS; i++) {
		twoDPoints.push_back(cv::Point2f(cvmGet(image_points, i, 0), cvmGet(image_points, i, 1)));
	}

	std::vector<cv::Point3f> threeDPoints;
	for (int i=0; i<NUM_POINTS; i++) {
		threeDPoints.push_back(cv::Point3f(all_object_points[i][0], all_object_points[i][1], all_object_points[i][2]));

	}

	std::vector<std::vector<cv::Point2f> > imagePoints;
  imagePoints.push_back(twoDPoints);
  
  std::vector<std::vector<cv::Point3f> > objectPoints;
  objectPoints.push_back(threeDPoints);

  cv::Mat cameraMatrix;
  cv::Mat distCoeffs;
  std::vector<cv::Mat> rvecs;
  std::vector<cv::Mat> tvecs;
  cv::calibrateCamera(objectPoints, imagePoints, dst2.size(), cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS);
  
  std::cout << "projection matrix: \n" << cameraMatrix << std::endl << std::endl;
  std::cout << "distortion matrix: \n" << distCoeffs << std::endl << std::endl;
  
  for(int i=0; i<rvecs.size(); ++i){
    std::cout << "rotation matrix: \n" << rvecs[i] << std::endl << std::endl;
  }

  for(int i=0; i<tvecs.size(); ++i){
    std::cout << "trnaslation matrix: \n" << tvecs[i] << std::endl << std::endl;
  }

	

}



int main() {
    CvMat* camera_matrix, *computed_camera_matrix; //
    CvMat* rotation_matrix, *computed_rotation_matrix; //
    CvMat* translation, *computed_translation;
	CvMat* image_points;
    CvMat* rot_vector;
    CvMat* object_points;
	CvMat* computed_projection_matrix;
	FILE *fp;

	object_points = cvCreateMat(NUM_POINTS, 3, CV_32F);
	image_points = cvCreateMat(NUM_POINTS, 2, CV_32F);

	rot_vector = cvCreateMat(3, 1, CV_32F);
    camera_matrix = cvCreateMat(3, 3, CV_32F);
    rotation_matrix = cvCreateMat(3, 3, CV_32F);
    translation = cvCreateMat(3, 1, CV_32F);

    computed_camera_matrix = cvCreateMat(3, 3, CV_32F);
    computed_rotation_matrix = cvCreateMat(3, 3, CV_32F);
    computed_translation = cvCreateMat(3, 1, CV_32F);
	computed_projection_matrix = cvCreateMat(3, 4, CV_32F);

	fp = fopen("assign2-Part2b","w");

	fprintf(fp, "Rotation matrix\n");
	for (int i=0; i<3; i++) {
   		for (int j=0; j<3; j++) {
    		cvmSet(camera_matrix,i,j, intrinsic[i][j]);
    		cvmSet(rotation_matrix,i,j, projection[i][j]);
    	}
		fprintf(fp, "%f %f %f\n", 
			cvmGet(rotation_matrix,i,0), cvmGet(rotation_matrix,i,1), cvmGet(rotation_matrix,i,2));
		
	}
    for (int i=0; i<3; i++)
    	cvmSet(translation, i, 0, projection[i][3]);

	fprintf(fp, "\nTranslation vector\n");
	fprintf(fp, "%f %f %f\n", 
		cvmGet(translation,0,0), cvmGet(translation,1,0), cvmGet(translation,2,0));

	fprintf(fp, "\nCamera Calibration\n");
	for (int i=0; i<3; i++) {
		fprintf(fp, "%f %f %f\n", 
			cvmGet(camera_matrix,i,0), cvmGet(camera_matrix,i,1), cvmGet(camera_matrix,i,2));
	}

	fprintf(fp,"\n");
	for (int i=0; i<NUM_POINTS; i++) {
		cvmSet(object_points, i, 0, all_object_points[i][0]);
		cvmSet(object_points, i, 1, all_object_points[i][1]);
		cvmSet(object_points, i, 2, all_object_points[i][2]);
		fprintf(fp, "Object point %d x %f y %f z %f\n", 
			i, all_object_points[i][0], all_object_points[i][1], all_object_points[i][2]);
	}
	fprintf(fp, "\n");

    cvRodrigues2(rotation_matrix, rot_vector);
    cvProjectPoints2(object_points, rot_vector, translation, camera_matrix, NULL, image_points);
    std::vector<cv::Point2f> points;
	for (int i=0; i<NUM_POINTS; i++) {
		fprintf(fp, "Image point %d x %f y %f\n", 
			i, cvmGet(image_points, i, 0), cvmGet(image_points, i, 1));
		std::vector<cv::Point2f> points;

	}

	computeprojectionmatrix(image_points, object_points, computed_projection_matrix);
	decomposeprojectionmatrix(computed_projection_matrix, computed_rotation_matrix, computed_translation, computed_camera_matrix);

/*

	fprintf(fp, "\nComputed Rotation matrix\n");
	for (int i=0; i<3; i++) {
		fprintf(fp, "%f %f %f\n", 
			cvmGet(computed_rotation_matrix,i,0), cvmGet(computed_rotation_matrix,i,1), cvmGet(computed_rotation_matrix,i,2));
	}

	fprintf(fp, "\nComputed Translation vector\n");
	fprintf(fp, "%f %f %f\n", 
		cvmGet(computed_translation,0,0), cvmGet(computed_translation,1,0), cvmGet(computed_translation,2,0));

	fprintf(fp, "\nComputed Camera Calibration\n");
	for (int i=0; i<3; i++) {
		fprintf(fp, "%f %f %f\n", 
			cvmGet(computed_camera_matrix,i,0), cvmGet(computed_camera_matrix,i,1), cvmGet(computed_camera_matrix,i,2));
	}
*/
	fclose(fp);

    return 0;
}
