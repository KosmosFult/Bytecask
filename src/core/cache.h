#ifndef CACHE_H
#define CACHE_H

/**
 * Cache for resources
 *     1. maintain the opening file descriptors for reading.
 *     2. direct key-value memory access cache.
 *     3. some replacement algorithm can be discussed.
*/

#define MAX_OPENFDS 8

int accessFd(int fid); 


#endif