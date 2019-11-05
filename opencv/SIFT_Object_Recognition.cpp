/**
* @概述: 在场景中采用SIFT算子进行已知目标检测
* @函数: SiftFeatureDetector、SiftDescriptorExtractor、FlannBasedMatcher、findHomography、perspectiveTransform
* @步骤：
*		Step 1: 在图像中使用SIFT算法SiftFeatureDetector检测关键点
*		Step 2: 对检测到的每一个关键点使用SiftDescriptorExtractor计算其特征向量（也称描述子）
*		Step 3: 使用FlannBasedMatcher通过特征向量对关键点进行匹配，使用阈值剔除不好的匹配
*		Step 4: 利用findHomography基于匹配的关键点找出相应的透视变换
*		Step 5: 利用perspectiveTransform函数映射点群，在场景中获取目标的位置
*/

#include <ctime>
#include <iostream>
#include "opencv2/core/core.hpp"	
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"		//包含 SiftFeatureDetector 的头文件
#include "opencv2/features2d/features2d.hpp"	//包含 FlannBasedMatcher 的头文件
#include "opencv2/calib3d/calib3d.hpp"			//包含 findHomography 的头文件


int main()
{
	cv::Mat imgObject = cv::imread("D:\\Proj\\opencv\\pic\\book2.jpg", CV_LOAD_IMAGE_GRAYSCALE);	// 待检测物体
	cv::Mat imgScene = cv::imread("D:\\Proj\\opencv\\pic\\desk2.jpg", CV_LOAD_IMAGE_GRAYSCALE);		// 已知场景

	if (imgObject.empty() || imgScene.empty())
	{
		std::cout << " --(!) Error reading images " << std::endl;
		return -1;
	}

	double begin = clock();

	///-- Step 1: 在图像中使用SIFT算法SiftFeatureDetector检测关键点
	cv::SiftFeatureDetector detector;//( minHessian );
	std::vector<cv::KeyPoint> keypointsObject, keypointsScene;
	detector.detect(imgObject, keypointsObject);
	detector.detect(imgScene, keypointsScene);
	std::cout << "object--number of keypoints: " << keypointsObject.size() << std::endl;
	std::cout << "scene--number of keypoints: " << keypointsScene.size() << std::endl;

	///-- Step 2: 使用SiftDescriptorExtractor计算其特征向量
	cv::SiftDescriptorExtractor extractor;
	cv::Mat descriptorsObject, descriptorsScene;
	extractor.compute(imgObject, keypointsObject, descriptorsObject);
	extractor.compute(imgScene, keypointsScene, descriptorsScene);

	///-- Step 3: 通过特征向量对关键点进行匹配
	cv::FlannBasedMatcher matcher;
	std::vector< cv::DMatch > allMatches;
	matcher.match(descriptorsObject, descriptorsScene, allMatches);
	std::cout << "number of matches before filtering: " << allMatches.size() << std::endl;

	//-- 计算关键点间的最大最小距离
	double maxDist = 0;
	double minDist = 100;
	for (int i = 0; i < descriptorsObject.rows; i++)
	{
		double dist = allMatches[i].distance;
		if (dist < minDist)
			minDist = dist;
		if (dist > maxDist)
			maxDist = dist;
	}
	printf("	max dist : %f \n", maxDist);
	printf("	min dist : %f \n", minDist);

	//-- 过滤匹配点，保留好的匹配点（这里采用的标准：distance<3*minDist）
	std::vector< cv::DMatch > goodMatches;
	for (int i = 0; i < descriptorsObject.rows; i++)
	{
		if (allMatches[i].distance < 2 * minDist)
			goodMatches.push_back(allMatches[i]);
	}
	std::cout << "number of matches after filtering: " << goodMatches.size() << std::endl;

	//-- 显示匹配结果
	cv::Mat resultImg;
	drawMatches(imgObject, keypointsObject, imgScene, keypointsScene,
		goodMatches, resultImg, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
		cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS //不显示未匹配的点
		);
	//-- 输出匹配点的对应关系
	for (size_t i = 0; i < goodMatches.size(); i++)
		printf("	good match %d: keypointsObject [%d]  -- keypointsScene [%d]\n", i, goodMatches[i].queryIdx, goodMatches[i].trainIdx);

	///-- Step 4: 使用findHomography找出相应的透视变换
	std::vector<cv::Point2f> object;
	std::vector<cv::Point2f> scene;
	for (size_t i = 0; i < goodMatches.size(); i++)
	{
		//-- 从好的匹配中获取关键点: 匹配关系是关键点间具有的一 一对应关系，可以从匹配关系获得关键点的索引
		//-- e.g. 这里的goodMatches[i].queryIdx和goodMatches[i].trainIdx是匹配中一对关键点的索引
		object.push_back(keypointsObject[goodMatches[i].queryIdx].pt);
		scene.push_back(keypointsScene[goodMatches[i].trainIdx].pt);
	}
	cv::Mat H = findHomography(object, scene, CV_RANSAC);

	///-- Step 5: 使用perspectiveTransform映射点群，在场景中获取目标位置
	std::vector<cv::Point2f> objCorners(4);
	objCorners[0] = cvPoint(0, 0);
	objCorners[1] = cvPoint(imgObject.cols, 0);
	objCorners[2] = cvPoint(imgObject.cols, imgObject.rows);
	objCorners[3] = cvPoint(0, imgObject.rows);
	std::vector<cv::Point2f> sceneCorners(4);
	cv::perspectiveTransform(objCorners, sceneCorners, H);

	//-- 在被检测到的目标四个角之间划线
	line(resultImg, sceneCorners[0] + cv::Point2f(imgObject.cols, 0), sceneCorners[1] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[1] + cv::Point2f(imgObject.cols, 0), sceneCorners[2] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[2] + cv::Point2f(imgObject.cols, 0), sceneCorners[3] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[3] + cv::Point2f(imgObject.cols, 0), sceneCorners[0] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);

	//-- 显示检测结果
	cv::imshow("detection result", resultImg);

	double end = clock();
	std::cout << "\nSIFT--elapsed time: " << (end - begin) / CLOCKS_PER_SEC * 1000 << " ms\n";

	cv::waitKey(0);
	return 0;
}