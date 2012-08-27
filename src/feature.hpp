#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <stdio.h>
#include <vector>

struct BitCode {
    unsigned int *bits;
    int len;
    int bitsLen;

    BitCode(int _len) {
        len = _len;
        bitsLen = (len + 31) / 32;
        bits = new unsigned int[bitsLen];
        for (int i = 0; i < bitsLen; i++)
            bits[i] = 0;
    }

    ~BitCode() {
	delete [] bits;
    } 

    void set_bit(int offset) {
        int b = offset / 32;
        int l = offset % 32;

        bits[b]  = bits[b] | (1 << l);
    }

    int compare(BitCode *bitCode) {
        int ans = 0;
        for (int i = 0; i < bitsLen; i++) {
            int tmp = bits[i] ^ bitCode->bits[i];
            while (tmp) {
                ans++;
                tmp = tmp & (tmp - 1);
            }
        }

        return ans;
    }
    
    int get_bit(int offset) {
        int b = offset / 32;
        int l = offset % 32;
       
        return (bits[b] >> l) & 1;
    }

    void print() {
        for (int i = 0; i < len; i++) {
            if (get_bit(i) == 0) 
                printf("0");
            else 
                printf("1");
        }
        printf("\n");
    }
};

class Feature {
public:
    virtual ~Feature() {};

    virtual float compare(const Feature *rptr) = 0;
    float inter_compare(const int *, const int *, int);

    virtual char *serialize() = 0;
    virtual char *unserialize(char *str) = 0;
    virtual char *get_name() = 0;
    virtual void set_name(char *pname) = 0;
    virtual void print() = 0;
    virtual int get_valid() = 0;
    virtual void save(FILE *&fp) = 0;
    virtual int get_feature_ID() const;
    virtual void set_feature_ID(int id);
    virtual void read_from_file(FILE *pf) = 0;
    
private:
    int featureID;
};

class OmFeature : public Feature {
public:
    OmFeature(int *featureA, int *featureB, int len);
    OmFeature(int *featureA, int *featureB, int len,
            char *pname);
    OmFeature(char *str);
    OmFeature();
    ~OmFeature();

    float compare(const Feature *rptr);
    char *serialize();
    char *unserialize(char *str);
    char *get_name();
    void set_name(char *pname);
    void print();
    int get_valid();
    int valid;
	void save(FILE *&pf);
    void read_from_file(FILE *pf);

    void inter_handle();
private:
    int *a;
    int *b;
    int _len;

    char pName[128];

    BitCode *bitCode;
};

Feature *read_feature_from_file(const char *path);


class BaseFeature {
public:
    BaseFeature();
    virtual ~BaseFeature();
    virtual char *get_name();
    virtual char *set_name(const char *name);
    virtual int get_feature_ID() const;
    virtual void set_feature_ID(int id);
    virtual void init_feature_from_file(FILE *&pf);
    virtual void save_feature_to_file(FILE *&pf);
    virtual void print();

    virtual float compare(const BaseFeature *rptr) = 0;
    virtual char *serialize() = 0;
    virtual char *unserialize(char *str) = 0;
    virtual void save(FILE *&fp) = 0;
    virtual void read_from_file(FILE *pf) = 0;
    virtual void detect_image(unsigned char *data, int w, int h) = 0;

private:
    int featureID;
    char name[128];
};

struct OmInfo {
    int *order;
    int len;
    BitCode *bitCode;
};

class MyOmFeature: public BaseFeature {
public:
    MyOmFeature();
    ~MyOmFeature();

    float compare(const BaseFeature *rptr);
    char *serialize();
    char *unserialize(char *str);
    void print();
    void save(FILE *&fp);
    void read_from_file(FILE *pf);
    void detect_image(unsigned char *data, int w, int h);

private:
    std::vector<OmInfo> features;    
};

class DctFeature: public BaseFeature {
public:
    DctFeature();
    ~DctFeature();

    float compare(const BaseFeature *rptr);
    void print();
    void save(FILE *&fp);
    void read_from_file(FILE *pf);
    void detect_image(unsigned char *data, int w, int h);

private:
    char *serialize() {}
    char *unserialize(char *str) {}
    float *data;
    int size;
};

BaseFeature *get_feautre(unsigned char *data, int w, int h, int type);

#endif
