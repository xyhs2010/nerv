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
	int segL = blockarray.h_major ? blockarray.cols : blockarray.rows;
	double *sum = (double *)malloc(segL * sizeof(double));
	int *num = (int *)malloc(segL * sizeof(int));
	for (int i = 0; i < segL; i++) {
		sum[i] = 0;
		num[i] = 0;
	}
	int ir, ic;
	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		pblock = blockarray.blocks + i;
		if (blockarray.h_major) {
			pblock->maxAngle += M_PI / 2;
			if (pblock->maxAngle > M_PI) {
				pblock->maxAngle -= M_PI;
			}
		}
		if (pblock->useful) {
			if (blockarray.col_major) {
				ic = i / blockarray.rows;
				ir = i % blockarray.rows;
			} else {
				ic = i % blockarray.cols;
				ir = i / blockarray.cols;
			}
			if (blockarray.h_major) {
				sum[ic] += pblock->maxAngle;
				num[ic] += 1;
			} else {
				sum[ir] += pblock->maxAngle;
				num[ir] += 1;
			}
		}
	}
	int segments[MAX_PAGE][2];
	int segnum = 0, lastb, j; double diff;

//	for (int i = 0; i < segL; i++) {
//		if (num[i] > 0)
//			sum[i] /= num[i];
//		printf("%d  sum: %f, num: %d\n ", i, sum[i], num[i]);
//	}

	int blockwid = blockarray.blocks[0].endc - blockarray.blocks[0].startc;
	int lastSeg = 1;
	for (int i = 2; i < segL - 1; i++) {
		if (num[i] >= 3 && num[i + 1] >=3) {
			j = i;
			lastb = i;
			while (--j > lastSeg) {
				if (num[j] > 3 && num[j - 1] > 3) {
					lastb = j;
					break;
				}
			}
			if (lastb != i) {
				diff = sum[i] - sum[lastb];
				if (abs(diff) > M_PI/6) {
					if (diff * (sum[i+1] - sum[i]) < 0 && diff * (sum[lastb] - sum[lastb-1]) < 0) {
						segments[segnum][0] = (lastb + 0.5) * blockwid;
						segments[segnum][1] = (i + 0.5) * blockwid;
						segnum++;
						lastSeg = i;
					}
				}
			}
		}
	}

	free(sum);
	free(num);

	for (int i = 0; i < blockarray.cols * blockarray.rows; i++) {
		pblock = blockarray.blocks + i;
		if (!pblock->useful) {
			continue;
		}

		double x, y;
		x = 10 * cos(pblock->maxAngle); y = 10 * sin(pblock->maxAngle);
		Point p1(pblock->centerc - x, pblock->centerr - y), p2(pblock->centerc + x, pblock->centerr + y);
		line(src, p1, p2, Scalar(0, 0, 255), 1);

		sprintf(text, "(%.0f, %.0f)", pblock->maxWeight, pblock->minWeight);
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
