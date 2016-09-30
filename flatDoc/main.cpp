#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "accelerate.h"
#include "calcproj.h"
#include <math.h>

#define MAX_L 500

using namespace std;
using namespace cv;

static void help(cv::CommandLineParser *);

// 从 opencv 类型转为 double 数组
int convertFromMat(Mat src, Acmat *srcmat);

Mat convertToMat(Acmat *srcmat);

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
	if (maxL > MAX_L) {
		double rate = MAX_L / maxL;
		resize(src, src, Size(src.cols * rate, src.rows * rate));
	}
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);

	src = src * 0.5 + 128;
	Acmat srcmat;
	if (convertFromMat(gray, &srcmat) < 0) {
		return -1;
	}
	Acblockarray blockarray = createBlocks(&srcmat);
	char text[30];

	Acblock *pblock;

	blocksFilter(&blockarray);

	// page segment
	int segments[MAX_PAGE][2];
	int segnum = blocksSeg(&blockarray, segments);

	double zs[2 * FIT_ORDER] = {0};
	polyfit(&blockarray, zs);

//	for (int j = 0; j < 2 * FIT_ORDER; j++) {
//		printf("%.2f\t", zs[j]);
//	}
//	printf("\n");

	Acmat *pdesmat = acmatLikeMat(&srcmat);
	rectMat(&blockarray, pdesmat, zs);

	Mat newmat = convertToMat(pdesmat);
	imshow("des", newmat);
	destroyMat(pdesmat);
	free(pdesmat);

	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		pblock = blockarray.blocks + i;
		if (!pblock->useful) {
			continue;
		}

		double x, y;
		double angle = pblock->maxAngle;
		if (!blockarray.h_major) {
			angle = M_PI / 2 - angle;
		}
		x = 10 * cos(angle); y = 10 * sin(angle);
		Point p1(pblock->centerc - x, pblock->centerr - y), p2(pblock->centerc + x, pblock->centerr + y);
		line(src, p1, p2, Scalar(0, 0, 255), 1);

		sprintf(text, "(%.2f, %.0f)", pblock->maxAngle, pblock->maxWeight);
		putText(src, text, Point(pblock->centerc - 10, pblock->centerr + 10), FONT_HERSHEY_SIMPLEX, 0.2, Scalar(0, 0, 255));
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

int convertFromMat(Mat src, Acmat *srcmat) {
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

Mat convertToMat(Acmat *srcmat) {
	Acmat *tmpmat;
	if (srcmat->col_major) {
		tmpmat = changeMajor(srcmat);
	} else {
		tmpmat = srcmat;
	}
	Mat newmat(tmpmat->rows, tmpmat->cols, CV_64FC1, tmpmat->data);
	newmat.convertTo(newmat, CV_8UC1);
	if (tmpmat != srcmat) {
		destroyMat(tmpmat);
		free(tmpmat);
	}
	return newmat;
}
