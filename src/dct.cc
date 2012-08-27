#include "dct.hpp"
#include "utils.hpp"

#include <highgui.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <dirent.h>

void print_32f_image( const IplImage *src ) {
    for ( int y = 0; y < src->height; y++ ) {
        for ( int x = 0; x < src->width; x++ ) {
            std::cout << pixval32f(src, x, y) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "*********************\n";
}

void print_dct_feature( const float *feature, const int nLen ) {
    for ( int i = 0, index = 0; i < nLen; i++ ) {
        for ( int j = 0; j < nLen; j++) {
            if ( i == 0 && j == 0 ) {
                printf( "%.4f\t", 0.0 );
                continue;
            }
            printf( "%.4f\t", feature[ index++ ] );
        }
        std::cout << std::endl;
    }
    std::cout << "\n**********************\n";
}

IplImage *resize_image( IplImage *src, const int nSub ) {
    int subWidth = src->width / nSub;
    int subHeight = src->height / nSub;
    IplImage *subImage = cvCreateImage( cvSize( nSub, nSub ),
            IPL_DEPTH_32F, 1);
   
    for ( int i = 0; i < nSub; i++ ) {
        for ( int j = 0; j < nSub; j++ ) {
            int subY = i * subHeight;
            int subX = j * subWidth;

            cvSetImageROI( src, cvRect( subX, subY, subWidth, subHeight ) );
            CvScalar avg = cvAvg( src ); 
            set_pixval32f( subImage, j, i, avg.val[0] );
            cvResetImageROI( src );

      //      std::cout << avg.val[0] << " ";
        }
      //  std::cout << std::endl;
    }
    //std::cout << "----------------\n";

    return subImage;
}

int cmp_dct_feature( const void *a, const void *b ) {
    float va = *( float * )a;
    float vb = *( float * )b;

    return vb > va;
}

float *get_image_dct( const IplImage *src, const int nSub, int &featLen ) {
//  IplImage *img = get_gray( src ); 
    IplImage* img = cvCloneImage(src);
    IplImage *subImage = resize_image( img, nSub ); 
    IplImage *dctImage = cvCloneImage( subImage );

    cvDCT( subImage, dctImage, CV_DXT_FORWARD );
//  print_32f_image( dctImage );

//  float *data = ( float * )( dctImage->imageData );
//  qsort( data + 1, nSub * nSub - 1, sizeof( float ), cmp );
//  print_32f_image( dctImage );
    
    int subDct = 6;
    featLen = subDct * subDct - 1;
    int index = 0;
    float *feature = new float[ featLen ];
    /*copy only 35 points of dcts*/
    for ( int y = 0; y < subDct; y++ ) {
        for ( int x = 0; x < subDct; x++ ) {
            if ( x == 0 && y == 0 ) 
                continue;
            feature[ index++ ] = pixval32f( dctImage, x, y );
        }
    }

    /*print the feature without sort*/
//  print_dct_feature( feature, subDct );

    qsort( feature, featLen, sizeof( float ), cmp_dct_feature );
    //sort( feature, feature + featLen );
    /*print the feature after sort*/
    //print_dct_feature( feature, subDct );
//  for ( int i = 0; i < featLen; i++ ) {
//      printf( "%.3f\t", feature[ i ] );
//  }
//  std::cout << std::endl;

//  show_image( img, "gray" );

    cvReleaseImage( &img );
    cvReleaseImage( &subImage );
    cvReleaseImage( &dctImage );

    return feature;
}

int compare_image_by_dct( const IplImage *src, const IplImage *dst ) {
//  const int nSub = 8;
//  int featLenSrc;
//  int featLenDst;
//  float *featSrc = get_image_dct( src, nSub, featLenSrc );
//  float *featDst = get_image_dct( dst, nSub, featLenDst );

//  if ( featLenSrc != featLenDst ) {
//      std::cout << "error!\n";
//      return -1;
//  }

//  float dist = 0.0f;
//  for ( int i = 0; i < featLenSrc; i++ ) {
//      std::cout << ABS( featSrc[ i ] - featDst[ i ] ) << " ";
//      dist += ABS( featSrc[ i ] - featDst[ i ] ); 
//  }

//  std::cout << dist << std::endl;

//  delete [] featSrc;
//  delete [] featDst;
//  return 1;
}

//int compare_dir_by_dct() {
//  const char path[] = ""
//  struct dirent **namelist;
//}
float *get_image_canny(const IplImage *img) {
//  IplImage *gray = get_gray(src);
    IplImage *src = cvCloneImage(img);
    IplImage *pCannyImg = cvCreateImage(cvGetSize(src),
                                        IPL_DEPTH_8U,
                                        1);
    cvSmooth(src, src, CV_GAUSSIAN, 5);
    cvCanny(src, pCannyImg, 50, 150, 3);

//  show_image(gray, "original");
    show_image(pCannyImg, "canny");

//  cvReleaseImage(&gray);
    cvReleaseImage(&pCannyImg);

    return NULL;
}
