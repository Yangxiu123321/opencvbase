#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;

int main()
{
	cv::Mat srcImage = cv::imread("F:\\opencvstu\\opencvbase\\Project1\\Project1\\47.png");
	if (srcImage.empty())
		return -1;
	cv::imshow("srcImage ", srcImage);
	cv::Mat grayImg1;
	cv::cvtColor(srcImage,grayImg1,COLOR_RGB2GRAY);
	cv::imshow("grayImg1",grayImg1);
	std::cout << (int)grayImg1.at<uchar>(125, 100)<<endl;

	cv::Mat grayImg2;
	cv::cvtColor(srcImage,grayImg2,COLOR_BGR2GRAY);
	cv::imshow("grayImg2",grayImg2);
	std::cout << (int)grayImg2.at<uchar>(125, 100);
	cv::waitKey(0);
	return 0;
}