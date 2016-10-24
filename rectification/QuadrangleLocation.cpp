//
//  QuadrangleLocation.cpp
//
//  08-08-2016
//  xiaoyixuan
//  bit_xyx@163.com
//

#include "QuadrangleLocation.h"
#include <string>
#include <math.h>
#include <time.h>

extern "C"
{
    #include "lsd.h"
}

using namespace cv;
using namespace std;


#define fabs(x) fabs((x)*1.0f)
#define sqrt(x) sqrt((x)*1.0f)

namespace QuadFinder {
    int row = 200; // after resize
    int col = 300; // after resize
    double scale = 1;
    int debugName = 0;
    Mat displayLineImg;
}
;

// for debug
void PrintPic(string fn, Mat& img) {
    string f = "Debug/" + fn + ".png";
    char fname[128];
    strcpy(fname, f.c_str());
    imwrite(f.c_str(), img);
}

// for debug
//#define PRINT_ALL_IMAGE

Mat xcvPreprocessImage(Mat& srcimg) {
    QuadFinder::row = 200;
    QuadFinder::col = 300;
    int imgh = QuadFinder::row;
    int imgw = QuadFinder::col;
    
    QuadFinder::scale = 1.0;
    if (srcimg.rows > imgh) {
        QuadFinder::scale = (double) srcimg.rows / imgh;
    }
    
    imgw = round(srcimg.cols / QuadFinder::scale);
    
    QuadFinder::row = imgh;
    QuadFinder::col = imgw;
    
    Size newSize = Size(imgw, imgh);
    Mat grayimg;
    if (srcimg.channels() >= 3)
        cvtColor(srcimg, grayimg, CV_BGR2GRAY);
    else
        grayimg = srcimg.clone();
    resize(grayimg, grayimg, newSize);
    
#ifdef PRINT_ALL_IMAGE
    char buffer[128];
    sprintf(buffer, "Src/%d.png", ++QuadFinder::debugName);
    imwrite(buffer, grayimg);
#endif
    return grayimg;
}

/*
 * Name: EdgeDetection Algorithm // group LSG
 * Target: to compute horizon and vertical lsgs of srcimg
 (to be precise) to group line segments detected by LSD Algorithm into
 different LSG
 */
namespace xPI8 {
    const double one = M_PI / 8;
    const double two = 2 * one; // pi*1/4
    const double three = 3 * one;
    const double five = 5 * one;
    const double six = 6 * one; // 3/4 pi
    const double seven = 7 * one;
    const double nine = 9 * one;
    const double eleven = 11 * one;
}
;
// x*pi/8

inline bool isHorizontal(xcvSegment& seg) {
    if (seg.tanv > -xPI8::two && seg.tanv < xPI8::two) {
        return true;
        // -pi/8 < seg's angle < pi/8 or
        // 7pi/8 < seg's angle < 9pi/8
    }
    return false;
}

inline bool isVertical(xcvSegment& seg) {
    if (seg.tanv > -xPI8::two && seg.tanv < xPI8::two) {
        return false;
        // -3pi/8 < seg's angle < 3pi/8
        // 5pi/8 < seg's angle < 11pi/8
    }
    return true;
}

// test 6: filter small frament (len < 1/20)
bool cutBranchLen(xcvSegment* u, int& length) {
    if (u->dist < length * length)
        return true;
    return false;
}

