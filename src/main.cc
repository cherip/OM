#include "om.hpp"
#include "dbconn.hpp"
#include "query.hpp"
#include "ominterface.hpp"
#include "harris.hpp"

#include <cassert>
#include <stdio.h>
//#include <unistd.h>
//#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define _OP_CV

#ifdef _OP_CV
#include "utils.hpp"
#include <cv.h>
#include <highgui.h>
#endif

uint8 *read_yuv(char *path, int &w, int &h) {
	int len = strlen(path);
	int i;
	FILE *pf = fopen(path, "r");
	for (i = len - 1; i >= 0; i--) {
		if (path[i] == '-') break;
	}
	i++;
	path[i + 3] = 0;
	int _w = atoi(&path[i]);
	i = i + 4;
	path[i + 3] = 0;
	int _h = atoi(&path[i]);
	
	//printf("%d %d\n", _w, _h);
	uint8 *data = new uint8[_w * _h];
	fread(data, sizeof(char), _w * _h, pf);

	w = _w;
	h = _h;

	fclose(pf);

	return data;
}

int read_yuv_wh(char *path, int &w, int &h) {
	int len = strlen(path);
	int i,j;
	FILE *pf = fopen(path, "r");
	for (i = len - 1; i >= 0; i--) {
		if (path[i] == '-') break;
	}
  i++;
  for (j = i + 1; path[j] != 'x'; j++);
	path[j] = 0;
	int _w = atoi(&path[i]);
  path[j] = 'x';
  for (i = j + 1; path[i] != '.'; i++);
	path[i] = 0;
	int _h = atoi(&path[j + 1]);
  path[i] = '.';

  w = _w;
  h = _h;

  return 1;
}

uint8* read_264(char *path, int &w, int &h) {
	FILE *pf = fopen(path, "r");
	int _w, _h;
	fread((char *)&_w, sizeof(char), 4, pf);
	fread(&_w, sizeof(int), 1, pf);
	fread(&_h, sizeof(int), 1, pf);

//	printf("%s %d %d\n", path, _w, _h);
	
	uint8 *data = new uint8[_w * _h];
	fread(data, sizeof(char), _w * _h, pf);
    fclose(pf);

	w = _w;
	h = _h;

    return data;
}

uint8 *loadImage(char *path, int &w, int &h) {
	//uint8 *data;
#ifdef _OP_CV
    return load_jpg_image(path, w, h);
#else
	return read_yuv(path, w, h);

#endif

	//return data;
}

BaseFeature *get_library_feature(const char *path) {
//  MyOmFeature *ptrFeature = new MyOmFeature(); 
    DctFeature* ptrFeature = new DctFeature();
    int w, h; 
    uint8 *data = load_jpg_image(path, w, h);
    if (data == NULL)
        return NULL;
    ptrFeature->detect_image(data, w, h);

    delete [] data;

    return ptrFeature;
}

Feature *get_om_feature(char *path) {
	int w, h;
	uint8 *data = loadImage(path, w, h);
//  if (!check_image_entropy(data, w, h)) {
//        printf("||%s\n", path);    
//     //   show_image_from_path(path);
//      delete [] data;
//        return NULL;
//  }

//  delete [] data;
//  return NULL;

    if (data == NULL) return NULL;
    Feature *feature = get_real_feature(data, w, h);
    feature->set_name(path);

    delete [] data;

    return feature;
}

int query_jpg(char *path, ImageSet &imgSet) {
    BaseFeature *ptr = get_library_feature(path);

    BaseFeature **result;
    int nRes;
    nRes = imgSet.query(ptr, &result);
    if (nRes != 0) {
        printf("@@%s match:\n", path);
      //show_query_result(path, result, nRes);
        for (int i = 0; i < nRes; i++) {
            printf("\t%d %s\n", result[i]->get_feature_ID(), result[i]->get_name()); 
        }
    } else {
//      show_image_from_path(path);
        printf("##%s dont't match\n", path);
    }

    delete ptr;
    delete [] result;
}

