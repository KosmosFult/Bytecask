#ifndef BYTECASK_H
#define BYTECASK_H

#include "type.h"
#include "def.h"

int dbinit(int argc, char *argv[]);
int set(string &key, string &value, valuetype type = STRING);
string get(string &key);

/**
 * expire the key-value.
 *   return 0 when succeed.
 *   return -1 when faild for IO of MemIndex.
 *   return -2 when key not found.
 */
int expire(string &key);
int listKeys();
int dbsync();
dbinfo getDBinfo();
int merge();
int dbclose();

#endif