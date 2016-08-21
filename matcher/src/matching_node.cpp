#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include "joy_test/Target.h"



#include "objectDetector.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;


Mat src, obj, cur, fire;
RNG rng(0xFFFFFFFF);
objectDetector detector;
ros::ServiceClient rmb_client;
joy_test::Target srv;

static Scalar randomColor(int icolor)
{
    //int icolor = 50;//(unsigned)rng;
    return Scalar(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
}

void computeContourMaxMin(vector<Point> &contour, int& minX, int& minY, int& maxX, int& maxY)
{
//		cout << "Size:" << contour.size() << endl;
	for(int i=0; i< contour.size(); i++)
	{
		if(i==0)
		{
			minX = maxX = contour[i].x;
			minY = maxY = contour[i].y;
		}
		else
		{
			if(contour[i].x > maxX)
				maxX = contour[i].x;
			if(contour[i].y > maxY)
				maxY = contour[i].y;
			if(contour[i].x < minX)
				minX = contour[i].x;
			if(contour[i].y < minY)
				minY = contour[i].y;
		}
	}
}

void imageCb(const sensor_msgs::Image::ConstPtr& msg)
{
	// Point2f pt[4]; //1, pt2, pt3, pt4;
	vector<Point> pt;
	int minX, minY, maxX, maxY;
	cv_bridge::CvImagePtr cv_ptr;
	srv.request.mode = 0;
	srv.request.target = 0;
	// cout << "Before try\n";
	try
	{
	  cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	// cout << "Afetr try\n";
	src = cv_ptr->image;
	cur = src;
	cv::cvtColor(cur, cur, CV_BGR2GRAY);
	detector.setImageTemplate(obj);
	detector.setImageSearch(cur);
	detector.update();
	pt.clear();

	for (int i = 0; i < 4; ++i)
	{
		pt.push_back(detector.dst_corners[i]);
	}

	computeContourMaxMin(pt,minX,minY,maxX,maxY);

	if (isContourConvex(pt) && minX > 0 && minY > 0 && maxX < 640 && maxY < 480 && abs(maxX - minX) > 50 && abs(maxY-minY) > 50) {
		cout << "minX: " << minX << " |minY: " << minY << " |maxX: " << maxX << " |maxY: " << maxY << endl;
		line(src, pt[0], pt[1], randomColor(12350000), 2, 8, 0);
		line(src, pt[1], pt[2], randomColor(12350000), 2, 8, 0);
		line(src, pt[2], pt[3], randomColor(12350000), 2, 8, 0);
		line(src, pt[3], pt[0], randomColor(12350000), 2, 8, 0);
		srv.request.target = (int)(maxX + minX)/2;
		cout << "Desired X: " << srv.request.target << endl;
		srv.request.mode = 1;
	}



	detector.setImageTemplate(fire);
	detector.setImageSearch(cur);
	detector.update();
	pt.clear();

	for (int i = 0; i < 4; ++i)
	{
		pt.push_back(detector.dst_corners[i]);
	}

	computeContourMaxMin(pt,minX,minY,maxX,maxY);

	if (isContourConvex(pt) && minX > 0 && minY > 0 && maxX < 640 && maxY < 480 && abs(maxX - minX) > 50 && abs(maxY-minY) > 50) {
		cout << "minX: " << minX << " |minY: " << minY << " |maxX: " << maxX << " |maxY: " << maxY << endl;
		line(src, pt[0], pt[1], randomColor(50), 2, 8, 0);
		line(src, pt[1], pt[2], randomColor(50), 2, 8, 0);
		line(src, pt[2], pt[3], randomColor(50), 2, 8, 0);
		line(src, pt[3], pt[0], randomColor(50), 2, 8, 0);
		srv.request.target = (int)(maxX + minX)/2;
		cout << "Desired X: " << srv.request.target << endl;
		srv.request.mode = 2;
	}

	imshow("Source", src);
	waitKey(3);

	rmb_client.call(srv);	//exit(0);

}

int main(int argc, char *argv[])
{

	ros::init(argc, argv, "matching_node");
	ros::NodeHandle n;
	ros::Subscriber image_sub;
	rmb_client = n.serviceClient<joy_test::Target>("reach_target");
	obj = imread("/home/harsha/ros/src/matcher/src/template1.png", CV_LOAD_IMAGE_GRAYSCALE);
	fire = imread("/home/harsha/ros/src/matcher/src/template2.png", CV_LOAD_IMAGE_GRAYSCALE);

	image_sub = n.subscribe("/webcam/image_raw", 1, &imageCb);

	ros::spin();

	return 0;
}

objectDetector::objectDetector()
{
	bSetImageSearch = false;
	bSetImageTemplate = false;
	bShowImage = true;
	bTracking = false;
	bInitialized = false;

	winName = "correspondences";

	ransacReprojThreshold = 4;

	initModule_nonfree();

	detector = cv::FeatureDetector::create( "GridFAST" ); // [“Grid”, “Pyramid”, “Dynamic”] SIFT, SURF, FAST, GFTT, MSER and HARRIS
	descriptorExtractor = cv::DescriptorExtractor::create( "SIFT" ); // ["Opponent"] SIFT, SURF and BRIEF
	descriptorMatcher = cv::DescriptorMatcher::create( "FlannBased" ); // “FlannBased”, “BruteForceMatcher”, “BruteForce-L1” and “BruteForce-HammingLUT”
	matcherFilterType = CROSS_CHECK_FILTER;

}


void objectDetector::setImageTemplate(Mat img_t)
{
	if(bSetImageTemplate) {
		bSetImageTemplate = false;
		img_template_keypoints.clear();
		img_template_boundingbox.clear();
	}

	img_template = cv::Mat(img_t);

	detector->detect( img_template, img_template_keypoints );

	int n = img_template_keypoints.size();
	if(n > 1)
	{
		printf("[OK] Found %d keypoints.\n", n);
		bSetImageTemplate = true;

		CvPoint dst[] = { cvPoint(0, 0),
						cvPoint(0, img_t.rows),
						cvPoint(img_t.cols, img_t.rows),
						cvPoint(img_t.cols, 0) };

		for (int i = 0; i < 4; i++) {
			img_template_boundingbox.push_back(dst[i]);
		}
	}
	else {
		printf("[ERROR] No keypoints found!\n");
		img_template_keypoints.clear();
		img_template_boundingbox.clear();
		bSetImageTemplate = false;
	}

	descriptorExtractor->compute( img_template, img_template_keypoints, img_template_descriptors );


}

void objectDetector::setImageSearch(Mat img_s)
{
	if (bSetImageSearch) {
		bSetImageSearch = false;
		img_search_keypoints.clear();
	}

	img_search = cv::Mat(img_s);

	detector->detect( img_search, img_search_keypoints );

	int n = img_search_keypoints.size();
	if(n > 1)
	{
		printf("[OK] Found %d keypoints.\n", n);
		bSetImageSearch = true;
	}
	else {
		printf("[ERROR] No keypoints found!\n");
		img_search_keypoints.clear();
		bSetImageSearch = false;
	}

	// compute descriptors for all computed keypoints
	descriptorExtractor->compute( img_search, img_search_keypoints, img_search_descriptors );

}

void objectDetector::simpleMatching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
					const cv::Mat& descriptors1, const cv::Mat& descriptors2,
					vector<cv::DMatch>& matches12 )
{
	vector<cv::DMatch> matches;
	descriptorMatcher->match( descriptors1, descriptors2, matches12 );
}

void objectDetector::crossCheckMatching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
						const cv::Mat& descriptors1, const cv::Mat& descriptors2,
						vector<cv::DMatch>& filteredMatches12, int knn )
{
	filteredMatches12.clear();
	vector<vector<cv::DMatch> > matches12, matches21;
	descriptorMatcher->knnMatch( descriptors1, descriptors2, matches12, knn );
	descriptorMatcher->knnMatch( descriptors2, descriptors1, matches21, knn );
	for( size_t m = 0; m < matches12.size(); m++ )
	{
		bool findCrossCheck = false;
		for( size_t fk = 0; fk < matches12[m].size(); fk++ )
		{
			cv::DMatch forward = matches12[m][fk];

			for( size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++ )
			{
				cv::DMatch backward = matches21[forward.trainIdx][bk];
				if( backward.trainIdx == forward.queryIdx )
				{
					filteredMatches12.push_back(forward);
					findCrossCheck = true;
					break;
				}
			}
			if( findCrossCheck ) break;
		}
	}
}

