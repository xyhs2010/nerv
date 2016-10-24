// QuadranleLocation.h
// 探测文档，返回文档在原图中的四个点坐标  
// by xiaoyixuan 16-08-31
// bit_xyx@163.com
#ifndef __QUADRANGLE_LOCATION_H__
#define __QUADRANGLE_LOCATION_H__

#import "opencv2/opencv.hpp"
#include <cstring>
#include <vector>
#include <cmath>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define sq(a) ((a)*(a))

struct xcvSegment {
    cv::Point2i begin, end;  // begin end
    int dist;  // 此处为距离平方，因为double开方会有精度损失，且运算较慢
    double tanv; // tangle dy/dx;
    
    void set(const double* pvertexs) {
        begin.x = round(pvertexs[0]);
        begin.y = round(pvertexs[1]);
        end.x = round(pvertexs[2]);
        end.y = round(pvertexs[3]);
        dist = sq(begin.x-end.x) + sq(begin.y - end.y);
        tanv = ((double) (end.y - begin.y)) / (end.x - begin.x);
    }
    
    void set(cv::Point2i& bg, cv::Point2i& ed) {
        begin.x = bg.x;
        begin.y = bg.y;
        end.x = ed.x;
        end.y = ed.y;
        dist = sq(begin.x-end.x) + sq(begin.y - end.y);
        tanv = ((double) (end.y - begin.y)) / (end.x - begin.x);
    }
    
    bool operator <(const xcvSegment& nxt) const {
        return dist > nxt.dist;
    }
};

struct CandidateRect {
    cv::Point2i vertexes[4];
    double score;
    
    bool operator< (const CandidateRect &rect)  const
    {
        return score < rect.score;
    }
    
    bool operator> (const CandidateRect &rect)  const
    {
        return score > rect.score;
    }
    
};
// 接收一张opencv2::Mat表示的图，返回图中可能存在的文档的四个点坐标，存放在vertexs数组中
// 参数注意; 用户应确保srcimg为图片，且为vertexs分配内存并清空
int findRect(cv::Mat& srcimg, cv::Point2i* bestVertexs,
             std::vector<CandidateRect> & candidateVertexes);

#endif ///END
