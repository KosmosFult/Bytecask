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
#include <vector>

#define CRC_WIDTH 8
#define CRC_POLY 0x07

// #define TIMESTAMP_STR_LEN 10
#define TIMESTAMP_SIZE sizeof(time_t)
#define VALUETYPE_SIZE sizeof(valuetype)
#define EXPIRED_SIZE sizeof(recordentry::expired)
#define KEY_MAX_LEN 128
#define VALUE_MAX_LEN 256
#define V_KSIZE_LEN 3
#define V_VSIZE_LEN 4
#define ENTRY_HEADER_SIZE (CRC_WIDTH/8) + TIMESTAMP_SIZE + V_KSIZE_LEN + \
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
 * This struct has std::string variables for laziness. So do not persist
 * the entry directly or use memory binary operations. You need to
 * implement specific functions to persist it (ref. bytecask.cc-> 'persistRecord' and 'strToRecord')
 * When persisting, k_size, v_size of recordentr will be presented
 * as ascii char. There may be some problems because of Big-endian/Little-endian.
 * 
*/
typedef struct recordentry
{
    // header
    crc_t crc;
    time_t tstamp;
    int k_size;
    int v_size;
    valuetype type;    // for persisting, use byte not ascii(the type may be over '9')
    uint8_t expired;   // 0 or 1
    // data
    string key;
    string value;
}recordentry;


typedef struct hashvalue
{
    int file_id;
    int record_size;
    off_t offset;
    time_t tstamp;     
}hashvalue;

#endif