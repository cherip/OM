#ifndef _OM_HPP_
#define _OM_HPP_

//#include <cv.h>
#include "feature.hpp"
typedef unsigned char UINT8;
typedef unsigned char uint8;

//int *get_om(const IplImage *img, int &len);
void test_om(int mn = 1000);
void get_om_single();
void get_om_single(char *, char *);
void get_om_single(char *, char *, int type);
Feature *get_real_feature(uint8 *data, int w, int h);
int check_image_entropy(uint8 *data, int w, int h);
void *get_real_feature(uint8 *data, int w, int h, 
        int **orderAvg, int **orderEnt, int **orderPoint,
        int &nAvg, int &nEnt, int &nPoint);

#endif
