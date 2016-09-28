#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "accelerate.h"
#include "calcproj.h"
#include <math.h>


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
	char text[30];

	Acblock *pblock;

	blocksFilter(&blockarray);

	// page segment
	int segments[MAX_PAGE][2];
	int segnum = blocksSeg(&blockarray, segments);

	double coefs[4 * FIT_ORDER * FIT_ORDER] = {0};
	double singlep[4 * FIT_ORDER * FIT_ORDER] = {0};
	double zs[2 * FIT_ORDER] = {0};
	double singlez[2 * FIT_ORDER] = {0};
	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		pblock = blockarray.blocks + i;
		if (!pblock->useful)
			continue;
		double x, y, z;
		z = pblock->maxAngle;
		if (blockarray.h_major) {
			x = (pblock->centerc * 2.0 - pblock->mat->cols) / pblock->mat->cols;
			y = (pblock->centerr * 2.0 - pblock->mat->rows) / pblock->mat->rows;
		} else {
			y = (pblock->centerc * 2.0 - pblock->mat->cols) / pblock->mat->cols;
			x = (pblock->centerr * 2.0 - pblock->mat->rows) / pblock->mat->rows;
		}
		double value = 1;
		for (int j = 0 ;j < FIT_ORDER; j++) {
			singlep[j] = value;
			value *= x;
		}
		double *pnow = singlep;
		// 第一列
		cblas_dcopy(FIT_ORDER, pnow, 1, pnow + FIT_ORDER, 1);
		cblas_dscal(FIT_ORDER, y, pnow + FIT_ORDER, 1);

		// singlez
		cblas_dcopy(FIT_ORDER * 2, pnow, 1, singlez, 1);
		cblas_dscal(FIT_ORDER * 2, z, singlez, 1);
		cblas_daxpy(FIT_ORDER * 2, 1, singlez, 1, zs, 1);

		// 前 FIT_ORDER 列
		for (int j = 1; j < FIT_ORDER; j++) {
			cblas_dcopy(FIT_ORDER * 2, pnow, 1,
					pnow + 2 * FIT_ORDER, 1);
			cblas_dscal(FIT_ORDER * 2, x, pnow + 2 * FIT_ORDER, 1);
			pnow += 2 * FIT_ORDER;
		}

		// 所有
		pnow += 2 * FIT_ORDER;
		cblas_dcopy(FIT_ORDER * FIT_ORDER * 2, singlep, 1, pnow, 1);
		cblas_dscal(FIT_ORDER * FIT_ORDER * 2, y, pnow, 1);

		cblas_daxpy(FIT_ORDER * FIT_ORDER * 4, 1, singlep, 1, coefs, 1);

//		printf("x: %f, y: %f\n",x,y);

	}
//	for (int j = 0; j < 8; j++) {
//		printf("%f, ", zs[j]);
//	}
//	printf("\n");
	int ipiv[2 * FIT_ORDER];
	LAPACKE_dgesv(LAPACK_COL_MAJOR, 2 * FIT_ORDER, 1, coefs, 2 * FIT_ORDER, ipiv, zs, 2 * FIT_ORDER);

//	for (int j = 0; j < 8; j++) {
//		printf("%f, ", zs[j]);
//	}
//	printf("\n");


	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		pblock = blockarray.blocks + i;
		if (!pblock->useful) {
			continue;
		}

		double x, y;
		double angle = pblock->maxAngle;
		if (blockarray.h_major) {
			angle -= M_PI / 2;
		}
		x = 10 * cos(angle); y = 10 * sin(angle);
		Point p1(pblock->centerc - x, pblock->centerr - y), p2(pblock->centerc + x, pblock->centerr + y);
		line(src, p1, p2, Scalar(0, 0, 255), 1);

		sprintf(text, "(%.3f, %.0f)", pblock->maxAngle, pblock->minWeight);
		putText(src, text, Point(pblock->centerc - 20, pblock->centerr + 20), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 0, 255));
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
