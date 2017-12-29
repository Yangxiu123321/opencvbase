#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
//��ֱ�ߵȵȺ�����imgproc��
#include <opencv2/imgproc.hpp>  

int main()
{
	//����һ�����ڲ�����TsetΪ��ʾ
	cv::namedWindow("Test");
	//�����հ׵�ͼ��
	cv::Mat Image = cv::Mat::zeros(600, 600, CV_8UC3);

	//��������ķ�ʽ�������ο�
	cv::Point p1(100, 200);
	cv::Point p2(300, 400);
	cv::rectangle(Image, p1, p2, cv::Scalar(0, 0, 225), 1, 8, 0);

	//rect�ķ�ʽ�������ο�
	cv::rectangle(Image, cv::Rect(300, 300, 100, 200), cv::Scalar(255, 0, 0), 1, 8, 0);

	cv::imshow("Test", Image);
	cv::waitKey(0);
	return 0;
}
