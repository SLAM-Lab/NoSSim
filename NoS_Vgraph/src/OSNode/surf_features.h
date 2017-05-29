
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/imgproc.hpp"
//#include "image_file.h"
#include "vision_graph.h"


#ifndef SURF_FEATURE__H
#define SURF_FEATURE__H
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
void readme();


// Clear matches for which NN ratio is > than threshold
// return the number of removed points
// (corresponding entries being cleared,
// i.e. size will be 0)
int ratioTest(std::vector<std::vector<cv::DMatch> > &matches);



// Insert symmetrical matches in symMatches vector
void symmetryTest( const std::vector<std::vector<cv::DMatch> >& matches1,
			const std::vector<std::vector<cv::DMatch> >& matches2,
			std::vector<cv::DMatch>& symMatches) ;


// Identify good matches using RANSAC
// Return fundemental matrix
cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
			const std::vector<cv::KeyPoint>& keypoints1,
			const std::vector<cv::KeyPoint>& keypoints2,
			std::vector<cv::DMatch>& outMatches);
/** @function main */
int visionGraphApp(char* obj, char* scene);

void preProcess(char* input, char* output);

int featureMatch(char* img1, char* img2);

  /** @function readme */
//void readme()
//  { std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl; }

#endif //SURF_FEATURE___H


