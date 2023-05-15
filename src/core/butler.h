#ifndef BUTLER_H
#define BUTLER_H

/**
 * Module to master the resouces of the system；
 *     1. recover the memory index from persisted entry data.
 *     2. merge the old file data 
*/

#include "util.h"

int recoverMemIndex(string dbroot_path);

#endif