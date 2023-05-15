#ifndef TYPE_H
#define TYPE_H

/**
 * Types of the database system
*/


#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <unordered_map>
#include <map>


#define CRC_WIDTH 8
#define CRC_POLY 0x07

#define TIMESTAMP_STR_LEN 10
#define VALUETYPE_SIZE sizeof(valuetype)
#define EXPIRED_SIZE sizeof(recordentry::expired)
#define KEY_MAX_LEN 128
#define VALUE_MAX_LEN 256
#define V_KSIZE_LEN 3
#define V_VSIZE_LEN 4
#define ENTRY_HEADER_SIZE (CRC_WIDTH/8) + TIMESTAMP_STR_LEN + V_KSIZE_LEN + \
                            V_VSIZE_LEN + VALUETYPE_SIZE + EXPIRED_SIZE


using namespace std;

typedef uint8_t crc_t;


enum valuetype : uint8_t{
    STRING,
    INT,
    FLOAT,
    LIST
};

typedef struct openinfo
{
    int fd;
    time_t tstamp;  // recent used time(for LRU)
}openinfo;

/**
 * When persisting, members of recordentry will be presented
 * as ascii char except valuetype and crc. There may be some
 * problems because of Big-endian/Little-endian
*/
typedef struct recordentry
{
    // header
    crc_t crc;
    time_t tstamp;
    int k_size;
    int v_size;
    valuetype type;    // for persisting, use byte not ascii(the type may be over '9')
    uint8_t expired;   // 0 or 1(use ascii '0' or '1' for persisting)
    // data
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