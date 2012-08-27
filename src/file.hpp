#ifndef _FILE_HPP_
#define _FILE_HPP_

//#include <cv.h>
#ifdef _WIN64
#include <Windows.h>
#include <io.h> 
#else
#include <dirent.h>
#include <sys/stat.h>
#endif
#include <string.h>


class MyImage {
public:
    //xxIplImage *pImage;
    char *pImage;
    char *pName;

    MyImage(char *img, char *str) {
        pImage = img;
        int len = strlen(str); 
        pName = new char[len + 1];
        strncpy(pName, str, len);
        pName[len] = 0;
    }

    ~MyImage() {
//      cvReleaseImage(&pImage);
        delete [] pName;
    }
};

class FileReader {
public:
    FileReader(const char *path);
    FileReader(const char *, int (*p)(const dirent *));
    virtual ~FileReader();

    char *get_file_path();
    int get_file_num();
    void increase_file_num();
    int get_last_index();
    const char *get_root_path();

private:
    int open_db(const char *path);
    //virtual int real_select(const dirent *dp) = 0;

private:
    struct dirent **namelist;
    const char *path;
    int num;
    int addIdx;
    int idx;
    int (*select_fun)(const dirent *dp);
};

class ImageDB: public FileReader{
public:
    ImageDB(const char *path = imgDBPath);

    MyImage *get_image();

    static const char *imgDBPath;
private:
    //int select_name(const dirent *dp);
};

class FeatureDB: public FileReader {
public:
    FeatureDB(const char *path = default_path);
    static const char *default_path;
};



#endif
