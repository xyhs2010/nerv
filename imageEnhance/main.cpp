#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include "MatLib.h"
#include <opencv/cv.hpp>
#include <stdarg.h>
#include "guidedfilter.h"
#include <math.h>


using namespace std;
using namespace cv;

static void help(cv::CommandLineParser *);

Mat src, dst;
std::string outputPath;

float enhance[256];

int winCenter = 128;
int winHalfWid = 127;

void calcEnhance();
int changeVal(int origin, float enhance);

int element_shape = MORPH_RECT;

//the address of variable which receives trackbar position update
int max_iters = 20;
int open_close_pos = 0;
int erode_dilate_pos = 0;
int Start_Strength = 14;
int strength_pos = Start_Strength;

//图像gamma矫正
void MyGammaCorrection(Mat&, Mat&, float fGamma);

//图像直方图均衡化，也是图像增强的一种
static Mat calchistcontrol(vector<Mat> rgb);

//其他的图像滤波算法，L0滤波，见http://www.cse.cuhk.edu.hk/~leojia/projects/L0smoothing/
cv::Mat L0Smoothing(cv::Mat &im8uc3, double lambda, double kappa);

void showTwoImages(char *name, Mat &image1, Mat &image2);

#define min_uchar(a, b) (((a) < (b)) ? (a) : (b))
#define max_uchar(a, b) (((a) < (b)) ? (b) : (a))

//图像增强算法
static void OpenClose(int, void*);

// callback function for erode/dilate trackbar
static void ErodeDilate(int, void*);




int main(int argc, char** argv)
{
	cv::CommandLineParser parser(argc, argv, "{h help usage ?| |print this message}{ @inputImage| |image to process}{@outputImage| |image to be saved}");
	if (parser.has("h"))
	{
		help(&parser);
		return 0;
	}
	std::string filename = parser.get<std::string>("@inputImage");
    outputPath = parser.get<std::string>("@outputImage");
	if ((src = imread(filename, 1)).empty())
	{
		help(&parser);
		return -1;
	}
	imshow("Original", src);

	Mat gamma;
	float g = 1/2.2;
	//MyGammaCorrection(src, gamma, g);
	

	//create windows for output images
	/* namedWindow("Open/Close", 1); */
	/* namedWindow("Erode/Dilate", 1); */

	open_close_pos = erode_dilate_pos = max_iters;
	createTrackbar("iterations", "Original", &open_close_pos, max_iters * 2 + 1, OpenClose);
	/* createTrackbar("iterations", "Erode/Dilate", &erode_dilate_pos, max_iters * 2 + 1, ErodeDilate); */

	/* createTrackbar("strength", "Original", &strength_pos, max_iters*2+1, OpenClose); */
	createTrackbar("windowmid", "Original", &winCenter, 255, OpenClose);
	createTrackbar("windowhalfwid", "Original", &winHalfWid, 512, OpenClose);


	for (;;)
	{
		int c;

		OpenClose(open_close_pos, 0);
		//ErodeDilate(erode_dilate_pos, 0);
		c = waitKey(0);

		if ((char)c == 27)
			break;
		if ((char)c == 'e')
			element_shape = MORPH_ELLIPSE;
		else if ((char)c == 'r')
			element_shape = MORPH_RECT;
		else if ((char)c == 'c')
			element_shape = MORPH_CROSS;
		else if ((char)c == ' ')
			element_shape = (element_shape + 1) % 3;
	}

	return 0;
}

static void help(cv::CommandLineParser *parser)
{

	printf("\nEnhance document image\n");
    parser->printMessage();
	printf("This program also shows use of rect, ellipse and cross kernels\n\n"
        "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tr - use rectangle structuring element\n"
		"\te - use elliptic structuring element\n"
		"\tc - use cross-shaped structuring element\n"
		"\tSPACE - loop through all the options\n");
}

float windowFunc(float x) {
#ifdef DEBUG
    if (x < -1 || x > 1) {
        exit(1);
    }
#endif
    return sin(x * M_PI / 2);
}

void calcEnhance() {
    float res;
    enhance[0] = 0;
    for (int i = 1; i < 256; i++) {
        if (i <= winCenter - winHalfWid) {
            res = -1;
        } else if ( i >= winCenter + winHalfWid) {
            res = 1;
        } else {
            res = windowFunc((i - winCenter) / ((float)winHalfWid)); 
        }
        enhance[i] = res;
    }
}

