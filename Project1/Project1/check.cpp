#include <iostream>

#define CV_CN_MAX     512
#define CV_CN_SHIFT   3
#define CV_DEPTH_MAX  (1 << CV_CN_SHIFT) //8 110

#define CV_8U   0
#define CV_8S   1
#define CV_16U  2
#define CV_16S  3
#define CV_32S  4
#define CV_32F  5
#define CV_64F  6
#define CV_16F  7

#define CV_MAT_DEPTH_MASK       (CV_DEPTH_MAX - 1)//5
#define CV_MAT_DEPTH(flags)     ((flags) & CV_MAT_DEPTH_MASK)//(flags & 0101)

#define CV_MAKETYPE(depth,cn) (CV_MAT_DEPTH(depth) + (((cn)-1) << CV_CN_SHIFT))
#define CV_MAKE_TYPE CV_MAKETYPE

#define CV_8UC1 CV_MAKETYPE(CV_8U,1) //6 110
#define CV_8UC2 CV_MAKETYPE(CV_8U,2) //12 1100
#define CV_8UC3 CV_MAKETYPE(CV_8U,3) //24 11000
#define CV_8UC4 CV_MAKETYPE(CV_8U,4) //48 110000
#define CV_8UC(n) CV_MAKETYPE(CV_8U,(n))// 3*2^(n-1)

#define CV_8SC1 CV_MAKETYPE(CV_8S,1)// 1+6
#define CV_8SC2 CV_MAKETYPE(CV_8S,2)// 1+12
#define CV_8SC3 CV_MAKETYPE(CV_8S,3)// 1+24
#define CV_8SC4 CV_MAKETYPE(CV_8S,4)// 1+48
#define CV_8SC(n) CV_MAKETYPE(CV_8S,(n))

#define CV_16UC1 CV_MAKETYPE(CV_16U,1)//6
#define CV_16UC2 CV_MAKETYPE(CV_16U,2)//12
#define CV_16UC3 CV_MAKETYPE(CV_16U,3)//24
#define CV_16UC4 CV_MAKETYPE(CV_16U,4)
#define CV_16UC(n) CV_MAKETYPE(CV_16U,(n))

int main(void)
{
	std::cout << CV_8UC4;
	return 0;
}