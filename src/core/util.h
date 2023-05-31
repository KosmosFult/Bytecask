#ifndef UTIL_H
#define UTIL_H

#include "type.h"

/**
 * The memory index structure is named as dbhash.
 * It can also be replaced by other data structure (rb-tree) and
 * then you need to implement the "hashSet" and "hashGet" functions.
 * All operations on the index must go through the functions in `util.h`.
 *   
*/
typedef std::unordered_map<string, hashvalue> dbhash;

int hashSet(dbhash &ht, string &key, hashvalue &value);

/**
 * Get the index value by the key from memory index.
 * return 0 if found else -1. 
*/
int hashGet(dbhash &ht, string &key, hashvalue &value);
int hashDel(dbhash &ht, string &key);
int hashClear(dbhash &ht);

/**
 * Get the number of key-value pairs
*/
int hashSize(dbhash &ht);

crc_t calCRC(string &body);
string crcToStr(crc_t crc);
crc_t strToCRC(string crcstr);
int validCRC(string &body);

time_t strToTime(string time_str);
string timeToStr(time_t timestamp);
/**
 * Infomation of the system 
*/
int panic(string info);
int printOpenfds();

/**
 * Get the memory use for index. It just simply calculate the key-value
 * size of memory index and ignore the memory use for index data structure.
*/
size_t getIndexMemSize(dbhash &ht);

/**
 * Get the almost really size of the memory index, include the
 * addtionally memory use for index data structure.  
*/
size_t getIndexRsize(dbhash &t);
int printBasicInfo();

string toAbpath(string last_name);
bool fileExists(const char *path);
int directoryExists(const char *path);
string fid2fname(int fid);

/**
 * Can be used for the format as "xxx_`id`.yyy"
*/
int fname2fid(string fname);
string padWithZero(int num, int width);
bool hasExtension(const string &fileName, const string &extension);
vector<string> getDBFiles(const string &directory);
vector<string> getCPFiles(const string &directory);
vector<string> getFilesByExtension(const string &directory, const string &extension);
void sortFilesById(vector<string> &files);
off_t getFileSize(string fname);
void printProgressBar(int progress, int total);
#endif