int query_yuv(char *path, ImageSet &imgSet) {
	int w, h;
  FILE *pf = fopen(path, "r");

  if (pf == NULL) {
    printf("open yuv error!\n");
    return 0;
  }

  read_yuv_wh(path, w, h);
  //printf("path:%s %d %d\n", path, w, h);
  uint8 *data = new uint8[w * h * 3 / 2];
  
  int flag = 0;
  int size = 0;
  int idx = 0;
  while (size = fread(data, sizeof(char), w * h * 3 / 2, pf)) {
    if (size != w * h * 3 / 2) break;
//  Feature *feature = get_real_feature(data, w, h);
//  feature->set_name(path);
//
//  int nRes = imgSet.query(feature);
//  if (nRes != -1) {
//  printf("-> %s ", path);
//  printf("%d \n", imgSet.get_id(nRes)->get_feature_ID());
//  printf("%d \n", nRes);

//    printf("%.3f\n", feature->compare(imgSet.get_id(nRes)));
//    printf("%s\n", imgSet.get_id(nRes)->get_name());
//    show_yuv_and_path(data, w, h, imgSet.get_id(nRes)->get_name());
//    show_yuv(data, w, h);
//    show_image_from_path(imgSet.get_id(nRes)->get_name());
//    flag++;
//  } else {
//      printf("## %s\n", path);
//            show_yuv(data, w, h);
//  }
//  delete feature;
//  continue;

    BaseFeature *ptr = new MyOmFeature();
    ptr->detect_image(data, w, h);

    BaseFeature **result;
    int nRes;
    nRes = imgSet.query(ptr, &result);
    if (nRes != 0) {
        printf("%s %d match:\n", path, idx++);
        show_query_result(data, w, h, result, nRes);
//        show_yuv(data, w, h);
        for (int i = 0; i < nRes; i++) {
            printf("\t%d %s\n", result[i]->get_feature_ID(), result[i]->get_name()); 
        //    show_image_from_path(result[i]->get_name());
        }
    } else {
        printf("## %s %d dont't match\n", path, idx++);
        show_yuv(data, w, h); 
    }

    delete ptr;
    delete [] result;
  }

    delete [] data;
    fclose(pf);

  if (!flag) {
    //printf("%s\n", path);
    //show_image_from_path(path);
  }
  
  return flag;
}

int compare_two_pic(char *path, char *pathB) {
    int w, h;
  FILE *pf = fopen(path, "r");

  if (pf == NULL) {
    printf("open yuv error!\n");
    return 0;
  }

  read_yuv_wh(path, w, h);
  uint8 *data = new uint8[w * h * 3 / 2];
  int wb, hb;
  uint8 *dataB = load_jpg_image(pathB, wb, hb);
  //Feature *featureB = get_real_feature(dataB, wb, hb); 
  BaseFeature *featureB = new MyOmFeature();
  featureB->detect_image(dataB, wb, hb);
  
  int flag = 0;
  int size = 0;
  while (size = fread(data, sizeof(char), w * h * 3 / 2, pf)) {
    if (size != w * h * 3 / 2) break;
//  Feature *feature = get_real_feature(data, w, h);
//  feature->set_name(path);
    BaseFeature *feature = new MyOmFeature();
    feature->detect_image(data, w, h);

    printf("%.3f\n", feature->compare(featureB));
    featureB->print();
    feature->print();

//  show_image_from_path(featureB->get_name());
//  show_yuv(data, w, h);
  
    delete feature;
  }

  show_image_from_path(pathB);
  show_yuv(data, w, h);
}

/*void get_om_single(char *_pa, char *_pb, int type) {
//  char pa[] = "/mnt/share/src/D-2469160338965168503-flv-320x240.yuv-55.jpg";
//  char pb[] = "/mnt/share/src/D-2542384387432238353-flv-640x352.yuv-98.jpg";

    IplImage *img1 = cvLoadImage(_pa);
    IplImage *img2 = cvLoadImage(_pb);

    Feature *fa = get_om_feature(_pa);
    printf("get feature of image 1\n");
    Feature *fb = get_om_feature(_pb);
    printf("get feature of image 2\n");

    printf("%f\n", fa->compare(fb));

    show_image(img1, "img1");
    show_image(img2, "img2");
}*/

