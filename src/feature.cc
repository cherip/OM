#include "feature.hpp"
#include "om.hpp"
#include "utils.hpp"
#include "dct.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int Feature::get_feature_ID() const {
    return featureID;
}

void Feature::set_feature_ID(int id) {
    featureID = id;
}

float Feature::inter_compare(const int *a, const int *b, int len) {
//    int *rea = new int[len];
    //static int *rea = new int[len];
    //static int *s = new int[len];
	int rea[64], s[64];
    for (int i = 0; i < len; i++) {
        rea[a[i]] = i;    
    }

//    int *s = new int[len];
    for (int i = 0; i < len; i++) {
        s[i] = b[rea[i]];
    }

    int max = 0;
    for (int i = 0; i < len; i++) {
        int tmp = 0;
        for (int j = 0; j <= i; j++) {
             if (s[j] > i) tmp++;
        }
        if (tmp > max) {
            max = tmp;
        }
    }
    
//  for (int i = 0; i < len; i++) printf("%d ", rea[i]);
//  printf("\n");
//  for (int i = 0; i < len; i++) printf("%d ", s[i]);
//  printf("\n");
//  printf("max: %d\n", max);

    int n = len / 2;
    float k = (n - 2 * max) / (float)n;

    return k;
}

OmFeature::OmFeature(int *fA, int *fB, int len) {
    a = fA;
    b = fB;
    _len = len;

    inter_handle();
    valid = 1;
}

OmFeature::OmFeature(int *fA, int *fB, int len, char *pname) {
    a = fA;
    b = fB;
    _len = len;
    valid = 1;

    strcpy(pName, pname);

    inter_handle();
}

OmFeature::OmFeature() {
    valid = 1;
}

OmFeature::~OmFeature() {
    if (a != NULL)
        delete [] a;
    if (b != NULL)
        delete [] b;
    
    delete bitCode;
}

void OmFeature::inter_handle() {
    bitCode = new BitCode(_len); 

    int tmp = _len / 2;
    for (int i = 0; i < _len; i++) {
        if (a[i] > tmp) {
            bitCode->set_bit(i);
        }
    }
}

float OmFeature::compare(const Feature *_rptr) {
    OmFeature *rptr = (OmFeature *)_rptr;

    int bitDiff = bitCode->compare(rptr->bitCode);
    if (bitDiff > 8) {
//        printf("bit error!\n");
        return -1.0;
    }

    float resA = 1.0, resB = 1.0;
    if (a != NULL)
        resA = inter_compare(a, rptr->a, _len);
    if (b != NULL) 
        resB = inter_compare(b, rptr->b, _len);
    
    //printf("%f %f\n", resA, resB);

    return resA > resB ? resB : resA;
}

char *OmFeature::serialize() {
    static char str[512];
    int pos = 0;
    pos += sprintf(str + pos, "%d;", _len);
    for (int i = 0; i < _len; i++) {
        pos += sprintf(str + pos, "%d|", a[i]);
    }
    for (int i = 0; i < _len; i++) {
        pos += sprintf(str + pos, "%d|", b[i]);
    }
    str[pos] = 0;

    return str;
}

void OmFeature::save(FILE *&fp) {
    fprintf(fp, "%d;\n", _len);
    for (int i = 0; i < _len; i++) {
        fprintf(fp, "%d|", a[i]);
    }
	fprintf(fp, "\n");
    for (int i = 0; i < _len; i++) {
        fprintf(fp, "%d|", b[i]);
    }
    fprintf(fp, "\n");
}

char *OmFeature::unserialize(char *str) {
    sscanf(str, "%d;", &_len);
    a = new int[_len];
    b = new int[_len];

    int idx = 0;
    char *pre = str, *ptr = str;
    while (*ptr != ';') ptr++;
    pre = ptr + 1;

    ptr = pre + 1;
    while (idx < _len) {
        while (*ptr != '|') ptr++;
        *ptr = 0;
        a[idx++] = atoi(pre);
        pre = ptr + 1;
    }
    idx = 0;
    while (idx < _len) {
        while (*ptr != '|') ptr++;
        *ptr = 0;
        b[idx++] = atoi(pre);
        pre = ptr + 1;
    }

//  for (int i = 0; i < _len; i++) {
//      printf("(%d %d) ", a[i], b[i]);
//  }
//  printf("\n");

    inter_handle();
//  bitCode->print();
//  getchar();

    return NULL;
}

