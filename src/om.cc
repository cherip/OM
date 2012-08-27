#include "om.hpp"
#include "utils.hpp"
//#include "file.hpp"
#include "query.hpp"
#include "dbconn.hpp"

#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cmath>

//#define N 100 
//#define N2 8 
#define subN 0


int N = 25;
int N2 = 8;
const float ENTROPY_THRESHOLD = 3.5;

struct position{
    int idx;
    float val;
};

int cmp(const void *pa, const void *pb) {
    position *posA = (position *)pa;
    position *posB = (position *)pb;

    return posA->val > posB->val;
}

int* pos2result(position *pos, int len) {
//  if (len == 0) return NULL;
    int *show = new int[len];

    for (int i = 0; i < len; i++) {
        show[pos[i].idx] = i;
    }

    /*
     * show image ordinal result
     */
//    int up;
//    if (len == N * N)
//        up = N;
//    else if (len = N2 * N2)
//        up = N2;
//    for (int i = 0; i < up; i++) {
//        for (int j = 0; j < up; j++) {
//            printf("%d", show[i * up + j]); 
//            printf("(%.3f)\t", pos[show[i * up + j]].val);
//        }
//        printf("\n");
//    }
//    printf(">>>>>>>>>>>>>>\n");

    return show;
}

void get_rect_feature(uint8 *data, int w, int h, 
        int x, int y, int rw, int rh, float *avg, float *entropy) {
    const int nbox = 64; 
    //int *num = new int[nbox];
    int num[nbox];
    int sum = 0;
    for (int i = 0; i < nbox; i++) {
        num[i] = 0;
    }

    for (int i = y; i < y + rh; i++) {
        for (int j = x; j < x + rw; j++) {
            assert(i < h && j < w);
            int val = data[i * w + j];
            num[val * nbox / 256]++;
            sum += val;
        }
    }

    int total = rw * rh;
    float val = 0.0; 
    for (int i = 0; i < nbox; i++) {
        float per = num[i] / (float)total;
        val += -1 * per * log(per + 0.000001);
    }

    //delete [] num;
    
    *avg = (float)sum / (rw * rh);
    *entropy = val;
}

void get_rect_avg(uint8 *data, int w, int h,
        int x, int y, int rw, int rh, float *avg) {
    int sum = 0;
    for (int i = y; i < y + rh; i++) {
        for (int j = x; j < x + rw; j++) {
            assert(i < h && j < w);
            sum += data[i * w + j];
        }
    }

    *avg = (float) sum / (rw * rh);
}

void get_rect_entropy(uint8 *data, int w, int h,
        int x, int y, int rw, int rh, float *entropy) {
    const int nbox = 64; 
    int num[nbox];
    for (int i = 0; i < nbox; i++) {
        num[i] = 0;
    }

    for (int i = y; i < y + rh; i++) {
        for (int j = x; j < x + rw; j++) {
            assert(i < h && j < w);
            int val = data[i * w + j];
            num[val * nbox / 256]++;
        }
    }

    int total = rw * rh;
    float val = 0.0; 
    for (int i = 0; i < nbox; i++) {
        float per = num[i] / (float)total;
        val += -1 * per * log(per + 0.000001);
    }

    *entropy = val;
}

#define STEP_SIZE 2
#define CHECK_POINT 50 
#define TH_LUMA 30
#define VARIA_VAL 500
#define BIN_THRESH 0.9
#define CBIN_THRESH 0.62

//#define _P(x) (x)
#define _P(x) 

void show_edge_image(uint8 *data, int w, int h, int left, int right, int top, int bottom) {

    return;
    printf("width: %d height: %d\n", w, h);
    printf("top: %d bottom: %d left: %d right: %d\n",
            top, bottom, left, right);

    if (top != 0 || left != 0 || bottom != h - 1 || right != w - 1) {
        IplImage *ptr = yuv2iplImage(data, w, h);
        cvRectangle(ptr, cvPoint(left, top), cvPoint(right, bottom), cvScalar(0, 255, 0));
        show_image(ptr, "rect");

        cvReleaseImage(&ptr);
    }
} 

