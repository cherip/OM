#ifndef _DBCONN_H_
#define _DBCONN_H_

#include "feature.hpp"

#ifdef MYSQL
#include <mysql.h>
class DBConnection {
public:
    DBConnection(const char *url = default_url, 
            const char *name = default_name,
            const char *pwd = default_pwd, 
            const char *dbname = default_dbname);
    ~DBConnection();

    Feature **read_all(int *nFeature);
    void insert(Feature *pFeature);

private:
    static const char *default_url;
    static const char *default_name;
    static const char *default_pwd;
    static const char *default_dbname;

private:
    MYSQL mysql;
};
#else
#include "file.hpp"

class DBConnection {
public:
    DBConnection(const char *path = default_path);
    ~DBConnection();

    Feature **read_all(int *nFeature);
    void insert(Feature *pFeature);
    void improve_insert(Feature *);

    BaseFeature **read_myOmFeature(int *nFeature);
    void insert(BaseFeature *pFeature);

private:
    static const char *default_path;
private:
    const char *path;
    FeatureDB *featureDB;
};
#endif

#endif
