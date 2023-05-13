#ifndef TYPE_H
#define TYPE_H

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <unordered_map>
#include <map>


#define CRC_WIDTH 8
#define CRC_POLY 0x07
#define TIMESTAMP_STR_LEN 10

#define KEY_MAX_LEN 128
#define VALUE_MAX_LEN 256
#define V_KSIZE_LEN 3
#define V_VSIZE_LEN 4


using namespace std;

typedef uint8_t crc_t;

typedef struct openinfo
{
    int fd;
    time_t tstamp;  // 最近一次使用的时间(LRU)
}openinfo;

typedef struct recordentry
{
    crc_t crc;
    time_t tstamp;
    int k_size;
    int v_size;
    string key;
    string value;
}recordentry;


typedef struct hashvalue
{
    int file_id;
    int record_size;
    int offset;
    time_t tstamp;     
}hashvalue;

#endif