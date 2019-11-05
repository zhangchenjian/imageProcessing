#define _CRT_SECURE_NO_WARNINGS 
#include <iostream> 
#include <cv.h> 
#include <highgui.h> 
#include "opencv2/photo/photo.hpp"   

std::string fileName = "D:\\Proj\\opencv\\ConsoleApplication2\\fruits.jpg"; 
cv::Mat img, inpaintMask; 
cv::Point prevPt(-1, -1); 
cv::Mat inpainted;  
void help() 
{ 	
	std::cout << "tips:" << std::endl;
	std::cout << "\t esc -> exit" << std::endl;
	std::cout << "\t r -> reinpaint" << std::endl;
	std::cout << "\t i -> inpaint" << std::endl; 
}  
void onMouse(int event, int x, int y, int flags, void*) 
{ 	
	if (event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON)) 	
	{ 		
		prevPt = cv::Point(-1, -1); 	
	} 	
	else if (event == CV_EVENT_LBUTTONDOWN) 	
	{ 		
		prevPt = cv::Point(x, y); 	
	} 	
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)) 	
	{ 		
		cv::Point pt(x, y); 		
		if (prevPt.x < 0) 		
		{ 			
			prevPt = pt; 		
		}
		cv::line(inpaintMask, prevPt, pt, cv::Scalar::all(255), 8, 8, 0); 		
		cv::line(img, prevPt, pt, cv::Scalar::all(255), 8, 8, 0); 		
		prevPt = pt; 		
		cv::imshow("img", img); 		
		cv::imshow("mask", inpaintMask); 	
	} 
}  
int main() 
{ 	
	cv::Mat img0 = cv::imread(fileName, -1);  	
	
	if (img0.empty()) 	
	{ 		
		std::cout << "can't open the image" << fileName << std::endl; 		
		return 0; 	
	}  	
	
	img = img0.clone(); 	
	inpaintMask = cv::Mat::zeros(img.size(), CV_8U);  	
	
	cv::namedWindow("img", 1); 	
	cv::imshow("img", img); 	
	cv::imshow("mask", inpaintMask);  	
	
	cvSetMouseCallback("img", onMouse, 0);  	
	
	help();  	
	
	while (1) 	
	{ 		
		char c = cv::waitKey(); 		
		
		if (c == 27) 		
		{ 			
			break; 		
		} 		
		
		if (c == 'r') 		
		{ 			
			inpaintMask = cv::Scalar::all(0); 			
			img = img0.clone(); 			
			cv::imshow("img", img); 			
			cv::imshow("mask", inpaintMask); 		
		} 		
		if (c == 'i' || c == ' ') 		
		{			 			
			inpaint(img, inpaintMask, inpainted, 3, CV_INPAINT_NS); 			
			cv::imshow("inpainted", inpainted); 	
		} 	
	}  	
	
	return 0; 
}