void addImgSegment(vector<xcvSegment>* lsgh, vector<xcvSegment>* lsgv,
                   int& imgh, int& imgw) {
    Point2i bg, ed;
    xcvSegment seg;
    if (lsgh->size() == 0) {
        bg.x = 0;
        bg.y = 0;
        ed.x = imgw - 1;
        ed.y = 0; // upper segment
        seg.set(bg, ed);
        lsgh->push_back(seg);
        
        bg.x = 0;
        bg.y = imgh - 1;
        ed.x = imgw - 1;
        ed.y = imgh - 1; // down segment
        seg.set(bg, ed);
        lsgh->push_back(seg);
    } else if (lsgh->size() == 1) {
        if ((*lsgh)[0].begin.y > 0.5 * QuadFinder::col) {
            bg.x = 0;
            bg.y = 0;
            ed.x = imgw - 1;
            ed.y = 0; // upper segment
            seg.set(bg, ed);
            lsgh->push_back(seg);
        } else {
            bg.x = 0;
            bg.y = imgh - 1;
            ed.x = imgw - 1;
            ed.y = imgh - 1; // down segment
            seg.set(bg, ed);
            lsgh->push_back(seg);
        }
    }
    
    if (lsgv->size() == 0) {
        bg.x = 0;
        bg.y = 0;
        ed.x = 0;
        ed.y = imgh - 1; // left segment
        seg.set(bg, ed);
        lsgv->push_back(seg);
        
        bg.x = imgw - 1;
        bg.y = 0;
        ed.x = imgw - 1;
        ed.y = imgh - 1; // right segment
        seg.set(bg, ed);
        lsgv->push_back(seg);
    } else if (lsgv->size() == 1) {
        if ((*lsgv)[0].begin.x > 0.5 * QuadFinder::row) {
            bg.x = 0;
            bg.y = 0;
            ed.x = 0;
            ed.y = imgh - 1; // left segment
            seg.set(bg, ed);
            lsgv->push_back(seg);
        } else {
            bg.x = imgw - 1;
            bg.y = 0;
            ed.x = imgw - 1;
            ed.y = imgh - 1; // right segment
            seg.set(bg, ed);
            lsgv->push_back(seg);
        }
    }
    
}
// change double* into segment list
void double2Seglist(int& n_segs, double* segs, int& fn_segs, double* fullsegs,
                    vector<xcvSegment>* segh, vector<xcvSegment>* segv, int cutLen) {
    xcvSegment tmp;
    cutLen *= cutLen;
    vector<xcvSegment> tmph, tmpv;
    
    for (int i = 0; i < n_segs; i++) {
        const double *seg = &segs[i * 7];
        if (seg[6] < 2)
            continue;
        tmp.set(seg);
        if (tmp.dist < cutLen)
            continue;
        if (isHorizontal(tmp)) {
            if (tmp.begin.x > tmp.end.x) {
                swap(tmp.begin, tmp.end);
            }
            tmph.push_back(tmp);
        }
        if (isVertical(tmp)) {
            if (tmp.begin.y > tmp.end.y) {
                swap(tmp.begin, tmp.end);
            }
            tmpv.push_back(tmp);
        }
    }
    
    for (int i = 0; i < fn_segs; i++) {
        const double *seg = &fullsegs[i * 7];
        if (seg[6] < 2)
            continue;
        tmp.set(seg);
        if (tmp.dist < cutLen)
            continue;
        if (isHorizontal(tmp)) {
            if (tmp.begin.x > tmp.end.x) {
                swap(tmp.begin, tmp.end);
            }
            tmph.push_back(tmp);
        }
        if (isVertical(tmp)) {
            if (tmp.begin.y > tmp.end.y) {
                swap(tmp.begin, tmp.end);
            }
            tmpv.push_back(tmp);
        }
    }
    sort(tmph.begin(), tmph.end());
    sort(tmpv.begin(), tmpv.end());
    int vcutLength = QuadFinder::row * 0.2;
    int hcutLength = QuadFinder::col * 0.2;
    int hsize = tmph.size();
    int vsize = tmpv.size();
    for (int i = 0; i < 10; i++) {
        if (i < hsize && !cutBranchLen(&tmph[i], hcutLength))
            segh->push_back(tmph[i]);
        if (i < vsize && !cutBranchLen(&tmpv[i], vcutLength))
            segv->push_back(tmpv[i]);
    }
}

