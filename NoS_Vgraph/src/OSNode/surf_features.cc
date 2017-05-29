
#include "surf_features.h"



// Clear matches for which NN ratio is > than threshold
// return the number of removed points
// (corresponding entries being cleared,
// i.e. size will be 0)
int ratioTest(std::vector<std::vector<cv::DMatch> > &matches) {
   int removed=0;
   float ratio = 0.95;
// for all matches
   for (std::vector<std::vector<cv::DMatch> >::iterator matchIterator= matches.begin(); matchIterator!= matches.end(); ++matchIterator) {
// if 2 NN has been identified
      if (matchIterator->size() > 1) {
// check distance ratio
	if (((*matchIterator)[0].distance/(*matchIterator)[1].distance) > ratio) {
		//cout<<((*matchIterator)[0].distance/(*matchIterator)[1].distance)<<endl;
		matchIterator->clear(); // remove match
		//removed++;
	}
      } else { // does not have 2 neighbours
	matchIterator->clear(); // remove match
	//removed++;
      }
   }   
   //cout<<"Removed points are"<< removed <<endl;
   return removed;
}



// Insert symmetrical matches in symMatches vector
void symmetryTest( const std::vector<std::vector<cv::DMatch> >& matches1,
			const std::vector<std::vector<cv::DMatch> >& matches2,
			std::vector<cv::DMatch>& symMatches) {
// for all matches image 1 -> image 2
	for (std::vector<std::vector<cv::DMatch> >::const_iterator matchIterator1= matches1.begin();matchIterator1!= matches1.end(); ++matchIterator1) {
		// ignore deleted matches
		if (matchIterator1->size() < 2)
		continue;
		// for all matches image 2 -> image 1
		for (std::vector<std::vector<cv::DMatch> >::const_iterator matchIterator2= matches2.begin();matchIterator2!= matches2.end();++matchIterator2) {
		// ignore deleted matches
			if (matchIterator2->size() < 2)
			continue;
			// Match symmetry test
			if ((*matchIterator1)[0].queryIdx ==(*matchIterator2)[0].trainIdx &&(*matchIterator2)[0].queryIdx ==(*matchIterator1)[0].trainIdx) {
			// add symmetrical match
			symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,(*matchIterator1)[0].trainIdx,(*matchIterator1)[0].distance));
			break; // next match in image 1 -> image 2
			}
		}
	}


}


// Identify good matches using RANSAC
// Return fundemental matrix
cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
			const std::vector<cv::KeyPoint>& keypoints1,
			const std::vector<cv::KeyPoint>& keypoints2,
			std::vector<cv::DMatch>& outMatches) {
	bool refineF=true; // if true will refine the F matrix
	double distance=3.0; // min distance to epipolar
	double confidence=0.99; // confidence level (probability)


// Convert keypoints into Point2f
	std::vector<cv::Point2f> points1, points2;
	for (std::vector<cv::DMatch>::const_iterator it= matches.begin();it!= matches.end(); ++it) {
	// Get the position of left keypoints
		float x= keypoints1[it->queryIdx].pt.x;
		float y= keypoints1[it->queryIdx].pt.y;
		points1.push_back(cv::Point2f(x,y));
		// Get the position of right keypoints
		x= keypoints2[it->trainIdx].pt.x;
		y= keypoints2[it->trainIdx].pt.y;
		points2.push_back(cv::Point2f(x,y));
	}
// Compute F matrix using RANSAC
	std::vector<uchar> inliers(points1.size(),0);
	cv::Mat fundemental= cv::findFundamentalMat( cv::Mat(points1),cv::Mat(points2), // matching points
						inliers,// match status (inlier or outlier)
						CV_FM_RANSAC, // RANSAC method
						distance,
						// distance to epipolar line
						confidence); // confidence probability
// extract the surviving (inliers) matches
	std::vector<uchar>::const_iterator itIn= inliers.begin();
	std::vector<cv::DMatch>::const_iterator itM= matches.begin();
// for all matches
	for ( ;itIn!= inliers.end(); ++itIn, ++itM) {
		if (*itIn) { // it is a valid match
			outMatches.push_back(*itM);
		}
	}
	if (refineF) {
// The F matrix will be recomputed with
// all accepted matches
// Convert keypoints into Point2f
// for final F computation
		points1.clear();
		points2.clear();
		for (std::vector<cv::DMatch>::const_iterator it= outMatches.begin();it!= outMatches.end(); ++it) {
// Get the position of left keypoints\00h

			float x= keypoints1[it->queryIdx].pt.x;
			float y= keypoints1[it->queryIdx].pt.y;
			points1.push_back(cv::Point2f(x,y));
			// Get the position of right keypoints
			x= keypoints2[it->trainIdx].pt.x;
			y= keypoints2[it->trainIdx].pt.y;
			points2.push_back(cv::Point2f(x,y));
		}
	// Compute 8-point F from all accepted matches
		fundemental= cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2), // matches
					CV_FM_8POINT); // 8-point method
	}
	return fundemental;
}






void dump_array(Mat& matrix, char* fileName){
  //char fileName[30] = "testFile.dat";
  // Write data to file
  FILE* file = fopen (fileName, "wb");
/*
  for(int i = 0; i< matrix.rows; i++){
   for(int j = 0; j< matrix.cols; j++){
    //  float f = matrix(i,j);
      //fwrite(&f, sizeof(float), 1, file);
   }
  }
*/
  MatIterator_<float> it, end;
  for( it = matrix.begin<float>(), end = matrix.end<float>(); it != end; ++it){
              //  cout<<*it<<endl;
    float f = *it;
    fwrite(&f, sizeof(float), 1, file);

  }
  fclose(file);
}


