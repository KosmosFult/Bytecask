#include "bytecask.h"
#include <stdio.h>
#include <fcntl.h>
#include <map>
#include <unistd.h>

const string dbpath = "./db/";
int act_file_id;
int act_file_fd;
int act_file_offset;
dbhash htable;
map<int, openinfo> openfds;

string padWithZero(int num, int width) {
    std::string str = std::to_string(num);
    if (str.length() < width) {
        str = std::string(width - str.length(), '0') + str;
    }
    return str;
}

int persistRecord(time_t tstamp, string &key, string &value)
{
    string body = padWithZero(tstamp, sizeof(time_t)) +
                    padWithZero(key.length(), sizeof(size_t)) +
                    padWithZero(value.length(), sizeof(size_t)) +
                    key + value;
    string crc = calCRC(body);
    string record = crc+body;

    if(write(act_file_fd, record.c_str(), record.length())<0)
        return -1;

    act_file_offset += record.length();

    // 如果文件大小超过阈值则新建活动文件
    if(act_file_offset >= FILE_SIZE_TRIG)
    {
        act_file_id++;
        close(act_file_fd);
        string fname = dbpath + "dbfile_"+to_string(act_file_id)+".dbf";
        if((act_file_fd = open(fname.c_str(), O_APPEND | O_CREAT))<0)
            return -2;
        act_file_offset = 0;
        openfds.insert(make_pair(act_file_id, openinfo{act_file_fd, time(NULL)}));
    }

    return 0;
}





/*--------------------------------------*/


int dbinit(string & dbpath)
{
    act_file_id = 0;
    return 0;
}

int set(string &key, string &value)
{
    time_t ctime = time(NULL);
    int record_size = (CRC_WIDTH/8) + sizeof(time_t) + 2*sizeof(int) + 
                        key.length() + value.length();
    hashvalue v = {
        act_file_id,
        record_size,
        act_file_offset,
        ctime
    };

    hashSet(htable, key, v);
    persistRecord(ctime, key, value);
    
    return 0;
}