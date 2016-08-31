/*
 * imageEnhance.cpp
 *
 *  Created on: 2016-8-24
 *      Author: xyhs
 */

#include "imageEnhance.h"
#include <time.h>

using namespace std;
using namespace cv;

float colorMap[256];

float winFunc(float x);

void calcenhance(int, int);

cv::Mat imageEnhance(cv::Mat input) {
	int channels = input.channels();
    int maxL = input.rows > input.cols ? input.rows : input.cols;
	int sigma = 3;

	clock_t startT, endT; double cost_time;
    startT = clock();

//    图像滤波, filter
    int r = maxL / 300;
    if (r < 2) {
		r = 2;
	}
    Mat fil_F;
    if (channels == 3) {
	    input.convertTo(fil_F, CV_32FC3);
	} else {
	    input.convertTo(fil_F, CV_32FC1);
	}
	Mat fil_Squ, fil_Blur;
	boxFilter(fil_F.mul(fil_F), fil_Squ, fil_F.depth(), Size(r, r));
	boxFilter(fil_F, fil_Blur, fil_F.depth(), Size(r, r));
	Mat fil_Var = fil_Squ - fil_Blur.mul(fil_Blur);
	Scalar fil_mean = mean(fil_Var) * 1.1;
	fil_Var = fil_Var / (fil_Var + fil_mean);
	Scalar fil_one;
	if (channels == 3) {
		fil_one = Scalar(1, 1, 1);
	} else {
		fil_one = Scalar(1);
	}
	Mat fil_Result = fil_Var.mul(fil_F) + (fil_one - fil_Var).mul(fil_Blur);
	if (channels == 3) {
		fil_Result.convertTo(fil_Result, CV_8UC3);
	} else {
		fil_Result.convertTo(fil_Result, CV_8UC1);
	}

	endT = clock();
	cost_time=((double)(endT - startT))/CLOCKS_PER_SEC;
	startT = endT;
	printf("filter: %f\n", cost_time);

//	去除背景 remove background
	double rb_rate = 300.0 / maxL;
	Mat rb_Small;
	resize(fil_Result, rb_Small, Size(fil_Result.cols * rb_rate, fil_Result.rows * rb_rate));

	int rb_r = 7;
 	Mat element = getStructuringElement(MORPH_RECT, Size(rb_r * 2 + 1, rb_r * 2 + 1), Point(rb_r, rb_r));
 	Mat rb_Blur;
 	morphologyEx(rb_Small, rb_Blur, MORPH_CLOSE, element);
 	GaussianBlur(rb_Blur, rb_Blur, Size(), sigma, sigma);
 	resize(rb_Blur, rb_Blur, Size(fil_Result.cols, fil_Result.rows));
 	Mat rb_Result = fil_Result / rb_Blur * 255;

	endT = clock();
	cost_time=((double)(endT - startT))/CLOCKS_PER_SEC;
	startT = endT;
	printf("remove background: %f\n", cost_time);

// 调色 color
 	calcenhance(0, 246);

    Mat *cl_Gray;
    Mat cl_Hsv, cl_Result;
    vector<Mat> cl_planes;
    if (channels == 3) {
	    cvtColor(rb_Result, cl_Hsv, CV_BGR2HSV);
	    split(cl_Hsv, cl_planes);
	    cl_Gray = &cl_planes[2];
    } else {
    	cl_Gray = &rb_Result;
    }
    MatIterator_<uchar> it, end;
    for (it=cl_Gray->begin<uchar>(), end=cl_Gray->end<uchar>(); it != end; ++it)
        *it = saturate_cast<uchar>(colorMap[*it] * 128 + 128);
    if (channels == 3) {
    	merge(cl_planes, cl_Hsv);
    	cvtColor(cl_Hsv, cl_Result, CV_HSV2BGR);
    } else {
    	cl_Result = *cl_Gray;
    }

	endT = clock();
	cost_time=((double)(endT - startT))/CLOCKS_PER_SEC;
	startT = endT;
	printf("color: %f\n", cost_time);

//    图像锐化 sharpen
    Mat shp_Result;
    int shp_r = maxL / 350;
    if (shp_r > 1) {
		Mat shp_Blur;
		GaussianBlur(cl_Result, shp_Blur, Size(2 * shp_r - 1, 2 * shp_r - 1), sigma, sigma);
		double shp_amount = 1.5;
		int shp_thres = 4;
		Mat shp_Mask = abs(cl_Result - shp_Blur) < shp_thres;
		shp_Result = cl_Result * (1 + shp_amount) + shp_Blur * (-shp_amount);
		cl_Result.copyTo(shp_Result, shp_Mask);
	} else {
		shp_Result = cl_Result.clone();
	}

	endT = clock();
	cost_time=((double)(endT - startT))/CLOCKS_PER_SEC;
	startT = endT;
	printf("sharpen: %f\n", cost_time);

    return shp_Result;
}


float winFunc(float x) {
#ifdef DEBUG
    if (x < -1 || x > 1) {
        exit(1);
    }
#endif
    return (x + 1) * (x + 1) / 2.0 - 1.0;
}

void calcenhance(int Left, int Right) {
    float res;
    for (int i = 0; i < 256; i++) {
        if (i <= Left) {
            res = -1;
        } else if ( i >= Right) {
            res = 1;
        } else {
            res = winFunc(2 * (i - Left) / ((float)(Right - Left)) - 1);
        }
        colorMap[i] = res;
    }
}

