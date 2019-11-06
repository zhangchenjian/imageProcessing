#include "colorSpace.h"
#include "highgui.h"

colorSpace::colorSpace()
{
}


colorSpace::~colorSpace()
{
}

void colorSpace::rgb2hsv(cv::Mat src, cv::Mat& hsvImage)
{
	cv::cvtColor(src, hsvImage, CV_BGR2HSV);

	

	/*for (int row = 0; row < src.rows; row++)
	{
	for (int col = 0; col < src.cols; col++)
	{
	hsvImage.at<cv::Vec3b>(row, col)[0] = 0;
	hsvImage.at<cv::Vec3b>(row, col)[1] = src.at<cv::Vec3s>(row, col)[1];
	hsvImage.at<cv::Vec3b>(row, col)[2] = 0;
	}
	}*/

}

void colorSpace::rgb2gray(cv::Mat src, cv::Mat& grayImage)
{
	cv::cvtColor(src, grayImage, CV_BGR2GRAY);

	/*grayImage.create(src.size(), CV_8UC1);

	for (int row = 0; row < src.rows; row++)
	{
		for (int col = 0; col < src.cols; col++)
		{
			grayImage.at<uchar>(row, col) = 0.114 * src.at<cv::Vec3b>(row, col)[2] + 0.587 * src.at<cv::Vec3b>(row, col)[1] + 0.299 * src.at<cv::Vec3b>(row, col)[0];
		}
	}*/
}
