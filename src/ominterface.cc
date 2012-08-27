#include "ominterface.hpp"
#include "om.hpp"
#include "query.hpp"
#include "dbconn.hpp"

#include <stdio.h>
#include <string.h>

ImageSet *imgQueryDB;

//#define _FP
#ifdef _FP
FILE *fp;
#endif

int open_db(char *cfg) {
	FILE* pf = fopen(cfg,"r");
	if (pf==NULL) return 1;
	char path[512];
	fscanf(pf,"%s",path);
	fclose(pf);
	if (strlen(path)<=0)
		return 1;

	DBConnection db(path);	
	imgQueryDB = new ImageSet;
    int nFeature;
    Feature **all_feature = db.read_all(&nFeature);
    for (int j = 0; j < nFeature; j++) {
        imgQueryDB->insert(all_feature[j], NULL);
    }

#ifdef _FP
	fp = fopen("./feature.logg", "a");
	if (fp == NULL) printf("open feature log file error!\n");
	fprintf(fp, "everything is ok!\n");
#endif
	printf(">>>>>>>>>>>>>>>>\n");
	printf(">>>>>>>>>>>>>>>>\n");
	printf("open om db ok!\n");
	printf("load %d features\n", nFeature);
	printf("<<<<<<<<<<<<<<<<\n");
	printf("<<<<<<<<<<<<<<<<\n");
	//getchar();
	return 0;
}
int close_db() {
	printf("ok\n");
	if (imgQueryDB != NULL) 
		delete imgQueryDB;
	printf("db ok\n");
#ifdef _FP
	if (fp != NULL)
		fclose(fp);
#endif
	printf("fp ok\n");
	return 0;
}

int query_image(unsigned char *data, int w, int h) {
#ifdef _FP
	unsigned char *my_data = new unsigned char[w * h];
	int i = 0; 
	unsigned char *p = my_data, *q = data;
	while(i++ < w * h) *p++ = *q++;
#endif
	Feature *feature = get_real_feature(data, w, h);
	//int num = 2;
	//int nRes;
	//int *res = imgQueryDB->query(feature, num, nRes);
	//int ans = -1;
	int ans = imgQueryDB->query(feature);
#ifdef _FP
	if (ans != -1) {
  		fprintf(fp, "%d\n", ans);
  		feature->save(fp);
		fflush(fp);
		char name[256];
		sprintf(name, "./om/yuv/%d-%dx%d.kkk", ans, w, h);
		FILE *fp_data = fopen(name, "a");
		fwrite(data, sizeof(char), w * h, fp_data);
		fclose(fp_data);
		
		sprintf(name, "./om/yuv/%d-%dx%d.real_kkk", ans, w, h);
		fp_data = fopen(name, "a");
		fwrite(my_data, sizeof(char), w * h, fp_data);
		fclose(fp_data);
		//printf("get a match image!\n");
		//fprintf(fp, "%s
	}
#endif
	delete feature;
#ifdef _FP
	delete [] my_data;
#endif

	return ans;
}
