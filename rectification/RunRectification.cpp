//
//  RunRectification.m
//  Whiteboard
//
//  Created by netease youdao on 11-3-23.
//  Copyright 2011 beijing. All rights reserved.
//
//  Version 1.1 
//  xcvSimpleRecovery()
//  by xiaoyixuan on 16-8-31
//  bit_xyx@163.com

#include "RunRectification.h"
using namespace std;
using namespace cv;

#define TINY 1e-7

double crossMultVector(CvPoint a, CvPoint b) {
	return a.x * b.y - b.x * a.y;
}

/* 向量叉乘 */
float crossMulVect(CvPoint2D32f v1, CvPoint2D32f v2) {
	return v1.x * v2.y - v2.x * v1.y;
}
/* 向量点乘 */
float dotMulVect(CvPoint2D32f v1, CvPoint2D32f v2) {
	return v1.x * v2.x - v1.y * v2.y;
}
/* 向量长度 */
float vectLength(CvPoint2D32f vect) {
	return sqrt(vect.x * vect.x + vect.y * vect.y);
}

/* 以四个角点的重心为中心，逆时针顺序重新排列角点顺序 */
void sortPoints(CvPoint imageCornerPoints[]) {
	CvPoint center = cvPoint(0, 0);
	for (int i = 0; i < 4; i++) {
		center.x += imageCornerPoints[i].x;
		center.y += imageCornerPoints[i].y;
	}
	center.x /= 4;
	center.y /= 4;

	CvPoint2D32f vecXAxis = cvPoint2D32f(1.0f, 0.0f);
	float angle[4];

	for (int i = 0; i < 4; i++) {
		CvPoint2D32f vect = cvPoint2D32f(imageCornerPoints[i].x - center.x,
				imageCornerPoints[i].y - center.y);
		if (vectLength(vect) != 0.0f)
			angle[i] = acos(dotMulVect(vect, vecXAxis) / vectLength(vect));
		else
			angle[i] = 0.0f;

		if (crossMulVect(vect, vecXAxis) > 0)
			angle[i] = -angle[i];
	}

	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (angle[i] > angle[j]) {
				CvPoint swapPoint = imageCornerPoints[i];
				float swapAngle = angle[i];
				imageCornerPoints[i] = imageCornerPoints[j];
				angle[i] = angle[j];
				imageCornerPoints[j] = swapPoint;
				angle[j] = swapAngle;
			}
		}
	}

	CvPoint swapPoint = imageCornerPoints[2];
	imageCornerPoints[2] = imageCornerPoints[3];
	imageCornerPoints[3] = swapPoint;
}

