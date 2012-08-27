#ifndef _OMINTERFACE_H_
#define _OMINTERFACE_H_
extern "C"
{
int open_db(char *);
int close_db();
int query_image(unsigned char *data, int w, int h);
};
#endif
