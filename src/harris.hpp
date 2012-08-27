#ifndef _HARRIS_H_
#define _HARRIS_H_

#include <cv.h>
IplImage *derivateX(const IplImage *img);
IplImage *derivateY(const IplImage *img);
IplImage *harris(IplImage *, float, float ***ptsDes, int *npts, int *ndes);
IplImage *harris_laplace(IplImage *img, float threshold);
IplImage *cmp_two_image(IplImage *src1, IplImage *src2);
void test_harris(const char *path);
void harris_opencv(const char *path);
void surf_opencv(const char *path);

#endif