inline double xcvVectLength(CvPoint& vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline double xcvAdjust(double lena, double lenb) {
	return (lena * lena + lenb * lenb) / (lena * lena);
}

double xcvSimpleRecovery(CvPoint& vectUp, CvPoint& vectDown, CvPoint& vectLeft, CvPoint& vectRight) {
	double top = xcvVectLength(vectUp);
	double bottom = xcvVectLength(vectDown);
	double left = xcvVectLength(vectLeft) ;
	double right = xcvVectLength(vectRight);

	double height = (left + right) * 0.5 * xcvAdjust(top + bottom, fabs(top - bottom));
	double width = (top + bottom) * 0.5 * xcvAdjust(left + right, fabs(left - right));
	return width / height;
}

CvSize _getDestImgSize(int dw, int dh, CvPoint ptul, CvPoint ptur, CvPoint ptbl,
		CvPoint ptbr) {
	CvPoint cvptul, cvptur, cvptbl, cvptbr;
	CvPoint cvptPrincipalPoint;
	double dPixelAspectRatio = 1;
	double dFocalLength = .0;
	double dRectangleAspectRatio = 1.0;

	cvptul.x = ptul.x;
	cvptul.y = ptul.y;
	cvptur.x = ptur.x;
	cvptur.y = ptur.y;
	cvptbl.x = ptbl.x;
	cvptbl.y = ptbl.y;
	cvptbr.x = ptbr.x;
	cvptbr.y = ptbr.y;

	CvPoint vectUp = cvPoint(ptul.x - ptur.x, ptul.y - ptur.y);
	CvPoint vectDown = cvPoint(ptbl.x - ptbr.x, ptbl.y - ptbr.y);
	CvPoint vectLeft = cvPoint(ptul.x - ptbl.x, ptul.y - ptbl.y);
	CvPoint vectRight = cvPoint(ptur.x - ptbr.x, ptur.y - ptbr.y);

	double crossValue_1 = crossMultVector(vectUp, vectDown); //[RunRectification crossMultVector:vectUp withVector:vectDown];
	double crossValue_2 = crossMultVector(vectLeft, vectRight); //[RunRectification crossMultVector:vectLeft withVector:vectRight];

	CvSize cvszSize;

	if (fabs(crossValue_1) < TINY || fabs(crossValue_2) < TINY) {
		printf("Perfect IMG. Do not need rectification\n");

		cvszSize.width = sqrt(
				(float) vectUp.x * vectUp.x + vectUp.y * vectUp.y);
		cvszSize.height = sqrt(
				(float) vectLeft.x * vectLeft.x + vectLeft.y * vectLeft.y);

	} else {
		cvptPrincipalPoint = cvPoint(dw / 2, dh / 2);

		//1
		// version 1.0
/*		double dres = xcvRecoveryAspectRatio(cvptul, cvptur, cvptbl, cvptbr,
				cvptPrincipalPoint, dPixelAspectRatio, dFocalLength,
				dRectangleAspectRatio);*/
		double dres = xcvSimpleRecovery(vectUp, vectDown, vectLeft, vectRight);
		printf("aspect ratio = %f\n", dres);

		if (dres <= 0.0) {
			printf("aspect ratio < 0.0, error!!!\n");
			cvszSize.width = sqrt(
					(float) vectUp.x * vectUp.x + vectUp.y * vectUp.y);
			cvszSize.height = sqrt(
					(float) vectLeft.x * vectLeft.x + vectLeft.y * vectLeft.y);
		} else {
			//2
			xcvFindRectangleCorners(cvptul, cvptur, cvptbl, cvptbr, dres,
					&cvszSize);
		}
	}

	cvszSize.width = cvszSize.width > 1 ? cvszSize.width : 1;
	cvszSize.height = cvszSize.height > 1 ? cvszSize.height : 1;
	return cvszSize;
}

CvSize getDestImgSize(IplImage * image, CvPoint ptul, CvPoint ptur,
		CvPoint ptbl, CvPoint ptbr) {
	return _getDestImgSize(image->width, image->height, ptul, ptur, ptbl, ptbr);
}

//+ (UIImage *) rectifyImage: (UIImage *)image withUpperLeft:(CGPoint)ptul UpperRight:(CGPoint)ptur BottomLeft:(CGPoint)ptbl BottomRight:(CGPoint)ptbr
void rectifyImage(IplImage * image, IplImage * destImage, CvPoint ptul,
		CvPoint ptur, CvPoint ptbl, CvPoint ptbr, CvSize cvszSize) {
	CvPoint cvptul, cvptur, cvptbl, cvptbr;

	cvptul.x = ptul.x;
	cvptul.y = ptul.y;
	cvptur.x = ptur.x;
	cvptur.y = ptur.y;
	cvptbl.x = ptbl.x;
	cvptbl.y = ptbl.y;
	cvptbr.x = ptbr.x;
	cvptbr.y = ptbr.y;

	CvMat* cvmatHomography;
	cvmatHomography = cvCreateMat(3, 3, CV_32FC1);
	xcvFindHomographyMatrix(cvptul, cvptur, cvptbl, cvptbr, cvszSize,
			cvmatHomography);

	xcvHomographyTransform(image, destImage, cvmatHomography);
	cvReleaseMat(&cvmatHomography);
}

IplImage* xcvRotateImage(IplImage* src, int angle) {
	IplImage* dst = NULL;
	int width = abs(
			(double) (src->height * sin(angle * CV_PI / 180.0))
					+ (double) (src->width * cos(angle * CV_PI / 180.0)) + 1);
	int height = abs(
			(double) (src->height * cos(angle * CV_PI / 180.0))
					+ (double) (src->width * sin(angle * CV_PI / 180.0)) + 1);
	int tempLength = sqrt(
			(double) src->width * src->width + src->height * src->height) + 10;
	int tempX = (tempLength + 1) / 2 - src->width / 2;
	int tempY = (tempLength + 1) / 2 - src->height / 2;
	int flag = -1;

	dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);
	cvZero(dst);
	IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), src->depth,
			src->nChannels);
	cvZero(temp);

	cvSetImageROI(temp, cvRect(tempX, tempY, src->width, src->height));
	cvCopy(src, temp, NULL);
	cvResetImageROI(temp);

	float m[6];
	int w = temp->width;
	int h = temp->height;
	m[0] = (float) cos(angle * CV_PI / 180.);
	m[1] = (float) sin(angle * CV_PI / 180.);
	m[3] = -m[1];
	m[4] = m[0];
	// 将旋转中心移至图像中间
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	//
	CvMat M = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(temp, dst, &M);
	cvReleaseImage(&temp);
	return dst;
}

