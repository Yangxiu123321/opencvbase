#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @function main */
int main(int argc, char** argv)
{

	Mat src, src_gray;
	Mat grad;
	char window_name[] ="Sobel Demo - Simple Edge Detector";
	char windows_dx[] = "dx";
	char windows_dy[] = "dy";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	int c;

	/// װ��ͼ��
	src = imread("1.jpg");

	if (!src.data)
	{
		return -1;
	}

	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// ת��Ϊ�Ҷ�ͼ
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// ������ʾ����
	namedWindow(window_name, CV_WINDOW_NORMAL);

	/// ���� grad_x �� grad_y ����
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// �� X�����ݶ�
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	namedWindow(windows_dx, CV_WINDOW_NORMAL);
	imshow(windows_dx, abs_grad_x);

	/// ��Y�����ݶ�
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_y, abs_grad_y);
	printf("2:%d", abs_grad_y.at<uchar>(100, 100));
	printf("22:%d", abs_grad_y.at<uchar>(100, 101));

	namedWindow(windows_dy, CV_WINDOW_NORMAL);
	imshow(windows_dy, abs_grad_y);

	/// �ϲ��ݶ�(����)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	imshow(window_name, grad);

	waitKey(0);

	return 0;
}