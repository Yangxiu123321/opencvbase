#include <opencv2\opencv.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <string>
#include <windows.h>
//#include "Plate.h"
#include <stdlib.h>
#include <ctime>
#define window_name "���⻯��ֱ�Ӷ�ֵ�����Ǻ�"
using namespace cv;
using namespace std;


typedef enum { back, object } entropy_state;
float total;
//����hist;  
Mat drawHist(Mat hist, int bins, int height, Scalar rgb)
{
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);
	int scale = 1;
	Mat histImg = Mat::zeros(height, bins, CV_8UC3);
	float *binVal = hist.ptr<float>(0);
	for (int i = 0; i<bins; i++)
	{
		int intensity = cvRound(binVal[i] * height / maxVal);
		rectangle(histImg, Point(i*scale, 0),
			Point((i + 1)*scale, (intensity)), rgb, CV_FILLED);
	}
	flip(histImg, histImg, 0);
	return histImg;
}
//����ֱ��ͼ;  
Mat Hist(const Mat& src)
{
	Mat hist;
	int bins = 256;
	int histSize[] = { bins };
	float range[] = { 0,256 };
	const float* ranges[] = { range };
	int channels[] = { 0 };
	calcHist(&src, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	Mat histImg = drawHist(hist, bins, 200, Scalar(255, 0, 0));
	imshow("histRGB", histImg);
	return hist;
}
//���㵱ǰ��;  
float calEntropy(const Mat& hist, int threshold)
{
	float total_back = 0, total_object = 0;
	float entropy_back = 0, entropy_object = 0;
	float entropy = 0;
	int i = 0;
	const float* hist_p = (float*)hist.ptr<float>(0);
	total = 0;
	for (i = 0; i<hist.cols; i++)  //total���ܵĵ���
	{
		total += hist_p[i];
	}

	for (i = 0; i<threshold; i++) //total_back��ֱ��ͼ��i��ǰ�ĵ���
	{
		total_back += hist_p[i];
	}
	//cout << total_back << endl;
	total_object = total - total_back;//total_object�ǵ�i�к�ĵ���

	//������;  
	for (i = 0; i<threshold; i++)
	{
		//      if(hist_p[i]==0)  
		//          continue;  
		float percentage = hist_p[i] / total_back;
		if (percentage >0)
		{
			entropy_back += -percentage * logf(percentage); // �����Ķ��幫ʽ 
		}
	}
	//ǰ����;  
	for (i = threshold; i<hist.cols; i++)
	{
		//      if(hist_p[i]==0)  
		//      {  
		//          continue;  
		//      }  
		float percentage = hist_p[i] / total_object;
		if (percentage >0)
		{
			entropy_object += -percentage * logf(percentage); // �����Ķ��幫ʽ�� 
		}
	}

	entropy = entropy_object + entropy_back;
	//entropy =entropy_object;  
	return entropy;
}

float LeftBackEntropy(const Mat& hist, int threshold)  //��������ǲ���������ֵ�������ӣ����Ҳ���Ǳ����صı仯
{
	float total_back = 0, total_object = 0;
	float entropy_back = 0, entropy_object = 0;
	float entropy = 0;
	int i = 0;
	const float* hist_p = (float*)hist.ptr<float>(0);
	total = 0;
	for (i = 0; i<hist.cols; i++)  //total���ܵĵ���
	{
		total += hist_p[i];
	}
	for (i = 0; i<threshold; i++)
	{
		total_back += hist_p[i];
	}
	total_object = total - total_back;

	//������;  
	for (i = 0; i<threshold; i++)
	{
		//      if(hist_p[i]==0)  
		//          continue;  
		float percentage = hist_p[i] / total_back;
		if (percentage >0)
		{
			entropy_back += -percentage * logf(percentage); // �����Ķ��幫ʽ 
		}
	}

	entropy = entropy_back;
	//entropy =entropy_object;  
	return entropy;
}



void MaxEntropy(Mat img, Mat hist)
{
	total = sum(hist)[0];
	float MaxEntropyValue = 0.0f, MaxEntropyThreshold = 0.0f;
	float tmp;

	cout << hist.size() << endl;

	int num = 0;
	for (int i = 0; i<hist.cols; i++)
	{
		tmp = calEntropy(hist, i);

		if (tmp>MaxEntropyValue)
		{
			MaxEntropyValue = tmp;
			MaxEntropyThreshold = i;
		}
	}
	threshold(img, img, MaxEntropyThreshold, 255, CV_THRESH_BINARY);
	namedWindow("thresholdImg", WINDOW_NORMAL);
	imshow("thresholdImg", img);
	//imwrite("D:/thresholdImg.png",img);  
	cout << MaxEntropyThreshold << endl;
	cout << MaxEntropyValue << endl;
}

void LeftEntropy(Mat img, Mat hist)   //��������ǲ���������ֵ�������ӣ����Ҳ���Ǳ����صı仯
{

	total = sum(hist)[0];
	float MaxEntropyValue = 0.0, MaxEntropyThreshold = 0.0;
	float tmp;
	Mat SingleHist(hist.size(), CV_32FC1);//�������ͼ�����ֵ


	for (int i = 0; i<hist.cols; i++)
	{
		tmp = LeftBackEntropy(hist, i);
		SingleHist.at<float>(0, i) = tmp;//���ǲ������ͼ�����ֵ

	}

	Mat histImg = drawHist(SingleHist, 256, 200, Scalar(255, 0, 0));
	imshow("SingleHist", histImg);

}

int main(int argc, char *argv[])
{
	Mat src = imread("15.jpg", 0);
	namedWindow("SRC", WINDOW_NORMAL);
	imshow("SRC", src);
	Mat hist = Hist(src).t();
	MaxEntropy(src, hist);;
	LeftEntropy(src, hist);
	waitKey();
	return 1;
}