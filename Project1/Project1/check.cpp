#include <opencv2\opencv.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <string>
#include <windows.h>
//#include "Plate.h"
#include <stdlib.h>
#include <ctime>
#include <vector>
//以14为边长的正方形的邻域
#define NEIGHBORHOOD 7

using namespace cv;
using namespace std;



//void fastIntegral(Mat srcImg)
//{
//	//Mat grayImg;
//	//int lineIntegration[500][500];
//	//int imageIntegration[500][500];
//	//Mat pixAverage = Mat(srcImg.size(),CV_8UC1);
//	//cvtColor(srcImg, grayImg, COLOR_RGB2GRAY);
//	////得出每列像素的积分值
//	//for (int i = 0; i < grayImg.cols; i++)
//	//{
//	//	for (int j = 0; j < grayImg.rows; j++) {
//	//		if (j == 0) {
//	//			lineIntegration[j][i] = grayImg.at<uchar>(j, i);
//	//		}
//	//		else {
//	//			lineIntegration[j][i] = lineIntegration[j - 1][i] + grayImg.at<uchar>(j - 1, i);
//	//		}
//	//	}
//	//}
//
//	////得出每个坐标点对应的图像积分值
//	//for (int i = 0; i < grayImg.rows; i++) {
//	//	for (int j = 0; j < grayImg.cols; j++) {
//	//		if (j > 1) {
//	//			imageIntegration[i][j] = imageIntegration[i][j - 1] + lineIntegration[i][j];
//	//		}
//	//		else {
//	//			imageIntegration[i][0] = lineIntegration[i][0];
//	//		}
//	//		/*if (i > NEIGHBORHOOD || j > NEIGHBORHOOD)
//	//		{
//	//			if (i < 2 * NEIGHBORHOOD || j < 2 * NEIGHBORHOOD)
//	//			{
//	//				imageIntegration[i - 2 * NEIGHBORHOOD][j - 2 * NEIGHBORHOOD] = 0;
//	//				imageIntegration[i - 2 * NEIGHBORHOOD][j] = 0;
//	//				imageIntegration[i][j - 2 * NEIGHBORHOOD] = 0;
//
//	//			}
//	//			pixAverage[i - NEIGHBORHOOD][j - NEIGHBORHOOD] = imageIntegration[i - 2 * NEIGHBORHOOD][j - 2 * NEIGHBORHOOD] + imageIntegration[i][j] - imageIntegration[i - 2 * NEIGHBORHOOD][j] - imageIntegration[i][j- 2 * NEIGHBORHOOD];
//
//	//		}*/
//	//	}
//	//}
//	////计算各点邻域内像素的平均值
//	//for (int i = 0; i < grayImg.rows; i++) {
//	//	for (int j = 0; j < grayImg.cols; j++) {
//	//		if (i <  NEIGHBORHOOD || j <  NEIGHBORHOOD)
//	//		{
//	//			pixAverage[i][j] = imageIntegration[i + NEIGHBORHOOD][j + NEIGHBORHOOD]/196;
//	//		}
//	//		else {
//	//			pixAverage[i][j] = (imageIntegration[i - NEIGHBORHOOD][j - NEIGHBORHOOD] + imageIntegration[i + NEIGHBORHOOD][j + NEIGHBORHOOD] - imageIntegration[i - NEIGHBORHOOD][j] - imageIntegration[i][j - NEIGHBORHOOD])/196;
//	//		}cout << pixAverage[100][100] << endl;
//	//	}	
//	//}	
//
// }