int changeVal(int origin, float enhance) {
    origin -= 128;
    
    float lambda = M_PI / 256;
    int change = enhance * cos(lambda * origin) / lambda;

    if (origin + change > 127)
        change = 127 - origin;
    else if (origin + change < -128)
        change = -128 - origin;
    return change;
}

//图像gamma矫正
void MyGammaCorrection(Mat& src, Mat& dst, float fGamma)
{
	CV_Assert(src.data);
	CV_Assert(src.depth() != sizeof(uchar));
	// build look up table
	unsigned char lut[256];
	for (int i = 0; i < 256; i++)
	{
		lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
	}

	dst = src.clone();
	const int channels = dst.channels();
	switch (channels)
	{
	case 1:
	{

		MatIterator_<uchar> it, end;
		for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)
			//*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
			*it = lut[(*it)];

		break;
	}
	case 3:
	{

		MatIterator_<Vec3b> it, end;
		for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++)
		{
			//(*it)[0] = pow((float)(((*it)[0])/255.0), fGamma) * 255.0;
			//(*it)[1] = pow((float)(((*it)[1])/255.0), fGamma) * 255.0;
			//(*it)[2] = pow((float)(((*it)[2])/255.0), fGamma) * 255.0;
			(*it)[0] = lut[((*it)[0])];
			(*it)[1] = lut[((*it)[1])];
			(*it)[2] = lut[((*it)[2])];
		}

		break;

	}
	}
}

//其他的图像滤波算法，L0滤波，见http://www.cse.cuhk.edu.hk/~leojia/projects/L0smoothing/
cv::Mat L0Smoothing(cv::Mat &im8uc3, double lambda = 2e-2, double kappa = 2.0) {
	// convert the image to double format
	int row = im8uc3.rows, col = im8uc3.cols;
	cv::Mat S;
	im8uc3.convertTo(S, CV_64FC3, 1. / 255.);

	cv::Mat fx(1, 2, CV_64FC1);
	cv::Mat fy(2, 1, CV_64FC1);
	fx.at<double>(0) = 1; fx.at<double>(1) = -1;
	fy.at<double>(0) = 1; fy.at<double>(1) = -1;

	cv::Size sizeI2D = im8uc3.size();
	cv::Mat otfFx = psf2otf(fx, sizeI2D);
	cv::Mat otfFy = psf2otf(fy, sizeI2D);

	cv::Mat Normin1[3];
	cv::Mat single_channel[3];
	cv::split(S, single_channel);
	for (int k = 0; k < 3; k++) {
		cv::dft(single_channel[k], Normin1[k], cv::DFT_COMPLEX_OUTPUT);
	}
	cv::Mat Denormin2(row, col, CV_64FC1);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			cv::Vec2d &c1 = otfFx.at<cv::Vec2d>(i, j), &c2 = otfFy.at<cv::Vec2d>(i, j);
			Denormin2.at<double>(i, j) = sqr(c1[0]) + sqr(c1[1]) + sqr(c2[0]) + sqr(c2[1]);
		}
	}

	double beta = 2.0*lambda;
	double betamax = 1e5;

	while (beta < betamax) {
		cv::Mat Denormin = 1.0 + beta*Denormin2;

		// h-v subproblem
		cv::Mat dx[3], dy[3];
		for (int k = 0; k < 3; k++) {
			cv::Mat shifted_x = single_channel[k].clone();
			circshift(shifted_x, 0, -1);
			dx[k] = shifted_x - single_channel[k];

			cv::Mat shifted_y = single_channel[k].clone();
			circshift(shifted_y, -1, 0);
			dy[k] = shifted_y - single_channel[k];
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				double val =
					sqr(dx[0].at<double>(i, j)) + sqr(dy[0].at<double>(i, j)) +
					sqr(dx[1].at<double>(i, j)) + sqr(dy[1].at<double>(i, j)) +
					sqr(dx[2].at<double>(i, j)) + sqr(dy[2].at<double>(i, j));

				if (val < lambda / beta) {
					dx[0].at<double>(i, j) = dx[1].at<double>(i, j) = dx[2].at<double>(i, j) = 0.0;
					dy[0].at<double>(i, j) = dy[1].at<double>(i, j) = dy[2].at<double>(i, j) = 0.0;
				}
			}
		}

		// S subproblem
		for (int k = 0; k < 3; k++) {
			cv::Mat shift_dx = dx[k].clone();
			circshift(shift_dx, 0, 1);
			cv::Mat ddx = shift_dx - dx[k];

			cv::Mat shift_dy = dy[k].clone();
			circshift(shift_dy, 1, 0);
			cv::Mat ddy = shift_dy - dy[k];
			cv::Mat Normin2 = ddx + ddy;
			cv::Mat FNormin2;
			cv::dft(Normin2, FNormin2, cv::DFT_COMPLEX_OUTPUT);
			cv::Mat FS = Normin1[k] + beta*FNormin2;
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					FS.at<cv::Vec2d>(i, j)[0] /= Denormin.at<double>(i, j);
					FS.at<cv::Vec2d>(i, j)[1] /= Denormin.at<double>(i, j);
				}
			}
			cv::Mat ifft;
			cv::idft(FS, ifft, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					single_channel[k].at<double>(i, j) = ifft.at<cv::Vec2d>(i, j)[0];
				}
			}
		}
		beta *= kappa;
		std::cout << '.';
	}
	cv::merge(single_channel, 3, S);
	return S;
}

