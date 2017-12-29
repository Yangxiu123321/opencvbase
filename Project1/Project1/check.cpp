#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
//画直线等等函数在imgproc中
#include <opencv2/imgproc.hpp>  

int main()
{
	//创建一个窗口并且以Tset为标示
	cv::namedWindow("Test");
	//创建空白的图像
	cv::Mat Image = cv::Mat::zeros(600, 600, CV_8UC3);

	//两个顶点的方式创建矩形框
	cv::Point p1(100, 200);
	cv::Point p2(300, 400);
	cv::rectangle(Image, p1, p2, cv::Scalar(0, 0, 225), 1, 8, 0);

	//rect的方式创建矩形框
	cv::rectangle(Image, cv::Rect(300, 300, 100, 200), cv::Scalar(255, 0, 0), 1, 8, 0);

	cv::imshow("Test", Image);
	cv::waitKey(0);
	return 0;
}
