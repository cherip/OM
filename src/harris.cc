#include "harris.hpp"
#include "utils.hpp"

#include <opencv2/opencv.hpp>
//#include <highgui.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>

#define ABS(x) (x > 0 ? x : -x)

struct t_point {
    int x;
    int y;
    float val;
};

float mat[] = {-1.f, 0, 1.f};
int filterSize = 5;
float gauss[5][5];
float sigma = 2.5;
#define PRINT

int cmp(const t_point &a, const t_point &b) {
    return a.val - b.val;
}

int _cmp(const void *a, const void *b) {
    t_point *pa = (t_point *)a;
    t_point *pb = (t_point *)b;

    return pa->val - pb->val;
}

int is_extremun(float *val, int c, int r, int w, int h, int filter) {
    int cs = c - filter >= 0 ? c - filter : 0; 
    int rs = r - filter >= 0 ? r - filter : 0;
    int ce = c + filter <= w - 1 ? c + filter : w - 1;
    int re = r + filter <= h - 1 ? r + filter : h - 1;

    for (int i = rs; i < re; i++) {
        for (int j = cs; j < ce; j++) {
            if ((i != r || j != c) && val[i * w + j] >= val[r * w + c]) {
                return 0;
            }
        }
    }

    return 1;
}

int is_extremum_3D(float *up, float *mid, float *down, int c, int r,
                int w, int h) {
    float val = mid[w * r + c];
    int size = 1;
    for (int rr = -size; rr <= size; rr++) {
        for (int cc = -size; cc <= size; cc++) {
            if (up[w * (rr + r) + c + cc] >= val ||
                down[w * (rr + r) + c + cc] >=val ||
                ((rr != 0 || cc != 0) && mid[w * (rr + r) + cc + c] >= val)) {
                return 0;
            }
        }
    }

    return 1;
}

IplImage *derivateX(const IplImage *src) {
    CvMat matrix;
    matrix = cvMat(1, 5, CV_32F, mat);
    
    IplImage *dst = cvCloneImage(src);
    cvFilter2D(src, dst, &matrix);

    return dst;
}

IplImage *derivateY(const IplImage *src) {
    CvMat matrix;
    matrix = cvMat(5, 1, CV_32F, mat);
    
    IplImage *dst = cvCloneImage(src);
    cvFilter2D(src, dst, &matrix);
    
    return dst;
}

int _harris(IplImage *src, float threshold, float *_cims) {
    IplImage *deriX = derivateX(src);
    IplImage *deriY = derivateY(src);
    IplImage *deriXY = cvCloneImage(deriX);
    //  
    cvMul(deriX, deriY, deriXY);
    cvMul(deriX, deriX, deriX);
    cvMul(deriY, deriY, deriY);

    cvSmooth(deriX, deriX, CV_GAUSSIAN, 5);
    cvSmooth(deriY, deriY, CV_GAUSSIAN, 5);
    cvSmooth(deriXY, deriXY, CV_GAUSSIAN, 5);

    int w = src->width;
    int h = src->height;
    float *cims = _cims;
    float k = 0.06;
    float *vals = new float[w * h];
    memset(vals, 0, w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float det = Ix * Iy - Ixy * Ixy;
            float tr = Ix + Iy;

            float cim = det - k * tr * tr;
//          if (cim > threshold) {
//              cims[y * w + x] = cim;
//          } else 
//              cims[y * w + x] = 0.0;
            cims[y * w + x] = cim;
            vals[y * w + x] = cim;
        }
    }

    std::sort(vals, vals + w * h);
    int num =  w * h > 500 ? 500 : w * h * 3 / 4;
//  float thres = vals[w * h - 500];
    float thres = 5000;

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
//          if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, 10)) {
//          }
//          if (cims[y * w + x] < thres) {
//              cims[y * w + x] = 0;
//          }
        }
    }

    delete [] vals;
    cvReleaseImage(&deriX);
    cvReleaseImage(&deriY);
    cvReleaseImage(&deriXY);

    return 0; 
}

