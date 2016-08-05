#ifndef GUIDED_FILTER_H
#define GUIDED_FILTER_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

class GuidedFilterImpl;

class GuidedFilter
{
public:
    GuidedFilter(const cv::Mat &I, int r, double eps);
    ~GuidedFilter();

    cv::Mat filter(const cv::Mat &p, int depth = -1) const;

private:
    GuidedFilterImpl *impl_;
};

cv::Mat guidedFilter(const cv::Mat &I, const cv::Mat &p, int r, double eps, int depth = -1);

#endif
