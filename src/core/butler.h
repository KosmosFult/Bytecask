#ifndef BUTLER_H
#define BUTLER_H

/**
 * Module to master the resouces of the system；
 *     1. recover the memory index from persisted entry data.
 *     2. merge(compact) the old file data 
*/

#include "util.h"

int buildMemIndexByFiles(vector<string> &files, dbhash &index, int clear = 0);

int recoverMemIndex();

#endif