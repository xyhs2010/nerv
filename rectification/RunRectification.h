//
//  RunRectification.h
//  WhiteboardNew
//    对矫正算法进行封装
//  Created by Meng Zhang, netease youdao on 11-3-28.
//  Copyright 2011 beijing. All rights reserved.
//

#include <opencv2/core/core.hpp>
#include <string>

#include "Rectification.h"
#include "QuadrangleLocation.h"

/*对图像进行矫正*/
//+ (UIImage *) rectifyImage: (UIImage *)image withUpperLeft:(CGPoint)ptul UpperRight:(CGPoint)ptur BottomLeft:(CGPoint)ptbl BottomRight:(CGPoint)ptbr;
//+ (void) DoQuadrangleLocationWithUIImage:(UIImage *)image getVertexs:(CvPoint *)vertexs;
void rectifyImage(IplImage * image, IplImage * destImage, CvPoint ptul,
		CvPoint ptur, CvPoint ptbl, CvPoint ptbr, CvSize cvszSize);
CvSize getDestImgSize(IplImage * image, CvPoint ptul, CvPoint ptur,
		CvPoint ptbl, CvPoint ptbr);
void sortPoints(CvPoint imageCornerPoints[]);