IplImage *harris(IplImage *img, float threshold, float ***ptsDes, int *npts, int *ndes, t_point **_pts) {
    IplImage *src = get_gray(img);
    IplImage *deriX = derivateX(src);
    IplImage *deriY = derivateY(src);
    IplImage *deriXY = cvCloneImage(deriX);
    //  
    cvMul(deriX, deriY, deriXY);
    cvMul(deriX, deriX, deriX);
    cvMul(deriY, deriY, deriY);

    cvSmooth(deriX, deriX, CV_GAUSSIAN, 5);
    cvSmooth(deriY, deriY, CV_GAUSSIAN, 5);
    cvSmooth(deriXY, deriXY, CV_GAUSSIAN, 5);

    IplImage *printImg = cvCloneImage(img);

    int w = src->width;
    int h = src->height;
    float *cims = new float[w * h];
    float *vals = new float[w * h];
    float k = 0.06;
    memset(vals, 0, sizeof(vals));
//  t_point *pts =new t_point[w * h + 1]();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float det = Ix * Iy - Ixy * Ixy;
            float tr = Ix + Iy;

            float cim = det - k * tr * tr;
            cims[y * w + x] = cim;
            vals[y * w + x] = cim;

//          pts[y * w + x].x = x;
//          pts[y * w + x].y = y;
//          pts[y * w + x].val = cim;
        }
    }
    
    std::sort(vals, vals + w * h);
    //float thres = 7000;
    //int num =  w * h > 4000 ? 4000 : w * h * 3 / 4;
    int num = w * h * 2 / 4;
    float thres = vals[w * h - num];
    t_point *pts = new t_point[num + 1];
    int count = 0;

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
            if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, filterSize)) {
//                drawPoint(printImg, x, y);
                if (cims[y * w + x] > thres) {
                    drawPoint(printImg, x, y);
                }
                pts[count].x = x;
                pts[count++].y = y;
            }
        }
    }
    
    float **desc;
    int descSize;

    std::cout << count << std::endl;
    //desc = describe_feature(src, pts, count, descSize);

//  std::cout << "\n\n****************\n";
//  for (int i = 0; i < count; i++) {
//      for (int j = 0; j < descSize; j++) {
//          std::cout << desc[i][j] << "\t";
//      }
//      std::cout << std::endl;
//  }
    
    /*return the result*/
//  *ptsDes = desc;
//  *npts = count;
//  *ndes = descSize;
//  *_pts = pts;

    if (count != 0) {
        cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
        cvShowImage("1", printImg);
        cvWaitKey(0);
    }

    delete [] vals;
    delete [] cims; 
    cvReleaseImage(&src);
    cvReleaseImage(&deriX);
    cvReleaseImage(&deriY);
    cvReleaseImage(&deriXY);

    return printImg;
}


IplImage *cmp_two_image(IplImage *src1, IplImage *src2) {
    float **des1, **des2;
    int npts1, npts2;
    int ndes;
    t_point *pts1, *pts2;

    IplImage *img1 = harris(src1, 0.01, &des1, &npts1, &ndes, &pts1);
//    IplImage *img2 = harris(src2, 0.01, &des2, &npts2, &ndes, &pts2);
    //IplImage *print = stack_imgs(src1, src2);
    IplImage *print = img1;

    return NULL;
}


/*
 * no use opencv
 */

//#define SH(x) (x)
#define SH(x)

typedef unsigned char uint8;
typedef float float32;

struct harris_point {
    int x;
    int y;
};


void uint8tofloat32(const uint8 *data, float32 *dst, int w, int h) {
    for (int idx = 0; idx < w * h; idx++) {
        dst[idx] = data[idx];
    }
}

void float32touint8(const float32 *data, uint8 *dst, int w, int h) {
    float32 maxPoint = 0.0f;
    for (int idx = 0; idx < w * h; idx++) {
        if (maxPoint < data[idx]) 
            maxPoint = data[idx];
    }
    printf("float32 to uint8 maxPoint: %.4f\n", maxPoint);
    maxPoint = 256 / maxPoint;
    for (int idx = 0; idx < w * h; idx++) {
        dst[idx] = (uint8)(data[idx] * maxPoint);
    }
}

