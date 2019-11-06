#include "SpatialFilter.h"
#include "highgui.h"

SpatialFilter::SpatialFilter()
{
}


SpatialFilter::~SpatialFilter()
{
}

void SpatialFilter::loadImage(const std::string& filePath, int flags)
{
	src = cv::imread(filePath, flags);

	if (src.empty())
	{
		std::cout << "load image failed" << std::endl;
		std::abort();
	}
}

void SpatialFilter::showImage(const std::string& winName)
{
	cv::imshow(winName, src);
	cv::waitKey(0);
}

void SpatialFilter::linearTransform(cv::Mat& dst)
{
	dst.create(src.size(), src.type());
	for (int row = 0; row < src.rows;row++)
	{
		for (int col = 0; col < src.cols;col++)
		{
			dst.at<cv::Vec3b>(row, col)[0] = cv::saturate_cast<uchar> (k * src.at<cv::Vec3b>(row, col)[0] + b);
			dst.at<cv::Vec3b>(row, col)[1] = cv::saturate_cast<uchar> (k * src.at<cv::Vec3b>(row, col)[1] + b);
			dst.at<cv::Vec3b>(row, col)[2] = cv::saturate_cast<uchar> (k * src.at<cv::Vec3b>(row, col)[2] + b);
		}
	}
}

void SpatialFilter::setLinearCoef()
{
	std::cout << "please linear coefficient k and b:" << std::endl;
	std::cin >> k >> b;
}

void SpatialFilter::destroyWin()
{
	cv::destroyAllWindows();
}
