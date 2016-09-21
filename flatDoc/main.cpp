#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "accelerate.h"
#include "calcproj.h"
#include <math.h>

#define ANGLE_NUM (16)

using namespace std;
using namespace cv;

static void help(cv::CommandLineParser *);

// 从 opencv 类型转为 double 数组
int convertMat(Mat src, Acmat *srcmat);

int main(int argc, char** argv)
{
	cv::CommandLineParser parser(argc, argv, "{h help usage ?| |print this message}{i interact| |interaction mode}{ @inputImage| |image to process}{@outputImage| |image to be saved}");
	if (parser.has("h"))
	{
		help(&parser);
		return 0;
	}
	std::string filename = parser.get<std::string>("@inputImage");
    string outputPath = parser.get<std::string>("@outputImage");
    Mat src;
	if ((src = imread(filename, 1)).empty())
	{
		help(&parser);
		return -1;
	}
	if (!parser.has("i")) {
    }

	double maxL = src.cols > src.rows ? src.cols : src.rows;
	if (maxL > 1080) {
		double rate = 1080 / maxL;
		resize(src, src, Size(src.cols * rate, src.rows * rate));
	}
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	src = src * 0.5 + 128;
	Acmat srcmat;
	if (convertMat(gray, &srcmat) < 0) {
		return -1;
	}
	Acblockarray blockarray = createBlocks(&srcmat);

	double angles[ANGLE_NUM];
	double stds[ANGLE_NUM];
	for (int i = 0; i < ANGLE_NUM; i++) {
		angles[i] = (M_PI * i) / ANGLE_NUM;
	}

	int mini, maxi;
	double angle;
	char text[30];
	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		Acblock block = blockarray.blocks[i];
		projStdsAtAngles(angles, stds, ANGLE_NUM, &block);
		maxi = acmaxIndex(stds, ANGLE_NUM);
		mini = acminIndex(stds, ANGLE_NUM);
		angle = angles[maxi];

		double x, y;
		x = 10 * cos(angle); y = 10 * sin(angle);
		Point p1(block.centerc - x, block.centerr - y), p2(block.centerc + x, block.centerr + y);
		line(src, p1, p2, Scalar(0, 0, 255), 1);

		sprintf(text, "(%.0f, %.0f)", stds[maxi], stds[mini]);
		putText(src, text, Point(block.centerc - 20, block.centerr + 20), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 0, 255));
	}

	destroyBlockArray(&blockarray);
	destroyMat(&srcmat);

	imshow("Original", src);

	for (;;)
	{
		int c;
		c = waitKey(0);

		if ((char)c == 'q')
			return 0;
		else
			continue;
	}
	return 0;
}

static void help(cv::CommandLineParser *parser)
{

	printf("\nFlatten curved document image\n");
    parser->printMessage();
}

int convertMat(Mat src, Acmat *srcmat) {
	if (src.type() != CV_8UC1) {
		return -1;
	}
	srcmat->rows = src.rows;
	srcmat->cols = src.cols;
	srcmat->col_major = true;
	srcmat->data = (double *)malloc(src.rows * src.cols * sizeof(double));
	uchar *p;
	for (int i = 0; i < src.rows; ++i) {
		p = src.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j) {
			srcmat->data[j * src.rows + i] = p[j];
		}
	}
	return 0;
}
