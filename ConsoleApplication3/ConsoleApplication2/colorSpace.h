#pragma once

#include "cv.h"

class colorSpace
{
public:
	colorSpace();
	~colorSpace();
public:
	cv::Mat srcMat;
public:
	void rgb2hsv(cv::Mat src, cv::Mat& hsvImage);
	void rgb2gray(cv::Mat src, cv::Mat& grayImage);
};

