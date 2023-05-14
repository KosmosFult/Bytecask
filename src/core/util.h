#ifndef UTIL_H
#define UTIL_H

#include "type.h"

typedef std::unordered_map<string, hashvalue> dbhash;

int hashSet(dbhash &ht, string &key, hashvalue &value);
int hashGet(dbhash &ht, string &key, string &rvalue);
crc_t calCRC(string &body);
string crcToStr(crc_t crc);
crc_t strToCRC(string crcstr);
int validCRC(string &body);
int panic(string info);

#endif