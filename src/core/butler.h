#ifndef BUTLER_H
#define BUTLER_H

/**
 * Module to master the resouces of the system；
 *     1. recover the memory index from persisted entry data.
 *     2. merge(compact) the old file data 
*/

#include "util.h"
#include "def.h"

extern string dbpath;
extern dbhash htable;
extern int act_file_fd;
extern int act_file_id;
extern map<int, openinfo> openfds;


int buildMemIndexFromFiles(vector<string> &files, dbhash &index, int clear = 0);

int recoverMemIndex();

/**
 * Just merge the all data files and delete the old files. The opened file descriptors will be closed and 
 * removed from openfds.
 * The memory index and meta info will not update.
 * return 0 if succeed.
 * return -1 if faild. In this case, the data files and opened file descriptors will not change.
*/
int mergeDBfiles();

#endif