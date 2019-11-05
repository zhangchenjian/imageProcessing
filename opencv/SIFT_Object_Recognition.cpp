/**
* @����: �ڳ����в���SIFT���ӽ�����֪Ŀ����
* @����: SiftFeatureDetector��SiftDescriptorExtractor��FlannBasedMatcher��findHomography��perspectiveTransform
* @���裺
*		Step 1: ��ͼ����ʹ��SIFT�㷨SiftFeatureDetector���ؼ���
*		Step 2: �Լ�⵽��ÿһ���ؼ���ʹ��SiftDescriptorExtractor����������������Ҳ�������ӣ�
*		Step 3: ʹ��FlannBasedMatcherͨ�����������Թؼ������ƥ�䣬ʹ����ֵ�޳����õ�ƥ��
*		Step 4: ����findHomography����ƥ��Ĺؼ����ҳ���Ӧ��͸�ӱ任
*		Step 5: ����perspectiveTransform����ӳ���Ⱥ���ڳ����л�ȡĿ���λ��
*/

#include <ctime>
#include <iostream>
#include "opencv2/core/core.hpp"	
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"		//���� SiftFeatureDetector ��ͷ�ļ�
#include "opencv2/features2d/features2d.hpp"	//���� FlannBasedMatcher ��ͷ�ļ�
#include "opencv2/calib3d/calib3d.hpp"			//���� findHomography ��ͷ�ļ�


int main()
{
	cv::Mat imgObject = cv::imread("D:\\Proj\\opencv\\pic\\book2.jpg", CV_LOAD_IMAGE_GRAYSCALE);	// ���������
	cv::Mat imgScene = cv::imread("D:\\Proj\\opencv\\pic\\desk2.jpg", CV_LOAD_IMAGE_GRAYSCALE);		// ��֪����

	if (imgObject.empty() || imgScene.empty())
	{
		std::cout << " --(!) Error reading images " << std::endl;
		return -1;
	}

	double begin = clock();

	///-- Step 1: ��ͼ����ʹ��SIFT�㷨SiftFeatureDetector���ؼ���
	cv::SiftFeatureDetector detector;//( minHessian );
	std::vector<cv::KeyPoint> keypointsObject, keypointsScene;
	detector.detect(imgObject, keypointsObject);
	detector.detect(imgScene, keypointsScene);
	std::cout << "object--number of keypoints: " << keypointsObject.size() << std::endl;
	std::cout << "scene--number of keypoints: " << keypointsScene.size() << std::endl;

	///-- Step 2: ʹ��SiftDescriptorExtractor��������������
	cv::SiftDescriptorExtractor extractor;
	cv::Mat descriptorsObject, descriptorsScene;
	extractor.compute(imgObject, keypointsObject, descriptorsObject);
	extractor.compute(imgScene, keypointsScene, descriptorsScene);

	///-- Step 3: ͨ�����������Թؼ������ƥ��
	cv::FlannBasedMatcher matcher;
	std::vector< cv::DMatch > allMatches;
	matcher.match(descriptorsObject, descriptorsScene, allMatches);
	std::cout << "number of matches before filtering: " << allMatches.size() << std::endl;

	//-- ����ؼ����������С����
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

	//-- ����ƥ��㣬�����õ�ƥ��㣨������õı�׼��distance<3*minDist��
	std::vector< cv::DMatch > goodMatches;
	for (int i = 0; i < descriptorsObject.rows; i++)
	{
		if (allMatches[i].distance < 2 * minDist)
			goodMatches.push_back(allMatches[i]);
	}
	std::cout << "number of matches after filtering: " << goodMatches.size() << std::endl;

	//-- ��ʾƥ����
	cv::Mat resultImg;
	drawMatches(imgObject, keypointsObject, imgScene, keypointsScene,
		goodMatches, resultImg, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
		cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS //����ʾδƥ��ĵ�
		);
	//-- ���ƥ���Ķ�Ӧ��ϵ
	for (size_t i = 0; i < goodMatches.size(); i++)
		printf("	good match %d: keypointsObject [%d]  -- keypointsScene [%d]\n", i, goodMatches[i].queryIdx, goodMatches[i].trainIdx);

	///-- Step 4: ʹ��findHomography�ҳ���Ӧ��͸�ӱ任
	std::vector<cv::Point2f> object;
	std::vector<cv::Point2f> scene;
	for (size_t i = 0; i < goodMatches.size(); i++)
	{
		//-- �Ӻõ�ƥ���л�ȡ�ؼ���: ƥ���ϵ�ǹؼ������е�һ һ��Ӧ��ϵ�����Դ�ƥ���ϵ��ùؼ��������
		//-- e.g. �����goodMatches[i].queryIdx��goodMatches[i].trainIdx��ƥ����һ�Թؼ��������
		object.push_back(keypointsObject[goodMatches[i].queryIdx].pt);
		scene.push_back(keypointsScene[goodMatches[i].trainIdx].pt);
	}
	cv::Mat H = findHomography(object, scene, CV_RANSAC);

	///-- Step 5: ʹ��perspectiveTransformӳ���Ⱥ���ڳ����л�ȡĿ��λ��
	std::vector<cv::Point2f> objCorners(4);
	objCorners[0] = cvPoint(0, 0);
	objCorners[1] = cvPoint(imgObject.cols, 0);
	objCorners[2] = cvPoint(imgObject.cols, imgObject.rows);
	objCorners[3] = cvPoint(0, imgObject.rows);
	std::vector<cv::Point2f> sceneCorners(4);
	cv::perspectiveTransform(objCorners, sceneCorners, H);

	//-- �ڱ���⵽��Ŀ���ĸ���֮�仮��
	line(resultImg, sceneCorners[0] + cv::Point2f(imgObject.cols, 0), sceneCorners[1] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[1] + cv::Point2f(imgObject.cols, 0), sceneCorners[2] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[2] + cv::Point2f(imgObject.cols, 0), sceneCorners[3] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);
	line(resultImg, sceneCorners[3] + cv::Point2f(imgObject.cols, 0), sceneCorners[0] + cv::Point2f(imgObject.cols, 0), cv::Scalar(0, 255, 0), 4);

	//-- ��ʾ�����
	cv::imshow("detection result", resultImg);

	double end = clock();
	std::cout << "\nSIFT--elapsed time: " << (end - begin) / CLOCKS_PER_SEC * 1000 << " ms\n";

	cv::waitKey(0);
	return 0;
}