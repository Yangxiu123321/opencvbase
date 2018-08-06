#include <map>
#include <windows.h>
#include <iostream>
#include <assert.h>
#include "twoPass.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

TwoPass::TwoPass()
{
}


TwoPass::~TwoPass()
{
}

// 找到label x的根节点

int find(int x, int parent[])
{
	//assert(x < max_size);

	int i = x;

	while (0 != parent[i])

		i = parent[i];

	return i;
}



// 将label x 和 label y合并到同一个连通域

void union_Label(int x, int y, int parent[])
{
	//CV_Assert(x < max_size && y < max_size);

	int i = x;

	int j = y;

	while (0 != parent[i])

		i = parent[i];

	while (0 != parent[j])

		j = parent[j];

	if (i != j)

		parent[i] = j;
}

/**
@param binary sourceImage,must be a binary picture.
@param id:camera id,left id = 0.right id = others.
*/
void TwoPass::connectedComponentLabelingTwoPass(Mat &binary, CAMERA_ID id)
{		
	// 检测CV_Assert中的内容是否为真，如果是真的，stderr 打印一条出错信息，然后通过调用 abort 来终止程序运行。
	CV_Assert(binary.data);
	CV_Assert(binary.depth() == CV_8U);
	CV_Assert(binary.channels() == 1);
	// create label image
	Mat label;
	label.create(binary.size(), binary.type());

	// initialize label value
	int label_value = 0;
	// 数组清零
	ZeroMemory(parent, sizeof(parent));
	// first pass
	// 注意，得从第二行开始，第二列开始。
	for (int y = 1; y < binary.rows; y++) {

		uchar* p_binary = binary.ptr<uchar>(y);

		uchar* p_binary_up = binary.ptr<uchar>(y - 1);

		uchar* p_label = label.ptr<uchar>(y);

		uchar* p_label_up = label.ptr<uchar>(y - 1);

		for (int x = 0; x < label.cols; x++) {

			p_label[x] = 0;

		}

		for (int x = 1; x < binary.cols; x++)
		{
			if (255 == p_binary[x])
			{

				if (0 == p_binary[x - 1] && 0 == p_binary_up[x]) {

					label_value += 1;

					p_label[x] = (int)label_value;

				}
				else if (255 == p_binary[x - 1] && 0 == p_binary_up[x])
				{
					p_label[x] = p_label[x - 1];
				}
				else if (0 == p_binary[x - 1] && 255 == p_binary_up[x])
				{
					p_label[x] = p_label_up[x];
				}
				else if (255 == p_binary[x - 1] && 255 == p_binary_up[x])
				{
					p_label[x] = p_label[x - 1];
					union_Label(p_label[x - 1], p_label_up[x], parent);
				}
				else {

					cout << " p_binary[x-1] = " << (int)p_binary[x - 1] <<

						" p_binary_up[x] = " << (int)p_binary_up[x] << endl;

				}

			}

		}
	}

	// second pass
	for (int y = 1; y < binary.rows - 1; y++) {

		uchar* p_label = label.ptr<uchar>(y);

		for (int x = 1; x < binary.cols - 1; x++) 
		{
			if (p_label[x] > 0) {
				p_label[x] = find(p_label[x], parent);
			}
		}
	}
	// 根据编号对左右相机编号赋值
	if (0 == id)
	{
		label.copyTo(labelLeft);
	}
	else
	{
		label.copyTo(labelRight);
	}

}

cv::Scalar getRandomColor()
{
	uchar r = 255 * (rand() / (1.0 + RAND_MAX));

	uchar g = 255 * (rand() / (1.0 + RAND_MAX));

	uchar b = 255 * (rand() / (1.0 + RAND_MAX));

	return cv::Scalar(b, g, r);
}