Mat read_array(char* fileName){
  //char fileName[30] = "testFile.dat";
  FILE*  file = fopen(fileName, "rb");
  float* results = new float[500000];
  int i=0;
  while(1){
    float f;
    int n  = fread(&f, sizeof(float), 1, file);
    if (n<1){  	
	//	printf("%d\n", i);
		break;
    }
    results[i] = f;
    i++;
  }
  fclose(file);
  //printf("%d", i/64);
  /*
  for(int ii=0; ii<(i); ii++)
  	printf("%f\n", results[ii]);
  */
  float* array = new float[i];
  for(int ii=0; ii<(i); ii++)
  	array[ii] = results[ii];
  cv::Mat matrix = cv::Mat(i/64, 64, CV_32F, array);
  return matrix;
}

void preProcess(char* input, char* output){

  Mat img_object_raw = imread( input, IMREAD_GRAYSCALE );
  Mat img_object;
  resize(img_object_raw, img_object, Size(img_object_raw.rows/3, img_object_raw.cols/3), 0, 0, INTER_CUBIC);




  // Mat img_object= imread( input, IMREAD_GRAYSCALE );

  if( !img_object.data )
  { std::cout<< " --(!) Error reading images " << std::endl;  }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 10;
  Ptr<SURF> detector = SURF::create( minHessian );
  std::vector<KeyPoint> keypoints_object;
  detector->detect( img_object, keypoints_object );


  //-- Step 2: Calculate descriptors (feature vectors)
  Ptr<SURF> extractor = SURF::create( minHessian );
  Mat descriptors_object;
  extractor->compute( img_object, keypoints_object, descriptors_object );

  dump_array(descriptors_object, output);


}

int featureMatch(char* img1, char* img2){




  Mat descriptors_object, descriptors_scene;
  descriptors_object = read_array(img1);
  descriptors_scene = read_array(img2);


  //-- Step 3: Matching descriptor vectors using FLANN matcher
  BFMatcher matcher;
  std::vector< DMatch > matches;
  std::vector<std::vector<cv::DMatch> > matches1;
  matcher.knnMatch(descriptors_object, descriptors_scene, matches1, 2);

  std::vector<std::vector<cv::DMatch> > matches2;
  matcher.knnMatch( descriptors_scene, descriptors_object, matches2, 2);

  // cout<<matches1.size()<<endl;
  // cout<<matches2.size()<<endl;

  ratioTest(matches1);
  ratioTest(matches2);

  // cout<<matches1.size()<<endl;
  // cout<<matches2.size()<<endl;
// 4. Remove non-symmetrical matches
  std::vector<cv::DMatch> symMatches;
  symmetryTest(matches1,matches2,symMatches);

  std::cout<<"Good matches for images: " << img1 << ", " << img2 << " :" << symMatches.size() <<std::endl;
  return symMatches.size();
}



/** @function main */
int visionGraphApp(char* obj, char* scene)

{

    Mat img_object_raw = imread( obj, IMREAD_GRAYSCALE );
    Mat img_scene_raw = imread( scene, IMREAD_GRAYSCALE );
    Mat img_object;
    Mat img_scene;
    std::cout << img_object_raw.rows <<" "<<img_object_raw.cols<<std::endl;
    resize(img_object_raw, img_object, Size(img_object_raw.rows/3, img_object_raw.cols/3), 0, 0, INTER_CUBIC);
    resize(img_scene_raw, img_scene, Size(img_object_raw.rows/3, img_object_raw.cols/3), 0, 0, INTER_CUBIC);


  //Mat img_object = imread( obj, IMREAD_GRAYSCALE );
  //Mat img_scene = imread( scene, IMREAD_GRAYSCALE );


  if( !img_object.data || !img_scene.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 10;
  Ptr<SURF> detector = SURF::create( minHessian );
  std::vector<KeyPoint> keypoints_object, keypoints_scene;
  detector->detect( img_object, keypoints_object );
  detector->detect( img_scene, keypoints_scene );

  //-- Step 2: Calculate descriptors (feature vectors)
  Ptr<SURF> extractor = SURF::create( minHessian );
  Mat descriptors_object, descriptors_scene;
  extractor->compute( img_object, keypoints_object, descriptors_object );
  extractor->compute( img_scene, keypoints_scene, descriptors_scene );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  BFMatcher matcher;
  std::vector< DMatch > good_matches;
  std::vector<std::vector<cv::DMatch> > matches1;
  matcher.knnMatch(descriptors_object, descriptors_scene, matches1, 2);

  std::vector<std::vector<cv::DMatch> > matches2;
  matcher.knnMatch( descriptors_scene, descriptors_object, matches2, 2);

  // cout<<matches1.size()<<endl;
  // cout<<matches2.size()<<endl;

  ratioTest(matches1);
  ratioTest(matches2);

  // cout<<matches1.size()<<endl;
  // cout<<matches2.size()<<endl;
// 4. Remove non-symmetrical matches
  std::vector<cv::DMatch> symMatches;
  symmetryTest(matches1,matches2,symMatches);

// 5. Validate matches using RANSAC
/*
  cv::Mat H0 = ransacTest(symMatches, keypoints_object, keypoints_scene, good_matches);
  // cout << "Total number of good matches: " << good_matches.size() << endl;
  Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, // matches 
		img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
  std::cout<<"Good matches for images: " << obj << ", " << scene << " :" << good_matches.size() <<std::endl;
  return good_matches.size();
*/
  std::cout<<"Good matches for images: " << obj << ", " << scene << " :" << symMatches.size() <<std::endl;
  return symMatches.size();

}