void objectDetector::update()
{
	if (bSetImageSearch && bSetImageTemplate)
	{
		if (!bInitialized)
		{


			// do matching between the template and image search
			// without tracking previous features since none initialized
			filteredMatches.clear();
			switch( matcherFilterType )
			{
				case CROSS_CHECK_FILTER :
					crossCheckMatching( descriptorMatcher, img_template_descriptors, img_search_descriptors, filteredMatches, 1 );
					break;
				default :
					simpleMatching( descriptorMatcher, img_template_descriptors, img_search_descriptors, filteredMatches );
			}

			// reindex based on found matches
			vector<int> queryIdxs( filteredMatches.size() ), trainIdxs( filteredMatches.size() );
			for( size_t i = 0; i < filteredMatches.size(); i++ )
			{
				queryIdxs[i] = filteredMatches[i].queryIdx;
				trainIdxs[i] = filteredMatches[i].trainIdx;
			}

			// build homograhpy w/ ransac
			vector<cv::Point2f> points1; cv::KeyPoint::convert(img_template_keypoints, points1, queryIdxs);
			vector<cv::Point2f> points2; cv::KeyPoint::convert(img_search_keypoints, points2, trainIdxs);
            if (points1.size() < 4 || points2.size() < 4) {
                printf("Not enough keypoints.\n");
                return;
            }
			H12 = findHomography( cv::Mat(points1), cv::Mat(points2), CV_RANSAC, ransacReprojThreshold );

			// create a mask of the current inliers based on transform distance
			vector<char> matchesMask( filteredMatches.size(), 0 );

			printf("Matched %lu features.\n", filteredMatches.size());

			// convert previous image points to current image points via homography
			// although this is a transformation from image to world coordinates
			// it should estimate the current image points
			cv::Mat points1t; perspectiveTransform(cv::Mat(points1), points1t, H12);
			for( size_t i1 = 0; i1 < points1.size(); i1++ )
			{
				if( norm(points2[i1] - points1t.at<cv::Point2f>((int)i1,0)) < 4 ) // inlier
				{
					matchesMask[i1] = 1;
					img_search_points_inliers[1].push_back(points2[i1]);
				}
				else {
					img_search_points_outliers[1].push_back(points2[i1]);
				}
			}


			// update bounding box
			cv::Mat bb;
			perspectiveTransform(cv::Mat(img_template_boundingbox), bb, H12);
			for( int i = 0; i < 4; i++ )
			{
				dst_corners[i] = bb.at<cv::Point2f>(i,0);
				//img_template_boundingbox[i] = bb.at<cv::Point2f>(i,0);
			}


			// draw inliers
			drawMatches( img_search, img_template_keypoints,
						 img_template, img_search_keypoints,
						filteredMatches, drawImg,
						CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask
	#if DRAW_RICH_KEYPOINTS_MODE
						, DrawMatchesFlags::DRAW_RICH_KEYPOINTS
	#endif
						);

	#if DRAW_OUTLIERS_MODE
			// draw outliers
			for( size_t i1 = 0; i1 < matchesMask.size(); i1++ )
				matchesMask[i1] = !matchesMask[i1];
			drawMatches( img_template, img_template_keypoints,
						img_search, img_search_keypoints, filteredMatches, drawImg, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), matchesMask,
						DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	#endif

			imshow( winName, drawImg );


		}
		else {
			// track features from previous frame into the current frame and see which
			// features are inliers, and which are outliers.  among the features that
			// are outliers, see if any were marked as inliers in the previous frame and
			// remark then as outliers

			// mark decsriptors on new features marked as outliers once the number of
			// inliers drops to a certain threshold and perform matching on the template.

			// patch based seems powerful as well.  creating a manifold or detecting planes
			// in the set of inliers and tracking them as a whole may be more powerful.
			//<#statements#>
		}

		//std::swap(current_template_points, previous_template_points);
		std::swap(img_search_points_inliers[1], img_search_points_inliers[0]);
		std::swap(img_search_points_outliers[1], img_search_points_outliers[0]);
		swap(prev_img_search, img_search);
	} // end bSetImageSearch && bSetImageTemplate

}
