#pragma once

#include "cv.h"

class SpatialFilter
{
public:
	SpatialFilter();
	~SpatialFilter();
private:
	cv::Mat src;
	double k, b;

public:
	void loadImage(const std::string& filePath, int flags = 1);
	void showImage(const std::string& winName);
	void linearTransform(cv::Mat& dst);
	void setLinearCoef();
	void destroyWin();

};

