#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;

#define NBINS 9  
#define THETA 180 / NBINS  
#define CELLSIZE 20  
#define BLOCKSIZE 2  
#define R (CELLSIZE * (BLOCKSIZE) * 0.5)  
/********************************************************************************************************
��������:
�������ͼ��
����˵��:
Mat& srcMat-----------------------�洢ÿ��cellHOG����������������
2)cv::Rect roi--------------------����cell�ľ���λ��
3)std::vector<Mat>& integrals-----�洢��9������ͼ��ÿһ������ͼ�����һ���Ƕȷ�Χ����һ��bins
*********************************************************************************************************/
// �������ͼ  
std::vector<Mat> CalculateIntegralHOG(Mat& srcMat)
{
	//��1������һ��΢�ֵ��ݶ�ͼ��  
	cv::Mat   sobelMatX;
	cv::Mat   sobelMatY;

	cv::Sobel(srcMat, sobelMatX, CV_32F, 1, 0);
	cv::Sobel(srcMat, sobelMatY, CV_32F, 0, 1);

	std::vector<Mat> bins(NBINS);
	for (int i = 0; i < NBINS; i++)
	{
		bins[i] = Mat::zeros(srcMat.size(), CV_32F);
	}
	cv::Mat   magnMat;
	cv::Mat   angleMat;
	//��2������ת��,����ÿһ����X�����Y�����ϵ��ݶȣ�ʵ�ֵѿ�������ͼ������ת��  
	cartToPolar(sobelMatX, sobelMatY, magnMat, angleMat, true);
	//��3�������������д�����ʼ������ȫ����ģ���Ϊ�ڽ��ѿ�������ת��Ϊ������֮�󣬽Ƕȵķ�Χ��[0,360]  
	//     ���������д��������еĽǶ�������[0,180]�������  
	add(angleMat, Scalar(180), angleMat, angleMat<0);                //���angleMat<0�����180  
	add(angleMat, Scalar(-180), angleMat, angleMat >= 180);          //���angleMat>=180�����180  
																	 //��4���������д��뽫�ǶȾ���ת��Ϊһ���Ҷ�ֵ��Χ��[0,9]֮���ͼ��  
	angleMat /= THETA;
	//��5���������ѭ������ʵ�ǽ�ͼ����ݶȷ�ֵ���󰴾Ÿ���ͬ������ݶȽǶȣ���ÿ���Ƕȷ�Χ����Ӧ����ݶȷ�ֵ  
	//     �洢����Ӧ�ľ���ͼ��֮�ϣ���ʵ���ǽ��ݶȷ�ֵ����ͼ���ղ�ͬ���ݶȷ�ֵ�Ƕȷ�Ϊ9���ݶȷ�ֵ��ͼ��  
	for (int y = 0; y < srcMat.rows; y++)
	{
		for (int x = 0; x < srcMat.cols; x++)
		{
			int ind = angleMat.at<float>(y, x);
			bins[ind].at<float>(y, x) += magnMat.at<float>(y, x);
		}
	}
	//��6�������������ɵ�9�Ų�ͬ�Ƕȵ��ݶȷ�ֵ��������9�Ų�ͬ���ݶȷ�ֵ�Ļ���ͼ�������Ժ�  
	//     ����ͼ���ÿһ��ʹ�����һ�����Ͻǣ������ݶȷ�ֵ֮�ͣ����ɵ�9������ͼҲ����9��  
	//     bins����ͬbins�ϵ�HOGǿ��  
	std::vector<Mat> integrals(NBINS);
	for (int i = 0; i < NBINS; i++)
	{
		integral(bins[i], integrals[i]);
	}
	return integrals;
}
/********************************************************************************************************
��������:
���㵥��cell HOG����
����˵��:
1)cv::Mat& HOGCellMat-------------�洢ÿ��cellHOG����������������
2)cv::Rect roi--------------------����cell�ľ���λ��
3)std::vector<Mat>& integrals-----�洢��9������ͼ��ÿһ������ͼ�����һ���Ƕȷ�Χ����һ��bins
*********************************************************************************************************/
void cacHOGinCell(cv::Mat& HOGCellMat, cv::Rect roi, std::vector<Mat>& integrals)
{
	//��1��ͨ��9������ͼ�����ʵ��HOG�ļ��㣬HOG���ֱ��ͼ��9��bins��ÿ��bins�Ͷ�Ӧһ�Ż���ͼ��  
	int x0 = roi.x;                              //ȷ����������cell�����Ͻǵ�����  
	int y0 = roi.y;
	int x1 = x0 + roi.width;
	int y1 = y0 + roi.height;                    //ȷ����������cell�����½ǵ�����  

	for (int i = 0; i <NBINS; i++)
	{
		//��2�����ݾ��ε����Ͻǵ�����½ǵ������  
		cv::Mat integral = integrals[i];

		float a = integral.at<double>(y0, x0);
		float b = integral.at<double>(y1, x1);
		float c = integral.at<double>(y0, x1);
		float d = integral.at<double>(y1, x0);

		HOGCellMat.at<float>(0, i) = b - c - d + a;//ÿѭ��һ�Σ�����һ���ݶȷ����ϵ�HOG��������ʵ����  
												   //ÿѭ��һ�Σ��ͼ����ݶȷ���ֱ��ͼ�ϵ�һ��bins  
	}
}
/********************************************************************************************************
��������:
��ȡ��ǰ���ڵ�HOGֱ��ͼ----�˿���ʵ�����ڼ��㵥��Block��HOG�ݶȷ���ֱ��ͼ
����˵��:
1)cv::Point pt--------------------����Block�����ĵ�����
2)std::vector<cv::Mat>& integrals-----�洢��9������ͼ��ÿһ������ͼ�����һ���Ƕȷ�Χ����һ��bins
*********************************************************************************************************/
cv::Mat getHog(cv::Point pt, std::vector<cv::Mat>& integrals)
{
	if (pt.x - R<0 || pt.y - R<0 || pt.x + R >= integrals[0].cols || pt.y + R >= integrals[0].rows)
	{
		return cv::Mat();
	}
	//��1��BLOCK��HOGֱ��ͼ---�������˵��BLOCKSIZE*BLOCKSIZE��4��cell��HOG����ֱ��ͼ��������  
	//     ���һ��BLOCK��HOG����ֱ��ͼ����������  
	cv::Mat    hist(cv::Size(NBINS*BLOCKSIZE*BLOCKSIZE, 1), CV_32F);//36��
	cv::Point  t1(0, pt.y - R);
	int c = 0;
	//��2��������:ͨ������������ѭ�����ͱ�����4��cell�����ҽ�4��cell��HOG�������������һ��  
	//     ά���Ƚϴ��BLOCK��HOG��������  
	for (int i = 0; i<BLOCKSIZE; i++)
	{
		t1.x = pt.x - R;
		for (int j = 0; j<BLOCKSIZE; j++)
		{
			//��3����ȡ��ǰ���ڣ����оֲ�HOGֱ��ͼ����  
			cv::Rect roi(t1, t1 + cv::Point(CELLSIZE, CELLSIZE));
			//hist_temp��ȡhist��0��9�У��Ժ�ÿ���ۼ�9��,hist_temp��hist����һƬ�ڴ�
			cv::Mat  hist_temp = hist.colRange(c, c + NBINS);
			//��4������roiȷ���ľ������򣬼��㵥��cell��HOGֱ��ͼ(�䱾�ʾ���һ������������)  
			cacHOGinCell(hist_temp, roi, integrals);
			t1.x += CELLSIZE;
			c += NBINS;
		}
		t1.y = CELLSIZE;
	}//for i  
	 //��3�����÷���2���й�һ��  
	cv::normalize(hist, hist, 1, 0, NORM_L2);
	return hist;
}
/********************************************************************************************************
��������:
��������ͼ���HOG�ݶȷ���ֱ��ͼ---HOG����
����˵��:
cv::Mat srcImage------ԭʼ�������ɫͼ��
*********************************************************************************************************/
std::vector<Mat> cacHOGFeature(cv::Mat srcImage)
{
	cv::Mat          grayImage;
	std::vector<Mat> HOGMatVector;
	cv::cvtColor(srcImage, grayImage, CV_RGB2GRAY);  
	grayImage.convertTo(grayImage, CV_8UC1);
	//��1��9����ͬ�ݶȷ����ϵ�9���ݶȷ�ֵ�Ļ���ͼ�������  
	std::vector<Mat> integrals = CalculateIntegralHOG(grayImage);
	Mat image = grayImage.clone();
	image *= 0.5;
	//��2������ȫͼ�񣬼������յ��ݶȷ���ֱ��ͼHOG  
	cv::Mat HOGBlockMat(Size(NBINS, 1), CV_32F);
	for (int y = CELLSIZE / 2; y < grayImage.rows; y += CELLSIZE)
	{
		for (int x = CELLSIZE / 2; x < grayImage.cols; x += CELLSIZE)
		{
			//��3����ȡ��ǰ����HOG����ʵ��ǰ�Ĵ��ھ���һ��Block��ÿ��Block���ĸ�cell��ɣ�ÿ��CellΪ20*20  
			//     �˿飬����ľ��ǵ���Block���ݶȷ���ֱ��ͼHOG  
			cv::Mat hist = getHog(Point(x, y), integrals);
			if (hist.empty())continue;
			HOGBlockMat = Scalar(0);
			for (int i = 0; i < NBINS; i++)
			{
				for (int j = 0; j < BLOCKSIZE; j++)
				{
					HOGBlockMat.at<float>(0, i) += hist.at<float>(0, i + j * NBINS);
				}
			}
			//��4��L2������һ��:����õ���ÿ��Block�ĵľ������L2������һ����ʹ��ת��Ϊһ��Block��HOG��������  
			normalize(HOGBlockMat, HOGBlockMat, 1, 0, CV_L2);
			//��5�����ÿ�õ�һ��Block��HOG���������ʹ���HOGMatVector�����HOGMatVector��ʵ��������ͼ���HOG����������  
			//     ��Ȼ���������HOGMatVector���Ǹ���ά�������ʽ�������Ҫ����SVM������з���Ļ�������Ҫ��������Ϊһ  
			//     ά��������  
			HOGMatVector.push_back(HOGBlockMat);
			Point center(x, y);
			//��6������HOG����ͼ  
			for (int i = 0; i < NBINS; i++)
			{
				double theta = (i * THETA) * CV_PI / 180.0;
				Point rd(CELLSIZE*0.5*cos(theta), CELLSIZE*0.5*sin(theta));
				Point rp = center - rd;
				Point lp = center + rd;
				line(image, rp, lp, Scalar(255 * HOGBlockMat.at<float>(0, i), 255, 255));
			}
		}
	}
	imshow("out", image);
	imwrite("out.bmp",image);
	return HOGMatVector;
}
/********************************************************************************************************
ģ�鹦��:
����̨Ӧ�ó�������:Main����
*********************************************************************************************************/
int main()
{
	cv::Mat srcImage = cv::imread("F:\\opencvstu\\opencvbase\\Project1\\Project1\\47.png");
	if (srcImage.empty())
		return -1;
	cv::imshow("srcImage ", srcImage);
	std::vector<Mat> HOGFeatureMat = cacHOGFeature(srcImage);
	cv::waitKey(0);
	return 0;
}