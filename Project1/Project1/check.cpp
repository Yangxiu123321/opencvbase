#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;
//41.jpg

int main()
{
	Mat srcImg = imread("judge.bmp",0);
	Mat debugImg = srcImg.clone();
	namedWindow("ԭͼ",1);
	namedWindow("����ͼ",1);
	namedWindow("��Ե���",1);
	imshow("ԭͼ",srcImg);

	Point boundaryPoint[4];

	Point startPoint = Point(8, 170);
	Point endPoint = Point(570,256);

	LineIterator rightTimingPatternIt(srcImg, startPoint, endPoint);
	uchar crtPixel = 255;
	vector<int> timingCounter;
	int crtTimingCounter = 0;
	int timingBuffer = 0;

	//���ߵ��������е�������ÿһ���ضε������������д洢
	for (int x = 0; x < rightTimingPatternIt.count; x++)
	{
		if (abs((uchar)**rightTimingPatternIt - crtPixel) <= 127)
		{
			crtTimingCounter++;
			timingBuffer = 0;
		}
		else
		{
			crtTimingCounter++;
			timingBuffer++;

			if (timingBuffer >= 3)
			{
				timingCounter.push_back(crtTimingCounter - timingBuffer);
				crtTimingCounter = timingBuffer;
				crtPixel = abs(crtPixel - 255);
				timingBuffer = 0;
				if (timingCounter.size() == 5)
				{
					break;
				}

				//��ֵ�߽��
				boundaryPoint[timingCounter.size() - 1] = rightTimingPatternIt.pos();
				circle(debugImg, rightTimingPatternIt.pos(),2,Scalar(255,0,0));
			}
		}
		rightTimingPatternIt++;
	}

	waitKey(0);
}