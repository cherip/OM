#ifndef _QUERY_HPP_
#define _QUERY_HPP_

//#include "file.hpp"
#include "feature.hpp"
#include "dbconn.hpp"

struct SetUnit {
//  char fea[256]; 
    MyImage *pImg;
	//char *pImg;
    int *feat;
    Feature *pFeat;
    SetUnit *next;
    BaseFeature *ptrNew;
};

class ImageSet {
public:
    ImageSet();
    ~ImageSet();
    ImageSet(DBConnection &db);

    void insert(int *, int, MyImage *);
    void show();
    void insert(Feature *, MyImage *);
    void insert(BaseFeature *);
    int *query(Feature *, int num, int &);
    int query(Feature *);
    int query(BaseFeature *, BaseFeature ***);
    void serialize(DBConnection *);
    char *print(int id);
    Feature *get_id(int id);

private:
    int convert(int *, int, char *);
    float compare(int *a, int *b, int);

private:
    SetUnit **pTable;
    int *num;
    int idx;
};

#endif
