#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;
//41.jpg

int main()
{
	Mat srcImg = imread("41.jpg",0);
	namedWindow("画线图",0);
	namedWindow("边缘检测",0);
	vector<Vec2f> lines;
	Mat dstImg;
	Canny(srcImg,dstImg,50,200,3);
	imshow("边缘检测",dstImg);

	HoughLines(dstImg, lines, 1, CV_PI / 180,100 );
	Mat cdst = srcImg.clone();

	for (size_t i = 0; i < lines.size(); i++)//将求得的线条画出来
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
	cout << A << endl;
	imshow("画线图", cdst);
	waitKey(0);
}