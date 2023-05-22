#ifndef CACHE_H
#define CACHE_H

/**
 * Cache for resources
 *     1. maintain the opening file descriptors for reading.
 *     2. direct key-value memory access cache.
 *     3. some replacement algorithm can be discussed.
 * And the OS also has read-cache for file IO, so it is not clear how the 
 * db-internal cache can help. 
*/

#define MAX_OPENFDS 8

/**
 * Get file descriptor by file id.
 * If the target file has been open, it will search the fd from opened file descriptor list.
 * Otherwise, it will open the file and return the new fd. Replacement will happen when the
 * number of the opened files is greater than `MAX_OPENFDS`
*/
int accessFd(int fid); 


#endif