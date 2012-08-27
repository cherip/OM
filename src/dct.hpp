#ifndef _DCT_H_
#define _DCT_H_

#include <cv.h>

float *get_image_dct( const IplImage *src, const int nSub, int &featLen );

int compare_image_by_dct( const IplImage *src, const IplImage *dst );

float *get_image_canny(const IplImage *src);

#endif
