//
//  Rectification.h
//  WhiteboardNew
//    图像矫正的算法实现
//  Created by netease youdao on 11-3-28.
//  Copyright 2011 beijing. All rights reserved.
//

#include "Rectification.h"
using namespace std;
double maxDouble(double dx1, double dx2)
{
	return dx1 > dx2? dx1 : dx2;
}

double xcvRecoveryAspectRatio(CvPoint cvptULCorner, CvPoint cvptURCorner, CvPoint cvptBLCorner, CvPoint cvptBRCorner, 
							  CvPoint cvptPrincipalPoint, double dPixelAspectRatio, double dFocalLength, double dRectangleAspectRatio)
{
	CvMat *vecUpperLeft, *vecUpperRight, *vecBottomLeft, *vecBottomRight;///The homogeneous coordinates of four rectangle corners in the image.
	CvMat *vecN2, *vecN3, *vectmp3by1, *vectmp1by3, *vectmp1by1;
	CvMat *matIntrinsicMat, *matTranspose, *matInverse;
	double dK2, dK3, dtmp;
	double n21, n22, n23, n31, n32, n33;

	vecUpperLeft = cvCreateMat( 3, 1, CV_32FC1 ); 
	vecUpperRight = cvCreateMat( 3, 1, CV_32FC1 );
	vecBottomLeft = cvCreateMat( 3, 1, CV_32FC1 );
	vecBottomRight = cvCreateMat( 3, 1, CV_32FC1 );

	cvmSet( vecUpperLeft, 0, 0, cvptULCorner.x );
	cvmSet( vecUpperLeft, 1, 0, cvptULCorner.y );
	cvmSet( vecUpperLeft, 2, 0, 1.0) ;

	cvmSet( vecUpperRight, 0, 0, cvptURCorner.x );
	cvmSet( vecUpperRight, 1, 0, cvptURCorner.y );
	cvmSet( vecUpperRight, 2, 0, 1.0);

	cvmSet( vecBottomLeft, 0, 0, cvptBLCorner.x );
	cvmSet( vecBottomLeft, 1, 0, cvptBLCorner.y );
	cvmSet( vecBottomLeft, 2, 0, 1.0);

	cvmSet( vecBottomRight, 0, 0, cvptBRCorner.x );
	cvmSet( vecBottomRight, 1, 0, cvptBRCorner.y );
	cvmSet( vecBottomRight, 2, 0, 1.0 );


	vectmp3by1 = cvCreateMat( 3, 1, CV_32FC1 );
	cvCrossProduct( vecBottomLeft, vecUpperRight, vectmp3by1);
	dtmp = cvDotProduct( vectmp3by1, vecBottomRight );
	dK3 = dtmp;
	//printf("dk3_up: %f\n", dK3);
	cvZero( vectmp3by1 );
	cvCrossProduct( vecUpperLeft, vecUpperRight, vectmp3by1);
	dtmp = cvDotProduct( vectmp3by1, vecBottomRight );
	//assert( 0 != dtmp );
	if (0 == dtmp ) {
		return -1;
	}
	//printf("dk3_down: %f\n", dtmp);
	dK3 /= dtmp;
	//printf("dk3: %f\n", dK3);

	cvZero( vectmp3by1 );
	cvCrossProduct( vecBottomLeft, vecUpperRight, vectmp3by1);
	dtmp = cvDotProduct( vectmp3by1, vecUpperLeft );    
	dK2 = dtmp;
	//printf("dk2_up: %f\n", dtmp);
	cvZero( vectmp3by1 );
	cvCrossProduct( vecBottomRight, vecUpperRight, vectmp3by1);
	dtmp = cvDotProduct( vectmp3by1, vecUpperLeft );
	//assert( 0 != dtmp );
	if (0 == dtmp) {
		return -1;
	}
	//printf("dk2_down: %f\n", dtmp);
	dK2 /= dtmp;
	//printf("dk2: %f\n", dK2);

	vecN2 = cvCreateMat( 3, 1, CV_32FC1 );
	vecN3 = cvCreateMat( 3, 1, CV_32FC1 );

	cvZero( vectmp3by1 );
	cvScale( vecBottomRight, vectmp3by1, dK2, 0);
	cvSub( vectmp3by1, vecBottomLeft, vecN2, NULL);

	cvZero( vectmp3by1 );
	cvScale( vecUpperLeft, vectmp3by1, dK3, 0);
	cvSub( vectmp3by1, vecBottomLeft, vecN3, NULL );

	n21 = cvmGet( vecN2, 0, 0);
	n22 = cvmGet( vecN2, 1, 0);
	n23 = cvmGet( vecN2, 2, 0);
	n31 = cvmGet( vecN3, 0, 0);
	n32 = cvmGet( vecN3, 1, 0);
	n33 = cvmGet( vecN3, 2, 0);

	if( dK2 == 1 || dK3 == 1)
		return -1; 
	double t1 = (n21*n31 - (n21*n33 + n23*n31)*cvptPrincipalPoint.x + n23*n33*cvptPrincipalPoint.x*cvptPrincipalPoint.x) * dPixelAspectRatio * dPixelAspectRatio;
	double t2 = (n22*n32 - (n22*n33 + n23*n32)*cvptPrincipalPoint.y + n23*n33*cvptPrincipalPoint.y*cvptPrincipalPoint.y);

	dFocalLength = -1.0 *  (t1 + t2) * 1 /(n23*n33*dPixelAspectRatio*dPixelAspectRatio);

	//cout<<dFocalLength<<endl;
	dFocalLength = sqrt( abs( dFocalLength ) );
	

	matIntrinsicMat = cvCreateMat( 3, 3, CV_32FC1 );
	cvZero( matIntrinsicMat );
	cvmSet( matIntrinsicMat, 0, 0,  dFocalLength );
	cvmSet( matIntrinsicMat, 1, 1,  dFocalLength*dPixelAspectRatio );
	cvmSet( matIntrinsicMat, 2, 2,  1.0 );
	cvmSet( matIntrinsicMat, 0, 2,  cvptPrincipalPoint.x );
	cvmSet( matIntrinsicMat, 1, 2,  cvptPrincipalPoint.y );

	matTranspose = cvCreateMat( 3, 3, CV_32FC1);
	matInverse = cvCreateMat( 3, 3, CV_32FC1);
	dtmp = cvInvert( matIntrinsicMat, matInverse, CV_SVD );

	if( dtmp == 0 )
		return -2;

	cvTranspose( matInverse, matTranspose );
	vectmp1by1 = cvCreateMat( 1, 1, CV_32FC1 );
	vectmp1by3 = cvCreateMat( 1, 3, CV_32FC1 );

	cvTranspose( vecN2, vectmp1by3 );
	cvMatMul( vectmp1by3, matTranspose, vectmp1by3 );
	cvMatMul( vectmp1by3, matInverse, vectmp1by3 );
	cvMatMul( vectmp1by3, vecN2, vectmp1by1 );
	dRectangleAspectRatio = cvmGet( vectmp1by1, 0, 0 );

	cvZero( vectmp1by3 );
	cvZero( vectmp1by1 );
	cvTranspose( vecN3, vectmp1by3);
	cvMatMul( vectmp1by3, matTranspose, vectmp1by3 );
	cvMatMul( vectmp1by3, matInverse, vectmp1by3 );
	cvMatMul( vectmp1by3, vecN3, vectmp1by1);
	dRectangleAspectRatio /= cvmGet( vectmp1by1, 0, 0 );

	dRectangleAspectRatio = sqrt( dRectangleAspectRatio );

	cvReleaseMat( &vecUpperLeft );
	cvReleaseMat( &vecUpperRight );
	cvReleaseMat( &vecBottomLeft );
	cvReleaseMat( &vecBottomRight );

	cvReleaseMat( &vecN2 );
	cvReleaseMat( &vecN3 );
	cvReleaseMat( &vectmp3by1 );
	cvReleaseMat( &vectmp1by3 );
	cvReleaseMat( &matIntrinsicMat );
	cvReleaseMat( &matTranspose );
	cvReleaseMat( &matInverse );
	cvReleaseMat( &vectmp1by1 );

	return dRectangleAspectRatio;
}


