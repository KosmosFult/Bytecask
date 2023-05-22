#ifndef UTIL_H
#define UTIL_H

#include "type.h"

/**
 * The memory index structure is named as dbhash.
 * It can also be replaced by other data structure (rb-tree, LSM) and
 * then need to implement the "hashSet" and "hashGet" functions.
 *   
*/
typedef std::unordered_map<string, hashvalue> dbhash;
int hashSet(dbhash &ht, string &key, hashvalue &value);
int hashGet(dbhash &ht, string &key, hashvalue &value);
int hashDel(dbhash &ht, string &key);
int hashClear(dbhash &ht);

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
int getIndexMemSize();
int printBasicInfo();


bool fileExists(const char *path);
int directoryExists(const char *path);
string fid2fname(int fid);
int fname2fid(string fname);
string padWithZero(int num, int width);
bool hasExtension(const string &fileName, const string &extension);
vector<string> getDBFiles(const string &directory);
off_t getFileSize(string fname);
#endif