//图像直方图均衡化，也是图像增强的一种
static Mat calchistcontrol(vector<Mat> rgb)
{

	vector<Mat> rgb_planes;
	rgb_planes = rgb;
	/// 设定bin数目  
	int histSize = 255;

	/// 设定取值范围 ( R,G,B) )  
	float range[] = { 0, 255 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat r_hist, g_hist, b_hist;

	/// 计算直方图:  
	calcHist(&rgb_planes[0], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&rgb_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&rgb_planes[2], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

	// 创建直方图画布  
	int hist_w = 400; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(0, 0, 0));

	/// 将直方图归一化到范围 [ 0, histImage.rows ]  
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// 在直方图画布上画出直方图  
	// 	draw(r_hist, histSize, histImage, bin_w, hist_h, 1);
	// 	draw(g_hist, histSize, histImage, bin_w, hist_h, 2);
	// 	draw(b_hist, histSize, histImage, bin_w, hist_h, 3);
	return histImage;
}

/* void showMultipleImages(char *name, int count, ...) */
/* { */
/* 	va_list argv; */
/* 	vector<Mat> input_mat; */
/* 	int dstWidth = 0; */
/* 	int dstHeight = 0; */
/* 	va_start(argv, count); */
/* 	for (int i=0;i<count;i++) */
/* 	{ */
/* 		Mat img = va_arg(argv, Mat); */
/* 		dstWidth += img.cols; */
/* 		dstHeight = max(dstHeight, img.rows); */
/* 		input_mat.push_back(img); */
/* 	} */
/* 	cv::Mat dst = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(0, 0, 0)); */
/* 	int start = 0; */
/* 	for (int i=0;i<count;i++) */
/* 	{ */
/* 		Mat img = input_mat[i]; */
/* 		Rect roi = (Rect(start, 0,img.cols, img.rows)); */
/* 		Mat targetRoI = dst(roi); */
/* 		img.copyTo(targetRoI); */
/* 		start += img.cols; */
/* 	} */
/* 	va_end(argv); */
/* 	imshow(name, dst); */
/* } */

void showTwoImages(char *name, Mat &image1, Mat &image2)
{
	int dstWidth = image1.cols + image2.cols;
	int dstHeight = max(image1.rows, image2.rows);

	cv::Mat dst = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Rect roi(cv::Rect(0, 0, image1.cols, image1.rows));
	cv::Mat targetROI = dst(roi);
	image1.copyTo(targetROI);
	targetROI = dst(cv::Rect(image1.cols, 0, image1.cols, image1.rows));
	image2.copyTo(targetROI);
	imshow(name, dst);
}

//图像增强算法
static void OpenClose(int, void*)
{

	double sigma = 3, threshold = 5, amount = 0.25;
	Mat original = src.clone();
    Mat tmp = src.clone();

    calcEnhance();
	//图像滤波
	//可以为双边滤波，guided image filter，但双边滤波速度较慢
	//bilateralFilter(original, src, 10, 20, 20); //双边滤波
	int r = 2; // try r=2, 4, or 8
	double eps = 0.1 * 0.1; // try eps=0.1^2, 0.2^2, 0.4^2

	eps *= 255 * 255;   // Because the intensity range of our images is [0, 255]
	tmp = guidedFilter(original, original, r, eps); // guided image filter
	
	Mat dis = tmp.clone();


//  去除不同的光照
 	int n = open_close_pos - max_iters;
 	int an = n > 0 ? n : -n;
 	Mat element = getStructuringElement(element_shape, Size(an * 2 + 1, an * 2 + 1), Point(an, an));
 	if (n < 0)
 		morphologyEx(tmp, dis, MORPH_OPEN, element);
 	else
 		morphologyEx(tmp, dis, MORPH_CLOSE, element);
 	imshow("Open/Close", dis);
 	dis = n > 0 ? tmp / dis * 255 : dis / tmp * 255;
 	imshow("before_dis1", dis);

// 调色
    Mat expose = dis.clone();
    float strength = (strength_pos - max_iters + 0.0) / max_iters;
    int row = tmp.rows;
    int step = tmp.step;

    const int channels = expose.channels();
    switch(channels) {
        case 1: 
            {
                MatIterator_<uchar> it, end;
                for (it=expose.begin<uchar>(), end=expose.end<uchar>(); it != end; ++it)
                    *it += changeVal(*it, enhance[*it]);
                break;
            }
        case 3:
            {
                MatIterator_<Vec3b> it, end;
                for (it=expose.begin<Vec3b>(), end=expose.end<Vec3b>(); it != end; ++it)
                {
                    int mean = (*it)[0] + (*it)[1] + (*it)[2];
                    mean = mean / 3;
                    float rate = enhance[mean];
                    (*it)[0] += changeVal((*it)[0], rate);
                    (*it)[1] += changeVal((*it)[1], rate);
                    (*it)[2] += changeVal((*it)[2], rate);
                }
                break;
            }
    }

    imshow("strength", expose);
    //showMultipleImages("src/darken", 2, src, expose);



	//图像锐化
	Mat ssrc = expose.clone();
	Mat blurred; 
	//多种可能的滤波算法，双边滤波，高斯滤波，guided image filter，中值滤波
	//bilateralFilter(ssrc, blurred, 10, 100, 100);
	GaussianBlur(ssrc, blurred, Size(), sigma, sigma);
	//blurred = guidedFilter(ssrc, ssrc, r, eps);
	//medianBlur(ssrc, blurred, an*2+1);


	Mat lowContrastMask = abs(ssrc - blurred) < threshold;
	Mat sharpened = ssrc*(1 + amount) + blurred*(-amount);
	ssrc.copyTo(sharpened, lowContrastMask);
	/* imwrite("data/paper_save.jpg", sharpened); */
	/* imshow("diff", 255*abs(sharpened - ssrc)); */
    imshow("diff", sharpened);
	/* imwrite("data/res_s.jpg", ssrc); */
	/* imwrite("data/res.jpg", sharpened); */
    if (!outputPath.empty())
        imwrite(outputPath, sharpened);

	//图像黑白二值化，效果不好
	Mat gray;
	cvtColor(sharpened, gray, CV_BGR2GRAY);
	/* showMultipleImages("src/smooth/res", 2, original, sharpened); */
	//adaptiveThreshold(gray, gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
	//cv::threshold(gray, gray, 100, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
}

// callback function for erode/dilate trackbar
static void ErodeDilate(int, void*)
{
	int n = erode_dilate_pos - max_iters;
	int an = n > 0 ? n : -n;
	Mat element = getStructuringElement(element_shape, Size(an * 2 + 1, an * 2 + 1), Point(an, an));
	if (n < 0)
		erode(src, dst, element);
		//morphologyEx(src, dst, MORPH_OPEN, element);
	else
		dilate(src, dst, element);
		//morphologyEx(src, dst, MORPH_CLOSE, element);
	imshow("Erode/Dilate", dst);
	Mat dis = n > 0 ? src / dst * 255: dst/src * 255;
	imshow("dis_Erode", dis);
}
