#include "query.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <vector>
#include <algorithm>

#define TABLE_LEN 200011
#define THRESHOLD 0.5999
#define MY_OM_THREAS 0.5999

#include "dbconn.hpp"

struct query_struct {
    void *ptr;
    float dis;
};

bool cmp(const query_struct &a, const query_struct &b) {
    return a.dis > b.dis;
}

ImageSet::ImageSet() {
    pTable = new SetUnit*[TABLE_LEN];
    num = new int[TABLE_LEN];
    for (int i = 0; i < TABLE_LEN; i++) {
        pTable[i] = NULL;
        num[i] = 0;
    }
    idx = 0;
}

ImageSet::ImageSet(DBConnection &db) {
    ImageSet();

    int nFeature;
    Feature **all_feature = db.read_all(&nFeature);
    for (int j = 0; j < nFeature; j++) {
//        all_feature[j]->print();
//	printf("%d\n", j);
        insert(all_feature[j], NULL);
    }
}

ImageSet::~ImageSet() {
    for (int i = 0; i < idx; i++) {
        SetUnit *ptr = pTable[i];
         
//	printf("%d \n", i);
        delete ptr->pFeat;
        delete ptr;
    }

    delete [] pTable;
    delete [] num;
}

void ImageSet::insert(Feature *pFea, MyImage *pImg) {
    SetUnit *pUnit = new SetUnit(); 
    pUnit->pImg = pImg;
    pUnit->pFeat = pFea;


    int i = idx++;
    assert(i >= 0 && i < TABLE_LEN);

//  if (pTable[i] == NULL) {
    pTable[i] = pUnit;
//  }

    num[i]++;
}

void ImageSet::insert(BaseFeature *ptr) {
    SetUnit *pUnit = new SetUnit();
    pUnit->ptrNew = ptr;
    pUnit->pFeat = NULL;
    int i = idx++;
    assert(i >= 0 && i < TABLE_LEN);
    pTable[i] = pUnit;
    num[i]++;
}

int *ImageSet::query(Feature *pQuery, int num, int &nRes) {
    int *tmp = new int[num]; 
    int index = 0;
    float max = -1.0;

    for (int i = 0; i < idx; i++) {
        if (pTable[i]->pFeat->get_valid() == 0) {
            printf("unvalid feature in db\n");
            continue;
        }
        float dis = pTable[i]->pFeat->compare(pQuery); 
        if (dis > max) {
            max = dis;
            index = 1;
            tmp[0] = i;
        } else if (dis == max) {
            if (index < num)
                tmp[index++] = i;
        }
    } 

    if (max < THRESHOLD) {
        nRes = 0;
    } else {
        nRes = index;
    }
    return tmp;
}

int ImageSet::query(Feature *pQuery) {
    int index = -1;
    float max = -1.0;

    for (int i = 0; i < idx; i++) {
	    float dis = pQuery->compare(pTable[i]->pFeat);
        if (dis > max) {
            max = dis;
            index = i;
        } 
    }
    if (max < THRESHOLD) {
        return index = -1;
    } else {
        //return pTable[index]->pFeat->get_feature_ID();
        return index;
    }    
}

int ImageSet::query(BaseFeature *pQuery, BaseFeature ***pResult) {
    std::vector<query_struct> result;
    for (int i = 0; i < idx; i++) {
        float dis = pQuery->compare(pTable[i]->ptrNew);
//      printf("imgset query dis : %g\n", dis);
//      if (dis > MY_OM_THREAS) {   
        if (dis < 50.0) {
            query_struct t;
            t.dis = dis;
            t.ptr = pTable[i]->ptrNew;
            result.push_back(t);
        }
    }

    if (result.size() == 0) {
        *pResult = NULL;
        return 0;
    } else {
        sort(result.begin(), result.end(), cmp); 
        
        int result_size = result.size() > 10 ? 10 : result.size();
        BaseFeature **tmp = new BaseFeature*[result_size];
        for (int i = 0; i < result_size; i++) {
            tmp[i] = (BaseFeature *)result[i].ptr;
        }

        *pResult = tmp;
        return result_size;
    }
}

void ImageSet::serialize(DBConnection *dbconn) {
    for (int i = 0; i < idx; i++) {
    //    pTable[i]->pFeat->print();
//        dbconn->insert(pTable[i]->pFeat); 
        if (pTable[i]->pFeat != NULL)
            dbconn->improve_insert(pTable[i]->pFeat);
        if (pTable[i]->ptrNew != NULL) 
            dbconn->insert(pTable[i]->ptrNew);
    }
}

char *ImageSet::print(int id) {
    assert(id >= 0 && id < idx);
    char *name = pTable[id]->pFeat->get_name();
    printf("%s\n", name);
    return name;
}

Feature *ImageSet::get_id(int id) {
    if (id < 0 || id >= idx)
        return NULL;

    return pTable[id]->pFeat;
}