void TwoPass::labelColor(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg)
{
	if (_labelImg.empty() ||

		_labelImg.type() != CV_8UC1)

	{
		return;
	}
	std::map<int, cv::Scalar> colors;

	int rows = _labelImg.rows;
	int cols = _labelImg.cols;

	_colorLabelImg.release();

	_colorLabelImg.create(rows, cols, CV_8UC3);

	_colorLabelImg = cv::Scalar::all(0);

	for (int i = 0; i < rows; i++)
	{
		uchar* data_src = (uchar*)_labelImg.ptr<uchar>(i);

		uchar* data_dst = _colorLabelImg.ptr<uchar>(i);

		for (int j = 0; j < cols; j++)
		{
			int pixelValue = data_src[j];
			if (pixelValue > 0)
			{
				// 检查关联容器中是否有pixelValue这个label编号了，没有则为他们随机分配一个颜色
				if (colors.count(pixelValue) <= 0)
				{
					colors[pixelValue] = getRandomColor();
				}
				cv::Scalar color = colors[pixelValue];

				// 给每个通道的颜色赋值
				*data_dst++ = color[0];

				*data_dst++ = color[1];

				*data_dst++ = color[2];
			}
			else
			{
				data_dst++;

				data_dst++;

				data_dst++;
			}

		}

	}

}

#ifdef OPENNI_H

void TwoPass::deepthToWorld(vector<Point> inputPoint,vector<Point3f> outPutPoint,const VideoFrameRef &frameDepth,const VideoStream &streamDepth)
{
	// 转换为世界坐标系
	openni::DepthPixel *pDepth = (openni::DepthPixel*)frameDepth.getData();
	vector<Point>::iterator iterInput;
	vector<Point3f>::iterator iterOutput = outPutPoint.begin();
	for (iterInput == inputPoint.begin(); iterInput != inputPoint.end(); iterInput++, iterOutput++)
	{
		int x = iterInput->x;
		int y = iterInput->y;
		int z = x + y * frameDepth.getWidth();
		float fx = 0.0f, fy = 0.0f, fz = 0.0f;
		openni::DepthPixel&  rDepth = pDepth[z];
		openni::CoordinateConverter::convertDepthToWorld(streamDepth,
			x, y, rDepth,
			&fx, &fy, &fz);
		
	}
}
#endif //  OPENNI_H

/*
 creat map for label point
*/

void TwoPass::sortLabelToMap(CAMERA_ID id)
{
    Mat labelSrc;
    connectedArea.clear();
	if (0 == id)
	{
		labelLeft.copyTo(labelSrc);
	}
	else
	{
		labelRight.copyTo(labelSrc);
	}
	CV_Assert(!labelSrc.empty());
	CV_Assert(labelSrc.depth() == CV_8U);
	CV_Assert(labelSrc.channels() == 1);
	int rowNumber = labelSrc.rows;
	int colNumber = labelSrc.cols;
	for (int i = 0; i < rowNumber; i++)
	{
		uchar *Ptr = labelSrc.ptr<uchar>(i);
		for (int j = 0; j < colNumber; j++)
		{
			if (Ptr[j] > 0)
			{
				connectedArea[Ptr[j]].push_back(Point(j, i));
			}
		}
	}
}
/**
@param _src sourceImage,must be a binary picture.
@param _dst debuge image which has grayPoint.
*/
#define MAX_POINT_NUM_NOISE 10
void TwoPass::findGravityPoint(CAMERA_ID id) 
{
	// 计算重心
	map<int, vector<Point>>::iterator iter;
	Point gravotyPointTemp;
	vector<Point> gravotyPoint;
	for (iter = connectedArea.begin(); iter != connectedArea.end(); iter++)
	{
		int pointNum = iter->second.size();
		std::cout << pointNum << std::endl;
		// 过滤噪声
		if (pointNum < MAX_POINT_NUM_NOISE)
		{
			continue;
		}
		for (int index = 0; index < pointNum; index++)
		{
			gravotyPointTemp.x += iter->second[index].x;
			gravotyPointTemp.y += iter->second[index].y;
		}
		gravotyPointTemp.x = gravotyPointTemp.x / pointNum;
		gravotyPointTemp.y = gravotyPointTemp.y / pointNum;
		// 把点存储取来
		gravotyPoint.push_back(gravotyPointTemp);
	}
	if (0 == id)
	{
		gravityPointLeft.assign(gravotyPoint.begin(), gravotyPoint.end());
	}
	else
	{
		gravityPointRight.assign(gravotyPoint.begin(), gravotyPoint.end());
	}
}