void xcvFindRectangleCorners(CvPoint cvptULCorner, 
							 CvPoint cvptURCorner, 
							 CvPoint cvptBLCorner, 
							 CvPoint cvptBRCorner, 
							 double dAspectRatio,
							 CvSize* cvszSize)
{
	double dw1,dw2,dh1,dh2,r,w,h;
	dw1 = sqrt( (double)(cvptURCorner.x-cvptULCorner.x)*(cvptURCorner.x-cvptULCorner.x) + (cvptURCorner.y-cvptULCorner.y)*(cvptURCorner.y-cvptULCorner.y) );
	dw2 = sqrt( (double)(cvptBRCorner.x-cvptBLCorner.x)*(cvptBRCorner.x-cvptBLCorner.x) + (cvptBRCorner.y-cvptBLCorner.y)*(cvptBRCorner.y-cvptBLCorner.y) );
	dh1 = sqrt( (double)(cvptBLCorner.x-cvptULCorner.x)*(cvptBLCorner.x-cvptULCorner.x) + (cvptBLCorner.y-cvptULCorner.y)*(cvptBLCorner.y-cvptULCorner.y) );
	dh2 = sqrt( (double)(cvptBRCorner.x-cvptURCorner.x)*(cvptBRCorner.x-cvptURCorner.x) + (cvptBRCorner.y-cvptURCorner.y)*(cvptBRCorner.y-cvptURCorner.y) );

	r = maxDouble( dw1, dw2 ) / maxDouble( dh1, dh2);

	if( r >= dAspectRatio )
	{
		w = maxDouble( dw1, dw2 );
		h = w / dAspectRatio;

		if (h > maxDouble(dh1, dh2)) {
			h = maxDouble(dh1, dh2);
			w = h * dAspectRatio;
		}

	}
	else
	{
		h = maxDouble( dh1, dh2 );
		w = dAspectRatio * h;

		if (w > maxDouble(dw1, dw2)) {
			w = maxDouble(dw1, dw2);
			h = w / dAspectRatio;
		}
	}

	*cvszSize = cvSize(w,h);
}

