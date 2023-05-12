#ifndef UTIL_H
#define UTIL_H
#include <unordered_map>
#include <time.h>
#include <string>
using namespace std;

typedef struct hashvalue
{
    int file_id;
    int record_size;
    int offset;
    time_t tstamp;     
}hashvalue;

typedef std::unordered_map<string, hashvalue> dbhash; 

int hashSet(dbhash &ht, string &key, hashvalue &value);
int hashGet(dbhash &ht, string &key, string &rvalue);
string calCRC(string &body);

#endif