inline int xmult(Point o, Point a, Point b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

inline bool isParallel(xcvSegment& u, xcvSegment& v) {
    return (u.end.x - u.begin.x) * (v.end.y - v.begin.y)
    == (u.end.y - u.begin.y) * (v.end.x - v.begin.x);
}

bool isSegOnLine(xcvSegment& line, xcvSegment& seg) {
    if (isParallel(line, seg))
        return false;
    if (xmult(line.begin, line.end, seg.begin)
        * xmult(line.begin, line.end, seg.end) <= 0)
        return true;
    return false;
}

// Main API
void xcvEdgeDetect(Mat& srcimg, vector<xcvSegment>* segh,
                   vector<xcvSegment>* segv) {
    int len = srcimg.cols * srcimg.rows;
    double* image = new double[len]; // need release 1
    if (image == NULL) {
        fprintf(stderr, "error: not enough memory\n");
        exit(-1);
    }
    for (int y = 0; y < srcimg.rows; y++) {
        const uchar* prow = srcimg.ptr<uchar>(y);
        for (int x = 0; x < srcimg.cols; x++) {
            image[y * srcimg.cols + x] = prow[x];
        }
    }
    
    int n_segs = 0;
    int fn_segs = 0;
    double* fullsegs = yd_lsd_scale_region(&fn_segs, image, srcimg.cols,
                                        srcimg.rows, 0.4);
    fn_segs = 0;
    double* segs = yd_lsd_scale_region(&n_segs, image, srcimg.cols, srcimg.rows,
                                    0.0);
    delete image; // release 1
    double2Seglist(n_segs, segs, fn_segs, fullsegs, segh, segv,
                   0.01 * min(srcimg.cols, srcimg.rows));
    //buildLSG(&segList, lsgh, lsgv, srcimg.cols, srcimg.rows);
}
/* end EdgeDetection Algorithm
 */

/*
 * Name: QuadranglesDetection Algorithm
 * Target: to detect all quadrangles based on 2 LSG lists //with bruteforce:(
 */
// cal cross point of 2 lines
// cos(x*pi/8)
namespace COS8 {
    double two = cos(xPI8::two); // cos(pi/4)
    double six = cos(xPI8::six); // cos(3pi/4)
}
;

inline int pointDist(Point2i& a, Point2i& b) {
    return sq(a.x-b.x) + sq(a.y - b.y);
}

inline double pointDist(Point2i* a, Point2i* b) {
    return sq(a->x-b->x) + sq(a->y - b->y);
}

// just for segment's and line formed by intersect_points
inline bool dotOnLine(Point2i& la, Point2i& lb, Point2i& p) {
    // same dot
    if (abs(p.x - la.x) <= 1 && abs(p.y - la.y) <= 1)
        return true;
    if (abs(p.x - lb.x) <= 1 && abs(p.y - lb.y) <= 1)
        return true;
    if ((p.x - la.x) * (p.x - lb.x) <= 0 && (p.y - la.y) * (p.y - lb.y) <= 0)
        return true;
    return false;
}
// test 7: remove points that form collided segment
bool cutBranchCollide(Point2i* vertexs) {
    // segment u: vertexs[0]-> vertexs[1]
    // segment v: vertexs[2]-> vertexs[3]
    if (xmult(vertexs[0], vertexs[2], vertexs[1])
        * xmult(vertexs[0], vertexs[3], vertexs[1]) <= 0)
        return true;  // might cross
    if (xmult(vertexs[2], vertexs[0], vertexs[3])
        * xmult(vertexs[2], vertexs[1], vertexs[3]) <= 0)
        return true; // might cross
    
    // segment u: vertexs[0] -> vertexs[2]
    // segment v: vertexs[1] -> vertexs[3]
    if (xmult(vertexs[0], vertexs[1], vertexs[2])
        * xmult(vertexs[0], vertexs[3], vertexs[2]) <= 0)
        return true;  // might cross
    if (xmult(vertexs[1], vertexs[0], vertexs[3])
        * xmult(vertexs[1], vertexs[2], vertexs[3]) <= 0)
        return true; // might cross
    
    return false;
}

// test 5: cut invalid point, segment out of line
bool cutBranchSeg(Point2i& a, Point2i& b, xcvSegment& seg) {
    if (dotOnLine(a, b, seg.begin))
        return false;
    if (dotOnLine(a, b, seg.end))
        return false;
    if (dotOnLine(seg.begin, seg.end, a))
        return false;
    if (dotOnLine(seg.begin, seg.end, b))
        return false;
    return true;
}

// test 4: cut distance( vertexs of two seg are too far away, like 50 pixels away )
// can be enhanced
bool cutBranchDist(xcvSegment*u, xcvSegment* v) {
    if (pointDist(u->begin, v->begin) < 2500)
        return false;
    if (pointDist(u->begin, v->end) < 2500)
        return false;
    if (pointDist(u->end, v->begin) < 2500)
        return false;
    if (pointDist(u->end, v->end) < 2500)
        return false;
    return false;
}

// test 3: inner angle < pi/4 or > 3pi/4
bool cutBranchAngle(xcvSegment* u, xcvSegment* v) {
    Point2i a, b;
    a.x = u->end.x - u->begin.x;
    a.y = u->end.y - u->begin.y;
    b.x = v->end.x - v->begin.x;
    b.y = v->end.y - v->begin.y;
    double cosab = (a.x * b.x + a.y * b.y) / (sqrt(u->dist) * sqrt(v->dist));
    // pi/4 < angle(a, b) < 3pi/4
    //printf("%f %f %f\n", cosab, COS8::six, COS8::two);
    if (cosab > COS8::six && cosab < COS8::two)
        return false;
    return true; // cut
}

// test 2: area < 1/20 img area
bool cutBranchArea(Point2i* vertexs, int imgArea) {
    double vArea = abs(xmult(vertexs[0], vertexs[1], vertexs[2])) / 2.0
    + abs(xmult(vertexs[3], vertexs[1], vertexs[2])) / 2.0;
    if (vArea < 0.05 * imgArea)
        return true; // cut!
    return false;
}

// test 1: heigth-width ratio > 3 or < 1/3
bool cutBranchHWRatio(Point2i* vertexs) {
    double h = min(pointDist(vertexs[0], vertexs[1]),
                   pointDist(vertexs[2], vertexs[3]));
    double w = max(pointDist(vertexs[0], vertexs[2]),
                   pointDist(vertexs[1], vertexs[3]));
    double r = sqrt(h / w);
    if (r < 0.3 || r > 3)
        return true;
    return false;
}

// get intersect point
Point2i intersetPoint(xcvSegment* u, xcvSegment* v) {
    Point2i ret = u->begin;
    double up = ((u->begin.x - v->begin.x) * (v->begin.y - v->end.y)
                 - (u->begin.y - v->begin.y) * (v->begin.x - v->end.x));
    double down = ((u->begin.x - u->end.x) * (v->begin.y - v->end.y)
                   - (u->begin.y - u->end.y) * (v->begin.x - v->end.x));
    double t = up / down;
    ret.x += (u->end.x - u->begin.x) * t;
    ret.y += (u->end.y - u->begin.y) * t;
    return ret;
}

// for debug
void showRect(Point2i* v) {
    Mat rectImg = QuadFinder::displayLineImg.clone();
    swap(v[2], v[3]);
    for (int i = 0; i < 4; i++) {
        cv::line(rectImg, v[i], v[(i + 1) % 4], cv::Scalar(0, 0, 0), 2);
    }
    char showRectTitle[128];
    sprintf(showRectTitle, "Debug/%d.png", QuadFinder::debugName);
    imwrite(showRectTitle, rectImg);
    imshow("proceeding", rectImg);
    swap(v[2], v[3]);
    waitKey(1);
}

double calSingleEdge(Point2i& a, Point2i b, xcvSegment& l, bool isH) {
    Point2i* st;
    Point2i* ed;
    if (isH) {
        if (a.x < l.begin.x)
            st = &l.begin;
        else
            st = &a;
        if (b.x > l.end.x)
            ed = &l.end;
        else
            ed = &b;
    } else {
        if (a.y < l.begin.y)
            st = &l.begin;
        else
            st = &a;
        if (b.y > l.end.y)
            ed = &l.end;
        else
            ed = &b;
    }
    double total_len = pointDist(&a, &b);
    double inside = pointDist(st, ed);
    
    double ret = inside / total_len;
    ret = 1 - ret; // gap:total_len
    
    double begin2ed = pointDist(&l.begin, ed);
    double st2end = pointDist(st, &l.end);
    double outside = 0;
    if (begin2ed > inside)
        outside += (begin2ed - inside) / total_len;
    if (st2end > inside)
        outside += (st2end - inside) / total_len;
    ret += outside;
    if (inside == 0)
        inside = 1;
    ret += outside / inside;
    return ret;
}

// core !! evaluation
double calValue(xcvSegment& va, xcvSegment& vb, xcvSegment& ha, xcvSegment& hb,
                Point2i* vertexs) {
    double ret = 0;
    ret += calSingleEdge(vertexs[0], vertexs[1], va, false);
    ret += calSingleEdge(vertexs[2], vertexs[3], vb, false);
    ret += calSingleEdge(vertexs[0], vertexs[2], ha, true);
    ret += calSingleEdge(vertexs[1], vertexs[3], hb, true);
    return ret;
}



// Main API
void xcvQuadranglesDetect(vector<xcvSegment>* lsgh, vector<xcvSegment>* lsgv,
                          Point2i* vertexs, int& imgh, int& imgw,
                          vector<CandidateRect> & candidateRects) {
    int imgArea = imgh * imgw;
    for (int i = 0; i < 4; i++) {
        vertexs[i].x = vertexs[i].y = 0;
    }
    sort(lsgh->begin(), lsgh->end());
    sort(lsgv->begin(), lsgv->end());
    addImgSegment(lsgh, lsgv, imgh, imgw);
    int hlen = lsgh->size();
    int vlen = lsgv->size();
    
    Point2i tmpVertexs[4]; // left-top left-down right-top right-bottom
    double minV = imgArea;
    Point2i* minP = NULL;
    
    for (int hi = 0; hi < hlen; hi++) {
        xcvSegment* line_ha = &((*lsgh)[hi]); // horizontal line
        for (int vi = 0; vi < vlen; vi++) {
            xcvSegment* line_va = &((*lsgv)[vi]);
            if (cutBranchDist(line_ha, line_va))
                continue;
            if (cutBranchAngle(line_ha, line_va))
                continue;
            
            for (int hj = hi + 1; hj < hlen; hj++) {
                xcvSegment* line_hb = &((*lsgh)[hj]);
                if (cutBranchDist(line_va, line_hb))
                    continue;
                if (cutBranchAngle(line_va, line_hb))
                    continue;
                
                for (int vj = vi + 1; vj < vlen; vj++) {
                    xcvSegment* line_vb = &((*lsgv)[vj]);
                    if (cutBranchDist(line_hb, line_vb))
                        continue;
                    if (cutBranchAngle(line_hb, line_vb))
                        continue;
                    
                    // to make a copy
                    xcvSegment lineva = *line_va;
                    xcvSegment linevb = *line_vb;
                    xcvSegment lineha = *line_ha;
                    xcvSegment linehb = *line_hb;
                    if (min(lineva.begin.x, lineva.end.x)
                        > min(linevb.begin.x, linevb.end.x)) {
                        swap(lineva, linevb);
                    }
                    if (min(lineha.begin.y, lineha.end.y)
                        > min(linehb.begin.y, linehb.end.y)) {
                        swap(lineha, linehb);
                    }
                    
                    tmpVertexs[0] = intersetPoint(&lineva, &lineha);
                    tmpVertexs[1] = intersetPoint(&lineva, &linehb);
                    tmpVertexs[2] = intersetPoint(&linevb, &lineha);
                    tmpVertexs[3] = intersetPoint(&linevb, &linehb);
                    if (cutBranchHWRatio(tmpVertexs))
                        continue;
                    if (cutBranchArea(tmpVertexs, imgArea))
                        continue;
                    if (cutBranchSeg(tmpVertexs[0], tmpVertexs[1], lineva))
                        continue;
                    if (cutBranchSeg(tmpVertexs[2], tmpVertexs[3], linevb))
                        continue;
                    if (cutBranchSeg(tmpVertexs[0], tmpVertexs[2], lineha))
                        continue;
                    if (cutBranchSeg(tmpVertexs[1], tmpVertexs[3], linehb))
                        continue;
                    if (cutBranchCollide(tmpVertexs))
                        continue;
                    
                    //old version
                    //double curV = (double)(line_ha->dist + line_hb->dist + line_va->dist + line_vb->dist)/sumDist(tmpVertexs);
                    
                    double curV = calValue(lineva, linevb, lineha, linehb,
                                           tmpVertexs);
                    if (curV < minV) {
                        for (int i = 0; i < 4; i++) {
                            vertexs[i].x = tmpVertexs[i].x;
                            vertexs[i].y = tmpVertexs[i].y;
                        }
                        minP = tmpVertexs;
                        minV = curV;
                    }
                    
                    CandidateRect tmpRect;
                    for(int i=0; i< 4; ++i){
                        tmpRect.vertexes[i] = tmpVertexs[i];
                    }
                    tmpRect.score = curV;
                    candidateRects.push_back(tmpRect);
                } 					// end vj
            } 					// end hj
        } 					// end vi
    } 					// end hi
    
    if (minP == NULL) {
        vertexs[0].x = 0;
        vertexs[0].y = 0; // left top
        vertexs[1].x = 0;
        vertexs[1].y = QuadFinder::row - 1; // left bottom
        vertexs[2].x = QuadFinder::col - 1;
        vertexs[2].y = 0; // right top
        vertexs[3].x = QuadFinder::col - 1;
        vertexs[3].y = QuadFinder::row - 1; // right bottom
    }
    
#ifdef PRINT_ALL_IMAGE
    showRect(vertexs);
#endif
    
    for (int i = 0; i < 4; i++) {
        vertexs[i].x *= QuadFinder::scale;
        vertexs[i].y *= QuadFinder::scale;
    }
}
/* end QuadranglesDetection Algorithm
 */

#define TEST_TIME

int findRect(Mat& img, Point2i* vertexs, std::vector<CandidateRect> & candidateVertexes) {
#ifdef TEST_TIME
    clock_t start_time_0 = clock();
#endif
    /*=====================================================*/
    Mat originimg = xcvPreprocessImage(img);
    QuadFinder::displayLineImg = originimg;
#ifdef TEST_TIME
    clock_t preprocess_time_1 = clock();
#endif
    
    /*=====================================================*/
    vector<xcvSegment> segh, segv;
    xcvEdgeDetect(originimg, &segh, &segv);
#ifdef TEST_TIME
    clock_t lineDetect_time_2 = clock();
#endif
    /*=====================================================*/
    
    xcvQuadranglesDetect(&segh, &segv, vertexs, QuadFinder::row,
                         QuadFinder::col, candidateVertexes);
#ifdef TEST_TIME
    clock_t quadrangleDetect_3 = clock();
#endif
    /*=====================================================*/
    
#ifdef TEST_TIME
    fprintf(stderr, "--------Quadragle Detect info------------\n");
    double total_time = (double) (quadrangleDetect_3 - start_time_0)
    / CLOCKS_PER_SEC;
    fprintf(stderr, "  [total time: %f]\n", total_time);
    fprintf(stderr, "  [corner 0: (%d, %d)]\n", vertexs[0].x, vertexs[0].y);
    fprintf(stderr, "  [corner 1: (%d, %d)]\n", vertexs[1].x, vertexs[1].y);
    fprintf(stderr, "  [corner 2: (%d, %d)]\n", vertexs[2].x, vertexs[2].y);
    fprintf(stderr, "  [corner 3: (%d, %d)]\n", vertexs[3].x, vertexs[3].y);
    
    double preprocess_time = (double) (preprocess_time_1 - start_time_0)
    / CLOCKS_PER_SEC;
    fprintf(stderr, "preprocess time[%.2f%%]: %f\n",
            preprocess_time * 100.0 / total_time, preprocess_time);
    
    double lineDetect_time = (double) (lineDetect_time_2 - preprocess_time_1)
    / CLOCKS_PER_SEC;
    fprintf(stderr, "lineDetect time[%.2f%%]: %f\n",
            lineDetect_time * 100 / total_time, lineDetect_time);
    
    double quadDetect_time = (double) (quadrangleDetect_3 - lineDetect_time_2)
    / CLOCKS_PER_SEC;
    fprintf(stderr, "quadrangleDetect time[%.2f%%]: %f\n",
            quadDetect_time * 100 / total_time, quadDetect_time);
#endif
    
    return 0;
}