void show_float32(float32 *src, int w, int h, const char *name) {
    uint8 *data = new uint8[w * h];
    float32touint8(src, data, w, h);
    IplImage *img = yuv2iplImage(data, w, h);
    show_image(img, name);

    delete [] data;
    cvReleaseImage(&img);
}

/*
 * in our program, the kernel size must be odd
 */
void convolve2D(const float32 *src, float32 *dst, int w, int h,
        float32 *kernel, int kSize) {
    int kCtr = kSize / 2; 
        
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int m = 0; m < kSize; m++) {
                int cm = kSize - 1 - m;
                for (int n = 0; n < kSize; n++) {
                    int cn = kSize - 1 - n;

                    int rIdx = i + (m - kCtr);
                    int cIdx = j + (n - kCtr);

                    if (rIdx >= 0 && rIdx < h &&
                        cIdx >= 0 && cIdx < w) {
                        dst[i * w + j] += src[rIdx * w + cIdx] * kernel[cm * kSize + cn];
                    }
                }
            }
        }
    }
}

void get_direct_gradient(const float32 *src, int w, int h, 
        float32 *pGradX, float32 *pGradY, float32 *pGradXY) {
    float32 Iy[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    float32 Ix[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    for (int i = 0; i < 9; i++) {
        Ix[i] /= 3;
        Iy[i] /= 3;
    }
//  float32 Ix[] = {0, 0, 0, -1, 0, 1, 0, 0, 0};
//  float32 Iy[] = {0, -1, 0, 0, 0, 0, 0, 1, 0};

    convolve2D(src, pGradX, w, h, Ix, 3);
    convolve2D(src, pGradY, w, h, Iy, 3);

    SH(show_float32(pGradX, w, h, "gradX"));
    SH(show_float32(pGradY, w, h, "gradY"));

    for (int idx = 0; idx < w * h; idx++) {
        pGradXY[idx] = pGradX[idx] * pGradY[idx];
//        printf("%.3f ", pGradXY[idx]);
    }

    for (int idx = 0; idx < w * h; idx++) {
        pGradX[idx] = pGradX[idx] * pGradX[idx];
        pGradY[idx] = pGradY[idx] * pGradY[idx];
    }
}


float32* create_gaussian_kernel(float32 sigma, int nSize) {
    float32 *pkernel = new float32[nSize * nSize];
    float32 nCenter = ((float32)nSize - 1) / 2;
    float32 dSum = 0;
    for (int i = 0; i < nSize; i++) {
        for (int j = 0; j < nSize; j++) {
            float32 dDis = i - nCenter;
            float32 dDiy = j - nCenter;

            float32 dValue = exp(-(dDis * dDis + dDiy * dDiy) / (2 * sigma * sigma));
            pkernel[i * nSize + j] = dValue;
            dSum += dValue; 
        }
    }

    for (int idx = 0; idx < nSize * nSize; idx++) {
        pkernel[idx] /= dSum;
        pkernel[idx] = floor(pkernel[idx] * 10000.0f + 0.5) / 10000.0f;
    }

//  for (int i = 0; i < nSize; i++) {
//      for (int j = 0; j < nSize; j++) {
//          printf("%.3f ", pkernel[i * nSize + j]);
//      }
//      printf("\n");
//  }

    return pkernel;
}

std::vector<harris_point>
harris(float32 *data, int w, int h, float32 sigma, int _thresh, int radius) {
    int num = w * h;
    float32 *gradX = new float32[num];
    float32 *gradY = new float32[num];
    float32 *gradXY = new float32[num];

    get_direct_gradient(data, w, h, gradX, gradY, gradXY);
    int nSize = (int)floor(3 * sigma);
    //int nSize = 3;
    float32 *pkernel = create_gaussian_kernel(sigma, nSize);

    float32 *outX = new float32[num];
    float32 *outY = new float32[num];
    float32 *outXY = new float32[num];

    convolve2D(gradX, outX, w, h, pkernel, nSize);
    convolve2D(gradY, outY, w, h, pkernel, nSize);
    convolve2D(gradXY, outXY, w, h, pkernel, nSize);

    float32 *ret = new float32[num];

    float32 maxRet = 0.0f;
    for (int i = 0; i < num; i++) {
        if (i % w < radius || i % w >= w - radius ||
            i / w < radius || i / w >= h - radius) {
            continue;
        }
        ret[i] = (outX[i] * outY[i] - outXY[i] * outXY[i]) -
                    0.04 * (outX[i] + outY[i]) * (outX[i] + outY[i]);
        if (maxRet < ret[i]) {
            maxRet = ret[i];
            printf("%d %d %.3f %.3f %.3f\n", i % w, i / w,
                    outX[i], outY[i], outXY[i]);
        }
    }

    printf("max ret : %.3f\n", maxRet);

    maxRet = 256 / maxRet;
    for (int i = 0; i < num; i++) {
        ret[i] = maxRet * ret[i];
//      if (ret[i] >= _thresh) {
//          printf("%d %d %.3f | ", i % w, i / w, ret[i]);
//      }
//        printf("%.3f ", ret[i]);
    }
//  show_float32(ret, w, h, "ret");

    int thresh = _thresh;

    std::vector<harris_point> result;
    harris_point point;
    for (int y = radius; y < h - radius; y++) {
        for (int x = radius; x < w - radius; x++) {
            double value = ret[y * w + x];
            if (value < thresh) {
                continue;
            }

            bool ismax = true;
            for (int ny = y - radius; ismax && ny <= y + radius; ny++) {
                for (int nx = x - radius; ismax && nx <= x + radius; nx++) {
                    ismax = ismax && (ret[ny * w + nx] <= value);
                }
            }

            if (!ismax) continue;

            point.x = x;
            point.y = y;
            result.push_back(point);
        }
    }
    printf("get %d harris points\n", result.size());

    delete [] ret;
    delete [] pkernel;
    
    delete [] outX;
    delete [] outY;
    delete [] outXY;
    
    delete [] gradX;
    delete [] gradY;
    delete [] gradXY;

    return result;
}


void harris(uint8 *data, int w, int h) {
    float32 *tmpData = new float32[w * h]; 
    uint8tofloat32(data, tmpData, w, h);
    float32 sigma = 1.8;
    int thresh = 5, radius = 5;
    std::vector<harris_point> result = harris(tmpData, w, h, sigma, thresh, radius);

    IplImage* printImg = yuv2iplImage(data, w, h);
    for (int i = 0; i < result.size(); i++) {
        drawPoint(printImg, result[i].x, result[i].y);
    }

    show_image(printImg, "harris");

    cvReleaseImage(&printImg);
    delete [] tmpData;
}

void test_harris(const char *path) {
    int w, h;
    uint8 *data = load_jpg_image(path, w, h);
    harris(data, w, h);
}




#ifndef HARRIS_H
#define HARRIS_H

class Harris
{
private:
    cv::Mat  cornerStrength;  //opencv Harris函数检测结果，也就是每个像素的角点响应函数值
    cv::Mat cornerTh; //cornerStrength阈值化的结果
    cv::Mat localMax; //局部最大值结果
    int neighbourhood; //邻域窗口大小
    int aperture;//sobel边缘检测窗口大小（sobel获取各像素点x，y方向的灰度导数）
    double k;
    double maxStrength;//角点响应函数最大值
    double threshold;//阈值除去响应小的值
    int nonMaxSize;//这里采用默认的3，就是最大值抑制的邻域窗口大小
    cv::Mat kernel;//最大值抑制的核，这里也就是膨胀用到的核
public:
    Harris():neighbourhood(3),aperture(3),k(0.01),maxStrength(0.0),threshold(0.01),nonMaxSize(3){

    };

    void setLocalMaxWindowsize(int nonMaxSize){
        this->nonMaxSize = nonMaxSize;
    };

    //计算角点响应函数以及非最大值抑制
    void detect(const cv::Mat &image){
            //opencv自带的角点响应函数计算函数
            cv::cornerHarris (image,cornerStrength,neighbourhood,aperture,k);
            double minStrength;
            //计算最大最小响应值
            cv::minMaxLoc (cornerStrength,&minStrength,&maxStrength);

            cv::Mat dilated;
            //默认3*3核膨胀，膨胀之后，除了局部最大值点和原来相同，其它非局部最大值点被
            //3*3邻域内的最大值点取代
            cv::dilate (cornerStrength,dilated,cv::Mat());
            //与原图相比，只剩下和原图值相同的点，这些点都是局部最大值点，保存到localMax
            cv::compare(cornerStrength,dilated,localMax,cv::CMP_EQ);
    }

    //获取角点图
    cv::Mat getCornerMap(double qualityLevel) {
            cv::Mat cornerMap;
            // 根据角点响应最大值计算阈值
            threshold= qualityLevel*maxStrength;
            cv::threshold(cornerStrength,cornerTh,
            threshold,255,cv::THRESH_BINARY);
            // 转为8-bit图
            cornerTh.convertTo(cornerMap,CV_8U);
            // 和局部最大值图与，剩下角点局部最大值图，即：完成非最大值抑制
            cv::bitwise_and(cornerMap,localMax,cornerMap);
            return cornerMap;
    }

    void getCorners(std::vector<cv::Point> &points,
            double qualityLevel) {
            //获取角点图
            cv::Mat cornerMap= getCornerMap(qualityLevel);
            // 获取角点
            getCorners(points, cornerMap);
    }

    // 遍历全图，获得角点
    void getCorners(std::vector<cv::Point> &points,
    const cv::Mat& cornerMap) {

            for( int y = 0; y < cornerMap.rows; y++ ) {
                    const uchar* rowPtr = cornerMap.ptr<uchar>(y);
                    for( int x = 0; x < cornerMap.cols; x++ ) {
                    // 非零点就是角点
                          if (rowPtr[x]) {
                                points.push_back(cv::Point(x,y));
                          }
                     }
                }
          }

    //用圈圈标记角点
    void drawOnImage(cv::Mat &image,
    const std::vector<cv::Point> &points,
            cv::Scalar color= cv::Scalar(255,255,255),
            int radius=3, int thickness=2) {
                    std::vector<cv::Point>::const_iterator it=points.begin();
                    while (it!=points.end()) {
                    // 角点处画圈
                    cv::circle(image,*it,radius,color,thickness);
                    ++it;
            }
    }

};

#endif // HARRIS_H

void harris_opencv(const char *path) {
//  IplImage *src = cvLoadImage(path, 1);
//  IplImage *pHarrisImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
//  IplImage *grayImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cv::Mat grayImg, src = cv::imread(path);
    cv::cvtColor(src, grayImg, CV_BGR2GRAY);

    Harris myHarris;
    myHarris.detect(grayImg);

    std::vector<cv::Point> pts;
    myHarris.getCorners(pts, 0.01);
    myHarris.drawOnImage(grayImg, pts);

//  IplImage *dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

//  int block_size = 7;
//  cvCornerHarris(grayImg, pHarrisImg, block_size, 3, 0.04);

//  double min = 0, max = 255;
//  double minVal, maxVal;
//  cvMinMaxLoc(pHarrisImg, &minVal, &maxVal, NULL, NULL, 0);
//  double scale = (max - min) / (maxVal - minVal);
//  double shift = -minVal * scale + min;
//  cvConvertScale(pHarrisImg, dst, scale, shift);

    cv::namedWindow("harris");
    cv::imshow("harris", grayImg);
    cv::waitKey(0);
}

void surf_opencv(const char *path) {
    IplImage *src = cvLoadImage(path);
    IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCvtColor(src, gray, CV_BGR2GRAY);

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *objectKeyPoints = 0;
    CvSURFParams params = cvSURFParams(500, 1);

    double tt = (double)cvGetTickCount();
    cvExtractSURF(gray, 0, &objectKeyPoints, NULL, storage, params, 0);

    tt = (double)cvGetTickCount() - tt;

    int nPoints = objectKeyPoints->total;
    printf("%d points found in %gms seconds\n", nPoints, 
            tt / (cvGetTickFrequency() * 1000.0));

    IplImage *printImg = cvCloneImage(src);
    for (int i = 0; i < nPoints; i++) {
        CvSURFPoint *r = (CvSURFPoint *)cvGetSeqElem(objectKeyPoints, i);
        drawPoint(printImg, r->pt.x, r->pt.y);
    }

    show_image(printImg, "surf");
}