void fastIntegral(Mat srcImg)
{
	/* pii 为行指针，可以直接将二维数组赋值给 pii*/
	Mat lineIntegration = Mat(srcImg.size(), CV_32SC1);
	Mat imageIntegration = Mat(srcImg.size(), CV_32SC1);
	Mat pixAverage = Mat(srcImg.size(), CV_8UC1);
	Mat grayImg;
	cvtColor(srcImg, grayImg, COLOR_RGB2GRAY);
	int i, j;
	for (i = 0; i < srcImg.cols; i++)
	{
		for (j = 0; j < srcImg.rows; j++)
		{
			if (j == 0) lineIntegration.at<int>(0, i) = 0 + grayImg.at<uchar>(0, i);
			else lineIntegration.at<int>(j, i) = lineIntegration.at<int>(j - 1, i) + grayImg.at<uchar>(j, i);
			/* 式( 2) image［i］［j］ 为原图像的二维数组*/
			/* ps［i］［j］ 为每一列积分的二维数组*/
			if (i == 0) imageIntegration.at<int>(j, 0) = 0 + lineIntegration.at<int>(j, 0);
			else imageIntegration.at<int>(j, i) = imageIntegration.at<int>(j, i - 1) + lineIntegration.at<int>(j, i);
			/* 式( 3) pii［i］［j］ 为积分图像的二维数组*/
		}
	}

	//for (int i = 0; i < srcImg.cols; i++)
	//{
	//	for (int j = 0; j < srcImg.rows; j++)
	//	{
	//		if (j - NEIGHBORHOOD < 0 && i - NEIGHBORHOOD > 0)
	//		{
	//			pixAverage.at<uchar>(j, i) = (imageIntegration.at<int>(j + NEIGHBORHOOD, i + NEIGHBORHOOD) - imageIntegration.at<int>(j + NEIGHBORHOOD, i - NEIGHBORHOOD))/196;

	//		}
	//		else if (i - NEIGHBORHOOD < 0 && j - NEIGHBORHOOD > 0)
	//		{
	//			pixAverage.at<uchar>(j, i) = imageIntegration.at<int>(j + NEIGHBORHOOD, i + NEIGHBORHOOD) - imageIntegration.at<int>(j - NEIGHBORHOOD, i + NEIGHBORHOOD)/196;
	//		}
	//		else if (i - NEIGHBORHOOD < 0 || j - NEIGHBORHOOD > 0)
	//		{
	//			pixAverage.at<uchar>(j, i) = imageIntegration.at<int>(j + NEIGHBORHOOD, i + NEIGHBORHOOD)/196;
	//		}
	//		else
	//		{
	//			pixAverage.at<uchar>(j, i) = imageIntegration.at<int>(j - NEIGHBORHOOD, i - NEIGHBORHOOD) + imageIntegration.at<int>(j + NEIGHBORHOOD, i + NEIGHBORHOOD) - imageIntegration.at<int>(j + NEIGHBORHOOD, i - NEIGHBORHOOD) - imageIntegration.at<int>(j - NEIGHBORHOOD, i + NEIGHBORHOOD)/196;
	//		}
	//	}
	//}
	for (int i = NEIGHBORHOOD; i < srcImg.cols- NEIGHBORHOOD; i++)
	{
		for (int j = NEIGHBORHOOD; j < srcImg.rows- NEIGHBORHOOD; j++)
		{
				pixAverage.at<uchar>(j, i) = (imageIntegration.at<int>(j - NEIGHBORHOOD, i - NEIGHBORHOOD) + imageIntegration.at<int>(j + NEIGHBORHOOD, i + NEIGHBORHOOD) - imageIntegration.at<int>(j + NEIGHBORHOOD, i - NEIGHBORHOOD) - imageIntegration.at<int>(j - NEIGHBORHOOD, i + NEIGHBORHOOD)) / 196;
		}
	}
	//for (int i = 0; i < srcImg.rows; i++)
	//{
	//	for (int j = 0; j < srcImg.rows; j++)
	//	{
	//		cout << (int)pixAverage.at<uchar>(i, j);
	//	}
	//	cout << endl;
	//}
	//namedWindow("dst1", WINDOW_NORMAL);
	//imshow("dst1", pixAverage);
}
	
#define ADJUST 0.4f//大亮小暗，越大花椒盐噪越少
#define KERNEL 40  //均值内核大小，越大花椒盐越少,图像整体性越好
#define KENEL_2 KERNEL/2
//参数表:1、0.4,40 2、

int main(int argc, char *argv[])
{
	int start = 0;
	Mat src = imread("35.bmp");
	Mat dstImg;
	Mat grayImg;
	Mat integralImg;
	Mat object = Mat(src.rows, src.cols, CV_32F);
	Mat imageIntegralNorm;
	//namedWindow("SRC", WINDOW_NORMAL);
	//imshow("SRC", src);
	cvtColor(src, grayImg, COLOR_RGB2GRAY);
	grayImg.copyTo(dstImg);
	start = clock();
	//blur(grayImg,dstImg,Size(KERNEL, KERNEL));
	//对图像积分
	integral(grayImg, integralImg, CV_32S);
	//normalize(integralImg, integralImg, 0, 255, CV_MINMAX);
	//convertScaleAbs(integralImg, imageIntegralNorm);
	//imshow("nor", imageIntegralNorm);
	for (int i = KENEL_2; i < src.rows- KENEL_2; i++) {
		uchar* dstImgPtr = dstImg.ptr<uchar>(i);
		int* integralImgUpPtr = integralImg.ptr<int>(i- KENEL_2);
		int* integralImgDownPtr = integralImg.ptr<int>(i+ KENEL_2);
		for (int j = KENEL_2; j < src.cols- KENEL_2; j++) {
			dstImgPtr[j] = (integralImgUpPtr[j - KENEL_2] + integralImgDownPtr[j + KENEL_2] - integralImgUpPtr[j + KENEL_2] - integralImgDownPtr[j - KENEL_2]) / (KERNEL*KERNEL);
			//cout << integralImgDownPtr[j]<<" ";
			//cout << (int)dstImgPtr[j] << " ";
			//waitKey();
			if (dstImgPtr[j] > 254)dstImgPtr[j] = 255;
			if (dstImgPtr[j] < 0)dstImgPtr[j] = 0;
		}
		//cout << endl;
	}
	namedWindow("dst", 1);
	imshow("dst",dstImg);
	for (int i = 0; i < src.rows; i++) {
		uchar* grayImgPtr = grayImg.ptr<uchar>(i);
		uchar* dstImgPtr = dstImg.ptr<uchar>(i);
		int* objectPtr = object.ptr<int>(i);
		for (int j = 0; j < src.cols; j++) {
			objectPtr[j] = dstImgPtr[j] * (1.0f + ADJUST * ((sqrtf((float)fabs(grayImgPtr[j] - dstImgPtr[j]))) / dstImgPtr[j] - 1.0f));
			if (objectPtr[j] > 254)objectPtr[j] = 255;
			if (objectPtr[j] < 0)objectPtr[j] = 0;

			if (grayImgPtr[j] > objectPtr[j])dstImgPtr[j] = 255;
			else dstImgPtr[j] = 0;
		}
	}
	int time = clock() - start;
	//namedWindow("dst", WINDOW_NORMAL);
	//imshow("dst", dstImg);*/
	cout << src.size << endl;
	//fastIntegral(src);
	cout << time << endl;
	namedWindow("thresholdImg", WINDOW_NORMAL);
	imshow("thresholdImg", dstImg);
	waitKey();
	return 1;
}

