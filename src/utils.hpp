#ifndef _UTILS_H_
#define _UTILS_H_

#include <cv.h>
#include "feature.hpp"

typedef unsigned char uint8;

void show_image(const IplImage *src, const char *name);

/*
 * get y component of a image
 */
IplImage *get_component_Y(const IplImage *src);

IplImage *get_gray(const IplImage *src);
unsigned char get_img_val(const IplImage *src, int w, int h);
float pixval32f(const IplImage *src, int w, int h);
void set_pixval32f(const IplImage *src, int w, int h, float val);
void set_img_val(IplImage *src, int w, int h, unsigned char val);
unsigned char *iplImage2uint8point(IplImage *src);
IplImage *yuv2iplImage(const uint8* data, int w, int h);
void show_image_from_path(const char *path, const char *pathB);
void show_image_from_path(const char *path);
uint8* load_jpg_image(const char *path, int &w, int &h);
void show_yuv(const uint8* data, int w, int h);

void show_yuv_and_path(const uint8* data, int w, int h, const char *path);

void drawPoint(IplImage *src, int w, int h);
void drawPoint( IplImage *src, int w, int h, int color );
void show_query_result(const uint8 *data, int w, int h,
        BaseFeature **result, int nRes);
void show_query_result(IplImage* query, BaseFeature** result, int nRes);
void show_query_result(const char* query, BaseFeature** result, int nRes);
#endif
