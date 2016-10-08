/*
 * imageEnhance.h
 *
 *  Created on: 2016-8-24
 *      Author: xyhs
 */

#ifndef IMAGEENHANCE_H_
#define IMAGEENHANCE_H_


#include <opencv2/core.hpp>


#define FILTER_TYPE_AUTO 0
#define FILTER_TYPE_ENHANCE 1
#define FILTER_TYPE_GRAY 2

cv::Mat imageEnhance(cv::Mat input);

cv::Mat imageEnhance(cv::Mat input, int filter_type);

#endif /* IMAGEENHANCE_H_ */