inline int check_edge2(const int *sample, const int len) {
    const int nBin = 128;
    const int nClrBin = 64;
    int bin[nBin];
    int clrBin[nClrBin];
    for (int i = 0; i < nBin; i++) {
        bin[i] = 0;
    }
    for (int i = 0; i < nClrBin; i++) {
        clrBin[i] = 0;
    }

    for (int i = 0; i < len - 1; i++) {
        assert(sample[i] >= 0);
//      printf("%d %d\t", sample[i], sample[i] * nClrBin / 255);
//      if (i % 6 == 0) printf("\n");
        clrBin[sample[i] * nClrBin / 255]++;
        bin[(sample[i + 1] - sample[i] + 255) * nBin / 510]++;
    }
    _P(printf("\n"));
    int count = 0;
    for (int i = 0; i < nBin; i++) {
        _P(printf("%.4g ", (float)bin[i] / len));
        if (bin[i] > 0) count++;
    }
    _P(printf("\n"));
    int count2 = 0;
    for (int i = 0; i < nClrBin; i++) {
        _P(printf("%d ", clrBin[i]));
        if (clrBin[i] > 5) count2++;
    }
    _P(printf("\n"));
    if (count2 <= 7 && count <= 2) return 0;
    if (count2 > 7) return 1;

    std::sort(bin, bin + nBin);
    std::sort(clrBin, clrBin + nClrBin);
    int sum = 0;
    int clrSum = 0;
    int thresh = len * BIN_THRESH;
    int thresh2 = len * CBIN_THRESH;
    int flag = 0;
    for (int i = nBin - 1, k = 2; k > 0; i--, k--) {
        sum += bin[i];     
        _P(printf("%.4g\n", (float)sum / len));
        if (sum > thresh) {
            flag = 1;
            break;
        }
    }
    _P(printf("color: %d\n", len));
    for (int i = nClrBin - 1, k = 3; flag && k > 0; i--, k--) {
        clrSum += clrBin[i];
        _P(printf("%d %.4g\n", clrSum, (float)clrSum / len));
        if (clrSum > thresh2) {
            return 0;
        }
    }

    return 1;
}

