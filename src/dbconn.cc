#include "dbconn.hpp"
#include <stdio.h>

#ifdef MYSQL
const char *DBConnection::default_url = "localhost";
const char *DBConnection::default_name = "root";
const char *DBConnection::default_pwd = "qwe123";
const char *DBConnection::default_dbname = "ImgFeature";

DBConnection::DBConnection(const char *url, const char *name,
        const char *pwd, const char *dbname) {
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, url, name, pwd, dbname, 0, NULL, 0)) {
        printf("fail to connect: Error is %s\n", mysql_error(&mysql));
    }
}

DBConnection::~DBConnection() {
    mysql_close(&mysql);
}

Feature **DBConnection::read_all(int *nFeature) {
    static char cmd[256];

    sprintf(cmd, "select * from image");

    mysql_query(&mysql, cmd);
    
    MYSQL_RES *result;
    MYSQL_ROW row;
    result = mysql_store_result(&mysql);

    int num = mysql_num_rows(result);
    
    printf("get %d image from the db\n", num);

    Feature **res = new Feature*[num];
    int idx = 0;
    while (row = mysql_fetch_row(result)) {
        OmFeature *tmpFeature = new OmFeature();
        tmpFeature->set_name(row[1]);
        tmpFeature->unserialize(row[2]);
        res[idx++] = tmpFeature;
    }

    *nFeature = num;
    return res;
}

void DBConnection::insert(Feature *pFeature) {
    static char cmd[256];
    sprintf(cmd, "insert image (name, feature) value ('%s', '%s');",
            pFeature->get_name(), pFeature->serialize());
    printf("%s\n", cmd);
    mysql_query(&mysql, cmd);
}
#else

const char *DBConnection::default_path = "./feature/";

DBConnection::DBConnection(const char *path) {
    this->path = path;
    featureDB = new FeatureDB(path);
}

DBConnection::~DBConnection() {
    delete featureDB;
}

Feature **DBConnection::read_all(int *nFeature) {
    char *ptrPath;
    int num = featureDB->get_file_num();
    int idx = 0;
    Feature **res = new Feature*[num];
    printf("exist %d files\n", num);

    char *dataBuff = new char[1024];
    char imageName[256];

    while (true) {
        ptrPath = featureDB->get_file_path();
        if (ptrPath == NULL) {
            printf("not more files\n");
            break;
        }

        FILE *pFile = fopen(ptrPath, "r");
        if (pFile == NULL) {
            continue;
        }

        OmFeature *tmpFeature = new OmFeature();

        fscanf(pFile, "%d\n", &(tmpFeature->valid));
        tmpFeature->set_feature_ID(tmpFeature->valid);

        if (fgets(imageName, 256, pFile)) {
            imageName[strlen(imageName) - 1] = '\0';
            tmpFeature->set_name(imageName);
        } else {
            tmpFeature->set_name("not name");
        }

        fgets(dataBuff, 1024, pFile);
        //tmpFeature->set_name("not");
        tmpFeature->unserialize(dataBuff);

        res[idx++] = tmpFeature;
        fclose(pFile);
    }

    printf("load %d features\n", idx);
    *nFeature = idx;

    delete [] dataBuff;

    return res;
}

void DBConnection::improve_insert(Feature *pFeature) {
    char fileName[256];
    sprintf(fileName, "%s%d.smp", featureDB->get_root_path(), pFeature->get_feature_ID());

    printf("%s\n", fileName);
    FILE *pFile = fopen(fileName, "w");
    if (pFile == NULL) {
        printf("open save file error!\n"); 
        return ;
    }
    char *data = pFeature->serialize();
    //printf("try to write!\n%s\n%d\n", data, (int)strlen(data));
    //fwrite(data, sizeof(char), strlen(data), pFile);
    //printf("%s\n", pFeature->get_name());
    fprintf(pFile, "%d\n", pFeature->get_feature_ID());
    fprintf(pFile, "%s\n", pFeature->get_name());
    fprintf(pFile, "%s\n", data);

    featureDB->increase_file_num();
    fclose(pFile);
} 

BaseFeature **DBConnection::read_myOmFeature(int *nFeature) {
    char *ptrPath;
    int num = featureDB->get_file_num();
    int idx = 0;
    BaseFeature **res = new BaseFeature*[num];
    printf("exist %d files\n", num);

    while (true) {
        ptrPath = featureDB->get_file_path();
        if (ptrPath == NULL) {
            printf("not more files\n");
            break;
        }

        FILE *pFile = fopen(ptrPath, "r");
        if (pFile == NULL) 
            continue;

//      MyOmFeature *tmp = new MyOmFeature();
        DctFeature* tmp = new DctFeature();
        tmp->read_from_file(pFile);
        res[idx++] = tmp;

        fclose(pFile);
    }
    *nFeature = idx; 

    return res;
}

void DBConnection::insert(BaseFeature *pFeature) {
    char fileName[256];
    sprintf(fileName, "%s%d.smp", featureDB->get_root_path(), pFeature->get_feature_ID());
    //printf("%s\n", fileName);
    FILE *pf = fopen(fileName, "w");
    pFeature->save(pf);
    fclose(pf);
}

#endif