char *OmFeature::get_name() {
    return pName;
}

void OmFeature::set_name(char *pname) {
    strcpy(pName, pname);
}

void OmFeature::print() {
    printf("bit code: ");
    bitCode->print();
    for (int i = 0; i < _len; i++) {
        printf("%d ", a[i]);
        if (i != 0 && i % 8 == 0) printf("\n");
    }
    printf("\n-------\n");
    for (int i = 0; i < _len; i++) {
        printf("%d ", b[i]);
        if (i != 0 && i % 8 == 0) printf("\n");
    }
    printf("\n");
}

int OmFeature::get_valid() {
    return valid;
}

void OmFeature::read_from_file(FILE *pf) {
    fscanf(pf, "%d\n", &(valid));
    set_feature_ID(valid);

    if (fgets(pName, 256, pf)) {
        pName[strlen(pName) - 1] = '\0';
    } else {
        set_name("not name");
    }

    char dataBuff[1024];
    fgets(dataBuff, 1024, pf);
//tmpFeature->set_name("not");
    unserialize(dataBuff);
}


/*
 * non-member functions
 */

Feature *read_feature_from_file(const char *path) {
    FILE *pf = fopen(path, "r");
    if (pf == NULL) {
        return NULL;
    }

    Feature *pFeature = new OmFeature();
    pFeature->read_from_file(pf);

    fclose(pf);
    return pFeature;
}

/*
 * BaseFeature Structure
 */
BaseFeature::BaseFeature() {
    featureID = -1;
    strcpy(name, "noname");
}

BaseFeature::~BaseFeature() {}

char *BaseFeature::get_name() { 
    return this->name;
}

char *BaseFeature::set_name(const char *name) {
    strcpy(this->name, name);
    return this->name;
}

int BaseFeature::get_feature_ID() const {
    return featureID;
}

void BaseFeature::set_feature_ID(int id) {
    featureID = id;
}

void BaseFeature::init_feature_from_file(FILE *&pf) {
    fscanf(pf, "%d\n", &featureID);
    fgets(name, 128, pf);
    name[strlen(name) - 1] = '\0';
}

void BaseFeature::save_feature_to_file(FILE *&pf) {
    fprintf(pf, "%d\n%s\n", featureID, name);
}

void BaseFeature::print() {
    printf("%d\n%s\n", featureID, name);
}

/*
 * MyOmFeature Structure
 */
float compare_ominfo(OmInfo &a, OmInfo &b, int thres) {
    if (a.len != b.len) {
        return -1.0;
    }

//  a.bitCode->print();
//  b.bitCode->print();
//  printf("%d\n", a.bitCode->compare(b.bitCode));
    if (a.bitCode->compare(b.bitCode) > thres) {
        return -1.0;
    }

	int rea[64], s[64];
    int len = a.len;
    for (int i = 0; i < len; i++) {
        rea[a.order[i]] = i;    
    }

    for (int i = 0; i < len; i++) {
        s[i] = b.order[rea[i]];
    }

    int max = 0;
    for (int i = 0; i < len; i++) {
        int tmp = 0;
        for (int j = 0; j <= i; j++) {
             if (s[j] > i) tmp++;
        }
        if (tmp > max) {
            max = tmp;
        }
    }

//  printf("max %d\n", max);
    int n = len / 2;
    float k = (n - 2 * max) / (float)n;

    return k;
} 

BitCode *init_bitCode(int *data, int len) {
    BitCode *bitCode = new BitCode(len); 

    int tmp = len / 2;
    for (int i = 0; i < len; i++) {
        if (data[i] > tmp) {
            bitCode->set_bit(i);
        }
    }
    
    return bitCode;
}

