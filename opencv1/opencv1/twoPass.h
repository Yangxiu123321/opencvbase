#pragma once
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

typedef int CAMERA_ID;

class TwoPass
{
public:
	TwoPass();
	virtual ~TwoPass();

	void connectedComponentLabelingTwoPass(Mat &binary,CAMERA_ID id = 0);

	void labelColor(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg);

	void sortLabelToMap(CAMERA_ID id = 0);

    void findGravityPoint(CAMERA_ID id = 0);

	void findRectangle(CAMERA_ID id = 0);

	void gravityPointShow(CAMERA_ID id = 0);

	void recCenterPointShow(CAMERA_ID id = 0);

	void pointErrShow(CAMERA_ID id = 0);

#ifdef OPENNI_H
	void deepthToWorld(vector<Point> inputPoint, vector<Point3f> outPutPoint, const VideoFrameRef &frameDepth, const VideoStream &streamDepth);
#endif

	Mat getGravityMapLeft() {
		return gravityMapLeft;
	}
	Mat getGravityMapRight() {
		return gravityMapRight;
	}
	vector<Point> getGravityPointLeft() {
		return gravityPointLeft;
	}
	vector<Point> getGravityPointRight() {
		return gravityPointRight;
	}
private:
	int parent[1000] = { 0 };

	map<int, vector<Point>> connectedArea;

	Mat binaryLeft;
	Mat binaryRight;

	Mat labelLeft;
	Mat labelRight;

	Mat gravityMapLeft;
	Mat gravityMapRight;;

	Mat recCenterMapLeft;
	Mat recCenterMapRight;

	// 像素坐标系(重心)
	vector<Point> gravityPointLeft;
	vector<Point> gravityPointRight;

	//像素坐标系(矩形中心)
	vector<Point> recCenterLeft;
	vector<Point> recCenterRight;

	// 相机坐标系
	vector<Point> cameraPointLeft;
	vector<Point> cameraPointRight;
};