const int LIB_NUM = 1;
const char *lib_path[3] = {"/mnt/share/src/",
                        "/mnt/db/2/",
                        "/mnt/db/3/"};
//const char *lib_path[3] = {"/mnt/db/badimage/"};
//const char path[] = "../imgDBSmall/"; 
//const char * const path = "test264/";
char path[] = "/mnt/share/query/";
const char* feature_lib = "./dctFeature/";

void test_my() {
	open_db("om.cfg");

//	ImageDB imgDBquery(path);

	close_db();
}

void test_query() {
    DBConnection db(feature_lib);
    ImageSet imgSet;

    int nFeature;
    BaseFeature **all_feature = db.read_myOmFeature(&nFeature);
    for (int j = 0; j < nFeature; j++) {
//        all_feature[j]->print();
        imgSet.insert(all_feature[j]);
    }
    
    ImageDB imgDBquery(path);
    while (true) {
        MyImage *ptr = imgDBquery.get_image();
        if (ptr == NULL) break;
//        query_image(ptr->pName, imgSet);
        query_jpg(ptr->pName, imgSet);
//      query_yuv(ptr->pName, imgSet);
    }
}

void test_om(int mn) {
    int i = 0;
    int start, end;
    if (mn != 0) {
        ImageSet imgSet;
        start = clock();
        for (int j = 0; i < mn && j < LIB_NUM; j++) {
            ImageDB imgDB(lib_path[j]);

            while (true) {
                MyImage *ptr = imgDB.get_image();
                if (ptr == NULL) break;
                printf("%s\n", ptr->pName);
                
                BaseFeature *feature = get_library_feature(ptr->pName);
//                continue;
                if (feature != NULL) {
                    feature->set_name(ptr->pName);
                    feature->set_feature_ID(i);
                    imgSet.insert(feature);

                    if (i == 22620)
                        feature->print();

                    i++;
                }
                //test_harris(ptr->pName);
//              surf_opencv(ptr->pName);
//              harris_opencv(ptr->pName);
//              continue;
//                Feature *feature = get_om_feature(ptr->pName);
//              if (feature != NULL) {
//                  feature->set_feature_ID(i);
//                  imgSet.insert(feature, ptr);
//                  printf("insert ok! %d\n", i);
//                  i++;
//              }

                if (i >= mn) break;
            }
        }
        end = clock(); 
        printf("load image library costs %.4lfs, total %d images\n", 
                double(end - start) / CLOCKS_PER_SEC, i); 

        DBConnection dbconn(feature_lib);        
        imgSet.serialize(&dbconn);
    } else {

        test_query();
        exit(0);

        DBConnection dbconn("./feature1/");
        ImageSet imgSet;
        //ImageDB imgDBquery("/mnt/share/badimage/");
        ImageDB imgDBquery(path);
        //ImageDB imgDBquery;

        int nFeature;
//      BaseFeature **all_feature = dbconn.read_myOmFeature(&nFeature);
        Feature **all_feature = dbconn.read_all(&nFeature);
        for (int j = 0; j < nFeature; j++) {
//          all_feature[j]->print();
            imgSet.insert(all_feature[j], NULL);
        }

        //exit(0);

        int nMatch = 0;
        int num = 2, nRes = 0;
        start = clock();
        int *res;
        while (true) {
            MyImage *ptr = imgDBquery.get_image();
            if (ptr == NULL) break;
            nMatch += query_yuv(ptr->pName, imgSet);
//          Feature *feature = get_om_feature(ptr->pName);
//          nRes = imgSet.query(feature);
//          if (nRes != -1) {
//              printf("%s\n", ptr->pName);
//              printf("%d ", nRes);
//              nMatch++;

//              printf("%.3f\n", feature->compare(imgSet.get_id(nRes)));
//          } else {
//              printf("## %s\n", ptr->pName);
//          }
            i++;

//            delete feature;
            //break;
        }
        end = clock();
        printf("query all images costs %.4lfs, total %d images, find %d match\n", 
                double(end - start) / CLOCKS_PER_SEC, i, nMatch); 

    }
}