int read_ominfo(FILE *&pf, OmInfo &result) {
    int res = fread(&(result.len), sizeof(int), 1, pf);
    if (res != 1)
        return -1;
    result.order = new int[result.len];
    fread(result.order, sizeof(int), result.len, pf);
    
    result.bitCode = init_bitCode(result.order, result.len);

    return 0;
}

void save_ominfo(FILE *&pf, OmInfo &result) {
    fwrite(&(result.len), sizeof(int), 1, pf);
    fwrite(result.order, sizeof(int), result.len, pf);
}

void print_ominfo(const OmInfo &ominfo) {
    printf("%d: ", ominfo.len);
    for (int i = 0; i < ominfo.len; i++) {
        printf("%d ", ominfo.order[i]);
    }
    printf("\n");
}

MyOmFeature::MyOmFeature() {}
MyOmFeature::~MyOmFeature() {}

float MyOmFeature::compare(const BaseFeature *rptr) {
    MyOmFeature *realPtr = (MyOmFeature *)rptr;        

    int flag = 1;
    float minDis = 1.0f;
    for (int i = 0 ; i < features.size(); i++) {
        float dis = compare_ominfo(features[i], realPtr->features[i], 15);
//      printf("myomfeature dis: %g\n", dis);
        if (dis < minDis) {
            minDis = dis;
            if (minDis < -1.0 + 0.002) {
                return minDis;
            }
        }
    }

    return minDis;
}

void MyOmFeature::detect_image(unsigned char *data, int w, int h) {
    OmInfo avg,entropy;
    int t;
    get_real_feature(data, w, h,
            &(avg.order), &(entropy.order), NULL,
            avg.len, entropy.len, t);

    avg.bitCode = init_bitCode(avg.order, avg.len);
    entropy.bitCode = init_bitCode(entropy.order, entropy.len);

    features.push_back(avg);
    features.push_back(entropy);
}

char* MyOmFeature::serialize() {}
char* MyOmFeature::unserialize(char *str) {}
void MyOmFeature::print() {
    BaseFeature::print();
    for (int i = 0; i < features.size(); i++) {
        features[i].bitCode->print();
        print_ominfo(features[i]);
    }
}

void MyOmFeature::save(FILE *&pf) {
    save_feature_to_file(pf);    
    for (int i = 0; i < features.size(); i++) {
        save_ominfo(pf, features[i]);
    }
}

void MyOmFeature::read_from_file(FILE *pf) {
    init_feature_from_file(pf);
    OmInfo ominfo;
    while (read_ominfo(pf, ominfo) == 0) {
        features.push_back(ominfo);
    }
//  printf("%s %d features size: %d\n", __FILE__, __LINE__, features.size());
    //read_ominfo(pf);
}


//
// DCT Feature
//
float abs_32f(float x) {
    return x > 0 ? x : -x;
}

DctFeature::DctFeature() {
    data = NULL;
    size = 0;
}

DctFeature::~DctFeature() {
    if (data != NULL)
        delete [] data;
}

float DctFeature::compare(const BaseFeature* rptr) {
    DctFeature* realPtr = (DctFeature*)rptr;
    float dis = 0.0f;
    for (size_t t = 0; t < size; t++) {
        dis += abs_32f(realPtr->data[t] - data[t]);
    }
    return dis;
}

void DctFeature::save(FILE*& pf) {
   save_feature_to_file(pf); 
   fwrite(&size, sizeof(int), 1, pf);
   fwrite(data, sizeof(float), size, pf);
}

void DctFeature::read_from_file(FILE *pf) {
    init_feature_from_file(pf);
    fread(&size, sizeof(int), 1, pf);
    data = new float[size];
    fread(data, sizeof(float), size, pf);
}

void DctFeature::detect_image(unsigned char* data_, int w, int h) {
    IplImage* ptr = yuv2iplImage(data_, w, h);
    data = get_image_dct(ptr, 8, size);
}

void DctFeature::print() {
    BaseFeature::print();
    printf("%d: ", size);
    for (size_t t = 0; t < size; t++) {
        printf("%.4f ", data[t]);
    }
    printf("\n");
}

BaseFeature *get_feautre(unsigned char *data, int w, int h, int type) {
    
}
