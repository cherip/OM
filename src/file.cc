#include "file.hpp"
#include <stdio.h>  
#include <stdlib.h>
//#include <highgui.h>

FileReader::FileReader(const char *path) {
    this->path = path;
    open_db(path);
}

FileReader::FileReader(const char *path,
        int (*p)(const dirent *)) {
    this->path = path;
    this->select_fun = p;
    open_db(path);
}

FileReader::~FileReader() {
    for (int i = 0; i < num; i++) {
        free(namelist[i]);
    }
//    printf("delete fileReader ok\n");
}

int FileReader::open_db(const char *path) {
    num = scandir(path, &namelist, select_fun, alphasort);
    if (num < 0) {
        printf("open file dir failure!\n");
        num = 0;
        return 1;
    }
    addIdx = 0;
    idx = 0;
    return 0;
}

char *FileReader::get_file_path() {
    static char fullname[216];
    if (idx >= num) {
        return NULL;
    }
        
    sprintf(fullname, "%s%s", path, namelist[idx]->d_name);
    idx++;

    return fullname;
}

int FileReader::get_file_num() {
    return num;
}

int FileReader::get_last_index() {
    return addIdx;
}

void FileReader::increase_file_num() {
    addIdx++;
}

const char *FileReader::get_root_path() {
    return path;
}

int select_name(const dirent *dp) {
	if (strcmp(dp->d_name, ".") == 0 ||
	strcmp(dp->d_name, "..") == 0)
		return 0;

	return 1;

    if (strstr(dp->d_name, ".jpg") == 0) {
        return 0;
    }
    return 1;
}

//const char *ImageDB::imgDBPath = "/mnt/share/modify/";
//const char *ImageDB::imgDBPath = "/mnt/share/full/";
//const char *ImageDB::imgDBPath = "/mnt/share/image/";
const char *ImageDB::imgDBPath = "/mnt/share/src/";

ImageDB::ImageDB(const char *path):
    FileReader(path, select_name) {
}


MyImage *ImageDB::get_image() {
    char *ptr = get_file_path();
    if (ptr == NULL) 
        return NULL;
    MyImage *pMyImg = new MyImage(NULL, ptr);
    return pMyImg;
}

int select_feature(const dirent *dp) {
    if (strstr(dp->d_name, ".smp") == NULL) {
        return 0;
    }
    return 1;
}

const char *FeatureDB::default_path = "feature/";
FeatureDB::FeatureDB(const char *path):
    FileReader(path, select_feature) {}