void get_edge2(uint8 *data, int w, int h, int &sw, int &sh, int &pw, int &ph) { 
    int top = 0, bottom = h - 1;
    int left = 0, right = w - 1;

    int sampleLen = w > h ? w : h;
    int *sample = new int[sampleLen];
    // left
    _P(printf("---->left:\n"));
    for (int line = 0; line < w / 4; line += 2) {
        int k;
        for (k = 0; k < h; k++) {
            uint8 *point = data + w * k + line;
            //sample[k] = point[0] - point[1] + 255;
            sample[k] = point[0];
        } 
        left = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //right
    _P(printf("---->right:\n"));
    for (int line = w - 1; line > 3 * w / 4; line -= 2) {
        int k;
        for (k = 0; k < h; k++) {
            uint8 *point = data + w * k + line;
            sample[k] = point[0];
        }
        right = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //top
    _P(printf("---->top:\n"));
    for (int line = 0; line < h / 4; line += 2) {
        int k;
        uint8 *point = data + line * w;
        for (k = 0; k < w; k++) {
            sample[k] = point[k];
        }
        top = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //bottom
    _P(printf("---->bottom:\n"));
    for (int line = h - 1; line > 3 * h / 4; line -= 2) {
        int k;
        uint8 *point = data + line * w;
        for (k = 0; k < w; k++) {
            sample[k] = point[k];
        }
        bottom = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    sw = left;
    sh = top;
    pw = right - left + 1;
    ph = bottom - top + 1;
    show_edge_image(data, w, h, left, right, top, bottom);
}

inline int check_edge(int value) {
    int avg = value / (3 * (CHECK_POINT + 1));
    //printf("avg : %d\n", avg);
//  if (avg > TH_LUMA || avg < 256 - TH_LUMA) {
//      return 1;
//  }
    if (avg > TH_LUMA) {
        return 1;
    }
    return 0;
}

inline int check_edge(const int *sample, const int len) {
    const int binSize = 64;
    int bin[binSize];

    for (int i = 0; i < binSize; i++) 
        bin[i] = 0;

    for (int i = 0; i < len; i++) {
//      printf("%d ", sample[i]);
        bin[sample[i] * binSize / 255]++;
    }

    std::sort(bin, bin + binSize);
    int sum = 0;
    int thresh = len * BIN_THRESH;
    for (int i = binSize - 1, k = 3; k > 0; i--, k--) {
        sum += bin[i];
        printf("%.4g\n", (float)sum / len);
        if (sum > thresh) {
            printf("not edge\n");
            return 0;
        }
    }

    return 1;
}

void get_edge(uint8 *data, int w, int h, int &sw, int &sh, int &pw, int &ph) {
    //== remove the margin
    int yValue = 0;
    int lineNum = 0;
    uint8 bCont = 1;
    uint8 *pPoint = NULL;
    int widthStep = (w) / (CHECK_POINT - 1) - 1;
    int heightStep = (h) / (CHECK_POINT - 1) - 1;

    //int sample[CHECK_POINT];
    int tmpLen = w > h ? w : h;
    int *sample = new int[tmpLen];

    int top = 0, bottom = h - 1, left = 0, right = w - 1;

    //== 1.1 top
    printf("top:\n");
    for (lineNum = 0; bCont && lineNum < h / 4; lineNum += STEP_SIZE) {
        yValue = 0;
        for (int i = 0; i < w; i++) {
            pPoint = data + lineNum * w + i;
            sample[i] = pPoint[0];
        }
        if (check_edge(sample, w)) {
            bCont = 0;
        }
        top = lineNum;
    }

    //== 1.2 bottom
    printf("bottom:\n");
    for (bCont = 1, lineNum = h - 1; bCont && lineNum > 3 * h / 4;
            lineNum -= STEP_SIZE) {
        yValue = 0;
        for (int i = 0; i < w; i++) {
            pPoint = data + lineNum * w + i;
            sample[i] = pPoint[0];
        }
        if (check_edge(sample, w)) {
            bCont = 0;
        }
        bottom = lineNum;
    }

    int hStart = 0;
    int hEnd = h - 1;
    //== 1.3 left
    printf("left:\n");
    for (bCont = 1, lineNum = 0; bCont && lineNum < w / 4;
            lineNum += STEP_SIZE) {
        yValue = 0;
        for (int i = hStart; i <= hEnd; i++) {
            pPoint = data + lineNum + i * w;
            sample[i] = pPoint[0];
        }
        if (check_edge(sample, hEnd - hStart + 1)) {
            bCont = 0;
        }
        left = lineNum;
    }

    //== 1.4 right
    printf("right:\n");
    for (bCont = 1, lineNum = w - 1; bCont && lineNum > 3 * w / 4;
            lineNum -= STEP_SIZE) {
        yValue = 0;
        for (int i = hStart; i <= hEnd; i++) {
            pPoint = data + lineNum + i * w;
            sample[i] = pPoint[0];
        }
        if (check_edge(sample, hEnd - hStart + 1)) {
            bCont = 0;
        }
        right = lineNum;
    }

    sw = left;
    sh = top;
    pw = right - left + 1;
    ph = bottom - top + 1;

    printf("width: %d height: %d\n", w, h);
    printf("top: %d bottom: %d left: %d right: %d\n",
            top, bottom, left, right);

    IplImage *ptr = yuv2iplImage(data, w, h);
    cvRectangle(ptr, cvPoint(left, top), cvPoint(right, bottom), cvScalar(0, 255, 0));
    show_image(ptr, "rect");

    cvReleaseImage(&ptr);
}

int check_image_entropy(uint8 *data, int w, int h) {
    int sw, sh, pw, ph;
    get_edge(data, w, h, sw, sh, pw, ph);

    const int box = 256;
    int num[box];

    for (int i = 0; i < box; i++)
        num[i] = 0;

    //printf("\n%d %d %d\n", pw, ph, pw * ph);

    for (int i = sh; i < sh + ph; i++) {
        for (int j = sw; j < sw + pw; j++)
            num[data[i * w + j] * box / 256]++;
    }

    float sum = 0;
    int total = pw * ph;

    for (int i = 0; i < box; i++) {
        //printf(" %d", num[i]);
        float per = (float)num[i] / (float)total;    
        sum += -1.0 * per * log(per + 0.000001);
    }


    if (sum < ENTROPY_THRESHOLD) {
        printf("%.3f\n", sum);
        return 0;
    } else {
        return 1;
    }
}

int check_image_entropy(uint8 *data, int w, int h, int sw, int sh, int pw, int ph) {
    const int box = 256;
    int num[box];

    for (int i = 0; i < box; i++)
        num[i] = 0;

    for (int i = sh; i < sh + ph; i++) {
        for (int j = sw; j < sw + pw; j++)
            num[data[i * w + j] * box / 256]++;
    }

    float sum = 0;
    int total = pw * ph;

    for (int i = 0; i < box; i++) {
        //printf(" %d", num[i]);
        float per = (float)num[i] / (float)total;    
        sum += -1.0 * per * log(per + 0.000001);
    }

    printf("%.3f\n", sum);

    if (sum < ENTROPY_THRESHOLD) {
        return 0;
    } else {
        return 1;
    }
}

Feature *get_real_feature(uint8 *data, int w, int h) {
    /*
     * we only use the middle center area of the image
     *  ____________
     *  | _______  |
     *  | |      | |
     *  | |      | |
     *  | |______| |
     *  |__________|
     */

    int sw, sh, pw, ph;
    get_edge(data, w, h, sw, sh, pw, ph);

    int subw = pw / N2;
    int subh = ph / N2;
    int len = N2 * N2;
    position *posAvg = new position[len];
    position *posEnt = new position[len];

    int idx = 0;
    for (int i = 0; i < N2; i++) {
        for (int j = 0; j < N2; j++) {
            int leftX = j * subw + sw;
            int leftY = i * subh + sh;

            get_rect_feature(data, w, h, leftX, leftY, subw, subh,
                    &posAvg[idx].val, &posEnt[idx].val);
            posAvg[idx].idx = i * N2 + j;
            posEnt[idx].idx = i * N2 + j;
            idx++;
        }
    }

    qsort(posAvg, len, sizeof(position), cmp);
    int *resAvg = pos2result(posAvg, len);
    qsort(posEnt, len, sizeof(position), cmp);
    int *resEnt = pos2result(posEnt, len);
    
    Feature *feature = new OmFeature(resAvg, resEnt, len);

    delete [] posAvg;
    delete [] posEnt;

    return feature;
}

void *get_real_feature(uint8 *data, int w, int h, 
        int **orderAvg, int **orderEnt, int **orderPoint,
        int &nAvg, int &nEnt, int &nPoint) {
    /*
     * we only use the middle center area of the image
     *  ____________
     *  | _______  |
     *  | |      | |
     *  | |      | |
     *  | |______| |
     *  |__________|
     */

    int sw = 0, sh = 0, pw = w, ph = h;
    get_edge2(data, w, h, sw, sh, pw, ph);

    int subw = pw / N2;
    int subh = ph / N2;
    int len = N2 * N2;
    position *posAvg = NULL, *posEnt = NULL;
    if (orderAvg != NULL)
        posAvg = new position[len];
    if (orderEnt != NULL)
        posEnt = new position[len];

    int idx = 0;
    for (int i = 0; i < N2; i++) {
        for (int j = 0; j < N2; j++) {
            int leftX = j * subw + sw;
            int leftY = i * subh + sh;

            if (orderAvg != NULL) {
                get_rect_avg(data, w, h, leftX, leftY, subw, subh,
                    &posAvg[idx].val);
                posAvg[idx].idx = i * N2 + j;
            }
            if (orderEnt != NULL) {
                get_rect_entropy(data, w, h, leftX, leftY, subw, subh,
                    &posEnt[idx].val);
                posEnt[idx].idx = i * N2 + j;
            }
            idx++;
        }
    }

    if (orderAvg != NULL) {
        qsort(posAvg, len, sizeof(position), cmp);
        *orderAvg = pos2result(posAvg, len);
        nAvg = len;
    }
    if (orderEnt != NULL) {
        qsort(posEnt, len, sizeof(position), cmp);
        *orderEnt = pos2result(posEnt, len);
        nEnt = len;
    }

    if (orderAvg != NULL) {
        delete [] posAvg;
//        printf("delete\n");
    }
    if (orderEnt != NULL)
        delete [] posEnt;
}
