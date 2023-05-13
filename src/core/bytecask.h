#ifndef BYTECASK_H
#define BYTECASK_H

#include "type.h"

#define FILE_SIZE_TRIG 4096


int dbinit(const char* dbpath);
int set(string &key, string &value);
string get(string &key);
int erase(string &key);
int dbclose();


#endif