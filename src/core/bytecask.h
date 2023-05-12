#ifndef BYTECASK_H
#define BYTECASK_H

#include <string>
#include "util.h"


#define CRC_WIDTH 8
#define CRC_POLY 0x07
#define FILE_SIZE_TRIG 4096


typedef struct openinfo
{
    int fd;
    time_t tstamp;
}openinfo;

typedef struct record
{
    uint8_t crc;
    time_t tstamp;
    int k_size;
    int v_size;
    string key;
    string value;
}record;


int dbinit(string dbpath);
int set(string &key, string &value);
string get(string &key);
int erase(string &key);
int dbclose();


#endif