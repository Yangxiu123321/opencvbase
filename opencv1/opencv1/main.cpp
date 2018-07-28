
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/highgui/highgui.hpp"

#include <map>

#include <cassert>

#include <iostream>

using namespace std;

using namespace cv;



const int max_size = 1000;

int parent[max_size] = { 0 };



// 找到label x的根节点

int find(int x, int parent[]) {

	assert(x < max_size);

	int i = x;

	while (0 != parent[i])

		i = parent[i];

	return i;

}



// 将label x 和 label y合并到同一个连通域

void union_label(int x, int y, int parent[]) {

	assert(x < max_size && y < max_size);

	int i = x;

	int j = y;

	while (0 != parent[i])

		i = parent[i];

	while (0 != parent[j])

		j = parent[j];

	if (i != j)

		parent[i] = j;

}


void myConnectedComponentLabelingTwoPass(

	Mat& binary, Mat& label) {
    // 检测CV_Assert中的内容是否为真，如果是真的，stderr 打印一条出错信息，然后通过调用 abort 来终止程序运行。
	CV_Assert(binary.data);	
	CV_Assert(binary.depth() == CV_8U);
	CV_Assert(binary.channels() == 1);



	// create label image

	label.create(binary.size(), binary.type());



	// initialize label value

	int label_value = 0;



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
					union_label(p_label[x - 1], p_label_up[x], parent);
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

		for (int x = 1; x < binary.cols - 1; x++) {

			if (p_label[x] > 0) {

				p_label[x] = find(p_label[x], parent);

			}

		}

	}

}



//  Connected Component Analysis/Labeling -- Color Labeling 

//  Author:  www.icvpr.com  

//  Blog  :  http://blog.csdn.net/icvpr 

cv::Scalar icvprGetRandomColor()

{

	uchar r = 255 * (rand() / (1.0 + RAND_MAX));

	uchar g = 255 * (rand() / (1.0 + RAND_MAX));

	uchar b = 255 * (rand() / (1.0 + RAND_MAX));

	return cv::Scalar(b, g, r);

}



void icvprLabelColor(const cv::Mat& _labelImg, cv::Mat& _colorLabelImg)

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

					colors[pixelValue] = icvprGetRandomColor();

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

void findGravityPoint(cv::InputArray _src, cv::OutputArray _dst)
{
	Mat labelSrc;
	Mat gravityImg;

	map<int, vector<Point>> gravityPoint;

	_src.copyTo(labelSrc);
	_src.copyTo(_dst);

	int rowNumber = labelSrc.rows;
	int colNumber = labelSrc.cols;
	for (int i = 0; i < rowNumber; i++)
	{
		uchar *Ptr = labelSrc.ptr<uchar>(i);
		for (int j = 0; j < colNumber; j++)
		{
			if (Ptr[j] > 0)
			{
				gravityPoint[Ptr[j]].push_back(Point(i, j));
			}
		}
	}
	// 计算重心
	map<int, vector<Point>>::iterator iter;
	for (iter = gravityPoint.begin(); iter != gravityPoint.end(); iter++)
	{
		int size = iter->second.size();
		for(int index = 0;index < size;index++)
		{
			iter->second[index]
				
		}
	}



}


int main() {

	// load image

	const char* imageName = "C:\\Users\\Yang\\Pictures\\opencv\\binary1.bmp";

	Mat image;

	image = imread(imageName, 1);

	if (!image.data) {

		cout << "No image data" << endl;

		getchar();

		return -1;

	}



	// convert to gray

	cvtColor(image, image, CV_RGB2GRAY);// convert 3-channel image to 1-channel image



										// threshold

	Mat thresh;

	threshold(image, thresh, 100, 255, THRESH_BINARY);



	// label

	Mat label;

	myConnectedComponentLabelingTwoPass(thresh, label);


	Mat gravityPoint;



   // find GrayPoint



	// show

	imshow("image", image);

	imshow("thresh", thresh);

	imshow("label", label);


	waitKey(0);

}





