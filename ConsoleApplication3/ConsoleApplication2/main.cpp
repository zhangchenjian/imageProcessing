#include "highgui.h"
#include "colorSpace.h"
#include "SpatialFilter.h"


int main()
{
	//-- color space exchange
	/*colorSpace spaceChange;

	spaceChange.srcMat = cv::imread("D:\\Proj\\opencv\\ConsoleApplication3\\kenan.jpg");
	cv::imshow("rgb", spaceChange.srcMat);

	cv::Mat dstHSV;
	spaceChange.rgb2hsv(spaceChange.srcMat, dstHSV);
	cv::imshow("hsv", dstHSV);

	cv::Mat dstGray;
	spaceChange.rgb2gray(spaceChange.srcMat, dstGray);
	cv::imshow("gray", dstGray);

	cv::waitKey(0);
	cv::destroyAllWindows();*/


	//-- spatial filter
	SpatialFilter spatialFilter;
	spatialFilter.loadImage("D:\\Proj\\opencv\\ConsoleApplication3\\kenan.jpg");
	spatialFilter.showImage("src");

	cv::Mat linearTrans;
	spatialFilter.setLinearCoef();
	spatialFilter.linearTransform(linearTrans);
	cv::imshow("linearTrans", linearTrans);
	cv::waitKey(0);

	spatialFilter.destroyWin();

	return 0;
}