void TwoPass::gravityPointShow(CAMERA_ID id)
{
	vector<Point> point;

	// 在图上画出重心
	if (0 == id)
	{
		point.assign(gravityPointLeft.begin(), gravityPointLeft.end());
	}
	else
	{
		point.assign(gravityPointRight.begin(), gravityPointRight.end());
	}
	int gravotyPointNum = point.size();
	string pointInfo;
	Mat sketchPad;
	sketchPad.create(labelLeft.size(), labelLeft.type());
	sketchPad = Scalar::all(255);
	for (int index = 0; index < gravotyPointNum; index++)
	{
		circle(sketchPad, point[index], 3, Scalar(0), 1, 8, 0);
	}
	imshow("gravityMap", sketchPad);
	// 输出
	if (0 == id)
	{
		sketchPad.copyTo(gravityMapLeft);
	}
	else
	{
		sketchPad.copyTo(gravityMapRight);
	}
}

void TwoPass::findRectangle(CAMERA_ID id)
{
	map<int, vector<Point>>::iterator iter;
	Mat showRect;
	vector<Point> recCenter;
	if (0 == id)
	{
       showRect.create(labelLeft.size(), CV_8UC3);
	}
	else
	{
	   showRect.create(labelRight.size(), CV_8UC3);
	}
	showRect = Scalar::all(0);
	for (iter = connectedArea.begin(); iter != connectedArea.end(); iter++)
	{
		RotatedRect rect = minAreaRect(iter->second);
		Point2f P[4];
		recCenter.push_back(rect.center);
		rect.points(P);
		for (size_t i = 0; i < 4; i++)
		{
			line(showRect, P[i], P[(i + 1) % 4], Scalar(255), 2);
		}
		imshow("fRect", showRect);
	}
	if (0 == id)
	{
		recCenterLeft.assign(recCenter.begin(), recCenter.end());
	}
	else
	{
		recCenterRight.assign(recCenter.begin(), recCenter.end());
	}

}
void TwoPass::recCenterPointShow(CAMERA_ID id)
{
	vector<Point> point;

	if (0 == id)
	{
		point.assign(recCenterLeft.begin(), recCenterLeft.end());
	}
	else
	{
		point.assign(recCenterRight.begin(), recCenterRight.end());
	}
	int gravotyPointNum = point.size();
	string pointInfo;
	Mat sketchPad;
	sketchPad.create(labelLeft.size(), labelLeft.type());
	sketchPad = Scalar::all(255);
	for (int index = 0; index < gravotyPointNum; index++)
	{
		circle(sketchPad, point[index], 3, Scalar(0), 1, 8, 0);
	}
	imshow("recCenterMap", sketchPad);
	// 输出
	if (0 == id)
	{
		sketchPad.copyTo(recCenterMapLeft);
	}
	else
	{
		sketchPad.copyTo(recCenterMapRight);
	}
}

void TwoPass::pointErrShow(CAMERA_ID id)
{
	Mat errMap(Size(640,480),CV_8UC3,Scalar(255,255,255));
	int errValueX = 0;
	int errValueY = 0;
	Point startX(30, 0);
	Point startY(60, 0);
	Point endX;
	Point endY;
	vector<Point> point_one;
	vector<Point> point_two;
	endX.x = 30;
	endY.x = 60;
	vector<Point>::iterator iter_one = point_one.begin();
	vector<Point>::iterator iter_two = point_two.begin();
	if (0 == id)
	{
		point_one.assign(gravityPointLeft.begin(), gravityPointLeft.end());
		point_two.assign(recCenterLeft.begin(), recCenterLeft.end());
	}
	else
	{
		point_one.assign(gravityPointLeft.begin(), gravityPointLeft.end());
		point_two.assign(recCenterRight.begin(), recCenterRight.end());
	}
	
	for (iter_one = point_one.begin(); iter_one != point_one.end(); iter_one++, iter_two++)
	{

		endX.y = fabs(iter_one->x - iter_two->x);
		endY.y = fabs(iter_one->y - iter_two->y);
		std::cout << endX.x << " " << endY.y << std::endl;
		
	/*	line(errMap, startX, endX, Scalar(0, 0, 255), 1, 8, 0);
		line(errMap, startY, endY, Scalar(255, 0, 0), 1, 8, 0);*/
		imshow("errSw", errMap);
	}
	
}
