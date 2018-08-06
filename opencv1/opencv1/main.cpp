#include "twoPass.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/video/background_segm.hpp"

using namespace cv;
using namespace std;

int main(void) 
{
	TwoPass connectArea;
	VideoCapture frame(1);
	//frame.open("D:\\program files\\mvUSB\\MindVision\\Video\\Sample2.avi");
	bool start = true;
	Ptr<BackgroundSubtractorMOG2> bgsubtractor = createBackgroundSubtractorMOG2();
	while (start)
	{
		Mat image;
		frame >> image;
		if (!image.data) {

			cout << "No image data" << endl;

			getchar();

			return -1;
		}
		Mat fgMaskLeft;
		bgsubtractor->apply(image, fgMaskLeft, 0.05);

		Mat thresh;
		threshold(fgMaskLeft, thresh, 10, 255, THRESH_BINARY);

		Mat temp;
		medianBlur(thresh, temp, 3);
		dilate(temp, temp, Mat(), Point(-1, -1), 3);
		erode(temp, temp, Mat(), Point(-1, -1), 6);
		dilate(temp, temp, Mat(), Point(-1, -1), 3);

        // label
		connectArea.connectedComponentLabelingTwoPass(temp);
		// Map
		connectArea.sortLabelToMap();
		// gravityPoint
		connectArea.findGravityPoint();
		// gravityMap
		connectArea.gravityPointShow();
		// findRectAngle
		connectArea.findRectangle(0);
		// recCenterMap
		connectArea.recCenterPointShow(0);
		// err
		//connectArea.pointErrShow(0);
		// show
		imshow("image", image);

	    imshow("thresh", thresh);

		imshow("fg", temp);

		if (waitKey(30) == 'q')
		{
			start = false;
		}

	}
	return 0;
}





