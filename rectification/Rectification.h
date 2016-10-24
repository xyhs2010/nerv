//
//  Rectification.h
//  WhiteboardNew
//    图像矫正的算法实现
//  Created by netease youdao on 11-3-28.
//  Copyright 2011 beijing. All rights reserved.
//

#import "opencv2/opencv.hpp"
#include <iostream>

//double xcvRecoveryAspectRatio(CvPoint cvptULCorner, 
//                              CvPoint cvptURCorner, 
//                              CvPoint cvptBLCorner, 
//                              CvPoint cvptBRCorner);

//恢复白板宽高比
double xcvRecoveryAspectRatio(CvPoint cvptULCorner, CvPoint cvptURCorner, CvPoint cvptBLCorner, CvPoint cvptBRCorner, 
							  CvPoint cvptPrincipalPoint, double dPixelAspectRatio, double dFocalLength, double dRectangleAspectRatio);

//计算矫正图像中白板四个角点的坐标
void xcvFindRectangleCorners(CvPoint cvptULCorner, 
							 CvPoint cvptURCorner, 
							 CvPoint cvptBLCorner, 
							 CvPoint cvptBRCorner, 
							 double dAspectRatio,
							 CvSize* cvszSize);

//计算单应矩阵
void xcvFindHomographyMatrix(CvPoint cvptULCorner, 
							 CvPoint cvptURCorner, 
							 CvPoint cvptBLCorner, 
							 CvPoint cvptBRCorner, 
							 CvSize cvszSize,
							 CvMat* cvpmatHomography);

//执行射影变换
void xcvHomographyTransform(const IplImage* cvpimgSrcImage, 
							IplImage* cvpDstImage,
							CvMat* cvpmatHomography);
//图像裁剪
void xcvClipImage(const IplImage* cvimgSrcImage, 
				  IplImage* cvpimgDstImage);

//图像防缩
void xcvResizeImage(const IplImage* cvpimgSrcImage, 
					IplImage* cvpimgDstImage);
