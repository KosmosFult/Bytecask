#include "bytecask.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "cache.h"
#include "util.h"

string dbpath;
int act_file_id;
int act_file_fd;
int act_file_offset;
dbhash htable;

// 用于记录已打开的读文件描述符
map<int, openinfo> openfds;

/*--------------------------------------*/

int directoryExists(const char* path) {
    DIR* directory = opendir(path);
    if (directory != NULL) {
        closedir(directory);
        return 0;
    }
    return -1;
}

string fid2fname(int fid)
{
    return dbpath + "dbfile_"+to_string(fid)+".dbf";
}

string padWithZero(int num, int width) {
    std::string str = std::to_string(num);
    if (str.length() < width) {
        str = std::string(width - str.length(), '0') + str;
    }
    return str;
}

int persistRecord(time_t tstamp, string &key, string &value)
{
    int rfd;
    // 目前unix时间戳正常情况为10位数
    string body = padWithZero(tstamp, TIMESTAMP_STR_LEN) +
                    padWithZero(key.length(), V_KSIZE_LEN) +
                    padWithZero(value.length(), V_VSIZE_LEN) +
                    key + value;
    crc_t crc = calCRC(body);
    string record = crcToStr(crc)+body;

    if(write(act_file_fd, record.c_str(), record.length())<0)
        return -1;

    act_file_offset += record.length();

    // 如果文件大小超过阈值则新建活动文件
    if(act_file_offset >= FILE_SIZE_TRIG)
    {
        act_file_id++;
        close(act_file_fd);
        string fname = fid2fname(act_file_id);
        if((act_file_fd = open(fname.c_str(), O_APPEND | O_CREAT, 0644))<0)
            return -2;
        act_file_offset = 0;
        if((rfd = open(fname.c_str(), O_RDONLY))<0)
            return -3;
        openfds.insert(make_pair(act_file_id, openinfo{rfd, time(NULL)}));
    }

    return 0;
}

recordentry strToRecord(string & rs)
{
    int pos = 0;
    recordentry rc;

    rc.crc = strToCRC(rs.substr(pos, CRC_WIDTH/8));
    pos += CRC_WIDTH/8;
    rc.tstamp = stol(rs.substr(pos, TIMESTAMP_STR_LEN));  // 在time_t为long时成立
    pos += TIMESTAMP_STR_LEN;
    rc.k_size = stoi(rs.substr(pos, V_KSIZE_LEN));
    pos += V_KSIZE_LEN;
    rc.v_size = stoi(rs.substr(pos, V_VSIZE_LEN));
    pos += V_VSIZE_LEN;
    rc.key = rs.substr(pos, rc.k_size);
    pos += rc.k_size;
    rc.value = rs.substr(pos, rc.v_size);
    return rc;
} 


/*--------------------------------------*/

int dbReadMata()
{
    /**
     * TODO: 
     * 读取数据库源信息，例如当前最大文件号，进而获得act_file_id
    */
    act_file_id = 0;
    act_file_offset = 0;
    return 0;
}

int dbLoadMem()
{
    /**
     * TODO:
     * 恢复内存中的hash索引结果，可选方案
     *      1. 系统退出时持久化hash
     *      2. 扫描整个数据库文件，重构hash
    */
    return 0;
}


int dbinit(const char* path)
{
    int rfd;

    printf("database initializing...\n");
    dbpath = string(path);

    if(directoryExists(path)<0)
    {
        panic("dbinit--database path \"" + dbpath + "\" not exists\n");
        return -1;
    }
    else
        dbpath = string(path);

    
    dbReadMata();
    dbLoadMem();
    if((act_file_fd = open(fid2fname(act_file_id).c_str(), O_WRONLY|O_CREAT|O_APPEND, 0644))<0)
    {
        panic("dbinit--open file");
        return -1;
    }

    rfd = open(fid2fname(act_file_id).c_str(), O_RDONLY);
    openfds.insert(make_pair(act_file_id, openinfo{rfd, time(NULL)}));
    printf("database initializing completed\n");
    return 0;
}


int dbclose()
{
    return 0;
}

int set(string &key, string &value)
{
    time_t ctime = time(NULL);
    int record_size = (CRC_WIDTH/8) + TIMESTAMP_STR_LEN + V_KSIZE_LEN +
                        V_VSIZE_LEN +  key.length() + value.length();
    hashvalue v = {
        act_file_id,
        record_size,
        act_file_offset,
        ctime
    };

    hashSet(htable, key, v);

    int state = 0;
    if((state = persistRecord(ctime, key, value))<0)
    {
        panic("set error state_"+ to_string(state) + "\n");
        return -1;
    }
    
    return 0;
}

string get(string &key)
{
    int fd;
    char buf[VALUE_MAX_LEN];
    string rs;
    auto vitr = htable.find(key);
    if(vitr == htable.end())
        return "";

    fd = accessFd(vitr->second.file_id);
    if(fd < 0)
    {
        panic("get error--can not get file(fid = " + to_string(vitr->second.file_id));
        return "";
    }
    
    lseek(fd, vitr->second.offset, SEEK_SET);
    printf("%d\n", vitr->second.record_size);
    if(read(fd, buf, vitr->second.record_size)<=0)
    {
        panic("get error--record not found");
        return "";
    }
    
    rs = string(buf);
    
    recordentry rc = strToRecord(rs);
    return rc.value;
}