void xcvFindHomographyMatrix(CvPoint cvptULCorner, 
							 CvPoint cvptURCorner, 
							 CvPoint cvptBLCorner, 
							 CvPoint cvptBRCorner, 
							 CvSize cvszSize,
							 CvMat* cvpmatHomography)
{
	CvMat* cvpmatSrcPoints, *cvpmatDstPoints;
	cvpmatSrcPoints = cvCreateMat(4, 2, CV_32FC1);
	cvpmatDstPoints = cvCreateMat(4, 2, CV_32FC1);

	cvmSet(cvpmatSrcPoints, 0, 0, cvptULCorner.x);
	cvmSet(cvpmatSrcPoints, 0, 1, cvptULCorner.y);
	cvmSet(cvpmatSrcPoints, 1, 0, cvptURCorner.x);
	cvmSet(cvpmatSrcPoints, 1, 1, cvptURCorner.y);
	cvmSet(cvpmatSrcPoints, 2, 0, cvptBLCorner.x);
	cvmSet(cvpmatSrcPoints, 2, 1, cvptBLCorner.y);
	cvmSet(cvpmatSrcPoints, 3, 0, cvptBRCorner.x);
	cvmSet(cvpmatSrcPoints, 3, 1, cvptBRCorner.y);
	cvmSet(cvpmatDstPoints, 0, 0, 0);
	cvmSet(cvpmatDstPoints, 0, 1, 0);
	cvmSet(cvpmatDstPoints, 1, 0, cvszSize.width);
	cvmSet(cvpmatDstPoints, 1, 1, 0);
	cvmSet(cvpmatDstPoints, 2, 0, 0);
	cvmSet(cvpmatDstPoints, 2, 1, cvszSize.height);
	cvmSet(cvpmatDstPoints, 3, 0, cvszSize.width);
	cvmSet(cvpmatDstPoints, 3, 1, cvszSize.height);

	cvFindHomography( cvpmatSrcPoints, cvpmatDstPoints, cvpmatHomography, 0, 0, 0);
	cvReleaseMat( &cvpmatSrcPoints );
	cvReleaseMat( &cvpmatDstPoints );
}

void xcvHomographyTransform(const IplImage* cvpimgSrcImage, 
							IplImage* cvpDstImage,
							CvMat* cvpmatHomography)
{
	cvWarpPerspective( cvpimgSrcImage, cvpDstImage, cvpmatHomography, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(255));
}
