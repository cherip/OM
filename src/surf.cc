#include <cv.h>
#include <highgui.h>

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
}