int test_single_feature(char *path, int id) {	
//      DBConnection dbconn("./feature/");
//      ImageSet imgSet;

//      int nFeature;
//      Feature **all_feature = dbconn.read_all(&nFeature);
//      for (int j = 0; j < nFeature; j++) {
//          imgSet.insert(all_feature[j], NULL);
//      }


//  test_harris(path);
//  return 0;

  FILE *pf = fopen(path, "r");

  if (pf == NULL) {
    printf("open yuv error!\n");
    return -1;
  }

  int w, h;

  read_yuv_wh(path, w, h);
  //printf("path:%s %d %d\n", path, w, h);
  uint8 *data = new uint8[w * h * 3 / 2];

  char tmpName[128];
  sprintf(tmpName, "./newFeature/%d.smp", id);
//  Feature *pMatch = read_feature_from_file(tmpName);
    BaseFeature *pMatch = new MyOmFeature();
    FILE *pff = fopen(tmpName, "r");
    pMatch->read_from_file(pff);
  
  int flag = 0;
  int size = 0;
  while (size = fread(data, sizeof(char), w * h * 3 / 2, pf)) {
    if (size != w * h * 3 / 2) break;
//  Feature *feature = get_real_feature(data, w, h);
//  feature->set_name(path);
    BaseFeature *feature = new MyOmFeature();
    feature->detect_image(data, w, h);
    feature->set_name(path);
  
	int resIdx = id;
	
	if (resIdx == -1) {
		printf("error!\n");
		return -1;
	}
    printf("%d ", resIdx);

//  float dis = feature->compare(imgSet.get_id(resIdx));
    float dis = feature->compare(pMatch);
    printf("%.3f\n", dis);

//  if (dis < 0.599) {
        feature->print();
        pMatch->print();
//      imgSet.get_id(resIdx)->print();
//      printf("%s\n", imgSet.get_id(resIdx)->get_name());
        show_yuv(data, w, h);
        show_image_from_path(pMatch->get_name());
//      show_image_from_path(imgSet.get_id(resIdx)->get_name());
//  }
  }
//	Feature *pFeature = get_om_feature(path);

	//int resIdx = imgSet.query(pFeature);
//	int resIdx = id;
	
//	if (resIdx == -1) {
//		printf("error!\n");
//		return -1;
//	}
//              printf("%d ", resIdx);

//              printf("%.3f\n", pFeature->compare(imgSet.get_id(resIdx)));

//	pFeature->print();
//	imgSet.get_id(resIdx)->print();
//	printf("%s\n",//imgSet.get_id(resIdx)->get_name());
//  show_image_from_path(imgSet.get_id(resIdx)->get_name());
	return 0;
}

bool compare_two_image(const char* pa, const char* pb) {
    BaseFeature* featureA = get_library_feature(pa);
    BaseFeature* featureB = get_library_feature(pb);


  printf("%.3f\n", featureA->compare(featureB));
  show_image_from_path(pa);
  show_image_from_path(pb);
}

int main(int argc, char *argv[]) {
    printf("argc %d\n", argc);
//	test_my();
    if (argc == 2) {
        //printf("%d\n", atoi(argv[1]));
       	test_om(atoi(argv[1]));
    } else if (argc == 4) {
    //    get_om_single(argv[1], argv[2], atoi(argv[3]));
	test_single_feature(argv[1], atoi(argv[2]));
    } else if (argc == 3) {
    //    Feature *pf = get_om_feature(argv[1]);
	//pf->print();
        compare_two_image(argv[1], argv[2]);
    }
    return 0;
}
