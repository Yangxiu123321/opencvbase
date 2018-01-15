#include <opencv2\opencv.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <string>
#include <windows.h>
//#include "Plate.h"
#include <stdlib.h>
#include <ctime>

using namespace cv;
using namespace std;

typedef enum { back, object } entropy_state;
float total;
//绘制hist;  
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
//计算直方图;  
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
//计算当前熵;  
float calEntropy(const Mat& hist, int threshold)
{
	float total_back = 0, total_object = 0;
	float entropy_back = 0, entropy_object = 0;
	float entropy = 0;
	int i = 0;
	const float* hist_p = (float*)hist.ptr<float>(0);
	total = 0;
	for (i = 0; i<hist.cols; i++)  //total是总的点数
	{
		total += hist_p[i];
	}

	for (i = 0; i<threshold; i++) //total_back是直方图第i列前的点数
	{
		total_back += hist_p[i];
	}
	//cout << total_back << endl;
	total_object = total - total_back;//total_object是第i列后的点数

	//背景熵;  
	for (i = 0; i<threshold; i++)
	{
		//      if(hist_p[i]==0)  
		//          continue;  
		float percentage = hist_p[i] / total_back;
		if (percentage >0)
		{
			entropy_back += -percentage * logf(percentage); // 能量的定义公式 
		}
	}
	//前景熵;  
	for (i = threshold; i<hist.cols; i++)
	{
		//      if(hist_p[i]==0)  
		//      {  
		//          continue;  
		//      }  
		float percentage = hist_p[i] / total_object;
		if (percentage >0)
		{
			entropy_object += -percentage * logf(percentage); // 能量的定义公式； 
		}
	}

	entropy = entropy_object + entropy_back;
	//entropy =entropy_object;  
	return entropy;
}

float LeftBackEntropy(const Mat& hist, int threshold)  //这个函数是测试随着阈值不断增加，左侧也就是背景熵的变化
{
	float total_back = 0, total_object = 0;
	float entropy_back = 0, entropy_object = 0;
	float entropy = 0;
	int i = 0;
	const float* hist_p = (float*)hist.ptr<float>(0);
	total = 0;
	for (i = 0; i<hist.cols; i++)  //total是总的点数
	{
		total += hist_p[i];
	}
	for (i = 0; i<threshold; i++)
	{
		total_back += hist_p[i];
	}
	total_object = total - total_back;

	//背景熵;  
	for (i = 0; i<threshold; i++)
	{
		//      if(hist_p[i]==0)  
		//          continue;  
		float percentage = hist_p[i] / total_back;
		if (percentage >0)
		{
			entropy_back += -percentage * logf(percentage); // 能量的定义公式 
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

void LeftEntropy(Mat img, Mat hist)   //这个函数是测试随着阈值不断增加，左侧也就是背景熵的变化
{

	total = sum(hist)[0];
	float MaxEntropyValue = 0.0, MaxEntropyThreshold = 0.0;
	float tmp;
	Mat SingleHist(hist.size(), CV_32FC1);//测试左边图像的熵值


	for (int i = 0; i<hist.cols; i++)
	{
		tmp = LeftBackEntropy(hist, i);
		SingleHist.at<float>(0, i) = tmp;//这是测试左边图像的熵值

	}

	Mat histImg = drawHist(SingleHist, 256, 200, Scalar(255, 0, 0));
	imshow("SingleHist", histImg);

}

bool IsDimodal(double* HistGram)       // 检测直方图是否为双峰的
{
	// 对直方图的峰进行计数，只有峰数位2才为双峰 
	int Count = 0;
	for (int Y = 1; Y < 255; Y++)
	{
		if (HistGram[Y - 1] < HistGram[Y] && HistGram[Y + 1] < HistGram[Y])
		{
			Count++;
			if (Count > 2) return false;
		}
	}
	if (Count == 2)
		return true;
	else
		return false;
}


int GetMinimumThreshold(int* HistGram)
{
	int Y, Iter = 0;
	double *HistGramC = new double[256];           // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
	double *HistGramCC = new double[256];          // 求均值的过程会破坏前面的数据，因此需要两份数据
	for (Y = 0; Y < 256; Y++)
	{
		HistGramC[Y] = HistGram[Y];
		HistGramCC[Y] = HistGram[Y];
	}

	// 通过三点求均值来平滑直方图
	while (IsDimodal(HistGramCC) == false)                                        // 判断是否已经是双峰的图像了      
	{
		HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                 // 第一点
		for (Y = 1; Y < 255; Y++)
			HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;     // 中间的点
		HistGramCC[255] = (HistGramC[254] + HistGramC[255] + HistGramC[255]) / 3;         // 最后一点
		//System.Buffer.BlockCopy(HistGramCC, 0, HistGramC, 0, 256 * sizeof(double));
		for (int i = 0; i < 256; i++)HistGramC[i] = HistGramCC[i];
		Iter++;
		if (Iter >= 1000) return -1;                                                   // 直方图无法平滑为双峰的，返回错误代码
	}
	// 阈值极为两峰之间的最小值 
	bool Peakfound = false;
	for (Y = 1; Y < 255; Y++)
	{
		if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peakfound = true;
		if (Peakfound == true && HistGramCC[Y - 1] >= HistGramCC[Y] && HistGramCC[Y + 1] >= HistGramCC[Y])
			return Y - 1;
	}
	return -1;
}
int main(int argc, char *argv[])
{
	Mat src = imread("15.jpg", 0);
	Mat img;
	namedWindow("SRC", WINDOW_NORMAL);
	imshow("SRC", src);
	Mat hist = Hist(src).t();
	int HistGram[256];
	int thr=0;
	const float* hist_p = (float*)hist.ptr<float>(0);
	for (int i = 0; i<hist.cols; i++) 
	{
		HistGram [i]= hist_p[i];
	}

	thr=GetMinimumThreshold(HistGram);
	cout << thr << endl;
	threshold(src, img, thr, 255, CV_THRESH_BINARY);
	namedWindow("thresholdImg", WINDOW_NORMAL);
	imshow("thresholdImg", img);
	//MaxEntropy(src, hist);;
	//LeftEntropy(src, hist);
	waitKey();
	return 1;
}