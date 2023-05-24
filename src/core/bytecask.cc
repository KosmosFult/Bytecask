#include "bytecask.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <filesystem>
#include "cache.h"
#include "util.h"
#include "butler.h"
#include "inicpp.h"

string dbpath;
int act_file_id;
int act_file_fd;
off_t act_file_offset;
dbhash htable;

// 用于记录已打开的读文件描述符
map<int, openinfo> openfds;

/*--------------------------------------*/

int persistRecord(time_t tstamp, string &key, string &value, valuetype vtype, uint8_t expired = 0)
{
    int rfd;
    string type_byte(reinterpret_cast<char *>(&vtype), sizeof(vtype));
    string expired_byte(reinterpret_cast<char *>(&expired), 1);
    string body = timeToStr(tstamp) +
                  padWithZero(key.length(), V_KSIZE_LEN) +
                  padWithZero(value.length(), V_VSIZE_LEN) +
                  type_byte +
                  expired_byte +
                  key + value;
    crc_t crc = calCRC(body);
    string record = crcToStr(crc) + body;

    if (write(act_file_fd, record.c_str(), record.length()) < 0)
        return -1;

    act_file_offset += record.length();

    /**
     * if active file size reach the trigger then creat a
     * new file as current active file.
     */
    if (act_file_offset >= FILE_SIZE_TRIG)
    {
        act_file_id++;
        close(act_file_fd);
        string fname = fid2fname(act_file_id);
        if ((act_file_fd = open(fname.c_str(), O_APPEND | O_CREAT | O_WRONLY, 0644)) < 0)
            return -2;
        act_file_offset = 0;
    }

    return 0;
}

recordentry strToRecord(string &rs)
{
    int pos = 0;
    recordentry rc;

    rc.crc = strToCRC(rs.substr(pos, CRC_WIDTH / 8));
    pos += CRC_WIDTH / 8;
    rc.tstamp = strToTime(rs.substr(pos, TIMESTAMP_SIZE));
    pos += TIMESTAMP_SIZE;
    rc.k_size = stoi(rs.substr(pos, V_KSIZE_LEN));
    pos += V_KSIZE_LEN;
    rc.v_size = stoi(rs.substr(pos, V_VSIZE_LEN));
    pos += V_VSIZE_LEN;
    rc.type = *((const valuetype *)(rs.substr(pos, VALUETYPE_SIZE).data()));
    pos += VALUETYPE_SIZE;
    rc.expired = *(const uint8_t *)(rs.substr(pos, EXPIRED_SIZE).data());
    pos += EXPIRED_SIZE;

    rc.key = rs.substr(pos, rc.k_size);
    pos += rc.k_size;
    rc.value = rs.substr(pos, rc.v_size);
    return rc;
}

int createDefaultConfig(string path)
{
    ini::IniFile dbini;
    dbini["path"]["dbpath"] = "UNKNOWN";
    dbini.save(path);
    return 0;
}

/*--------------------------------------*/

int dbReadMata()
{
    /**
     * 读取数据库元信息，例如当前最大文件号，进而获得act_file_id
     */
    act_file_id = 0;
    act_file_offset = 0;

    vector<string> files = getDBFiles(dbpath);
    if (!files.empty())
    {
        act_file_id = fname2fid(files.back());
        act_file_offset = getFileSize(files.back());
    }
    return 0;
}

/**
 * 恢复内存中的hash索引结果，可选方案
 *      1. 系统退出时持久化hash
 *      2. 扫描整个数据库文件，重构hash
 */
int dbLoadMem()
{
    recoverMemIndex();
    return 0;
}

int dbinit(int argc, char *argv[])
{
    int rfd;
    ini::IniFile dbini;
    const char *argpath = argc > 1 ? argv[1] : NULL;
    filesystem::path exepath(argv[0]);
    filesystem::path exedir = exepath.parent_path();
    string config_path = exedir / CONFIG_FILE;

    printf("database initializing...\n");

    if (!filesystem::exists(config_path))
        createDefaultConfig(config_path);

    dbini.load(config_path);
    dbpath = dbini["path"]["dbpath"].as<string>();

    if (argpath)
    {
        if (filesystem::exists(argpath))
        {
            dbpath = string(argpath);
            dbini["path"]["dbpath"] = dbpath;
        }
        else
        {
            panic("database path \"" + string(argpath) + "\" not exists.");
            return -1;
        }
    }
    else if (!filesystem::exists(dbpath))
    {
        panic("database path \"" + dbpath + "\" not exists.\nplease check your \"" +
              config_path + "\".");
        return -1;
    }

    if(dbpath.back() != '/')
        dbpath.append("/");
    dbini.save(config_path);
    dbReadMata();
    dbLoadMem();
    if ((act_file_fd = open(fid2fname(act_file_id).c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0)
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

int set(string &key, string &value, valuetype type)
{
    time_t ctime = time(NULL);
    int record_size = ENTRY_HEADER_SIZE + key.length() + value.length();
    hashvalue v = {
        act_file_id,
        record_size,
        act_file_offset,
        ctime};

    int state = 0;
    if ((state = persistRecord(ctime, key, value, type)) < 0)
    {
        panic("set error state_" + to_string(state) + "\n");
        return -1;
    }
    hashSet(htable, key, v);

    return 0;
}

string get(string &key)
{
    int fd;
    char buf[VALUE_MAX_LEN];
    string rs;
    hashvalue hv;

    if (hashGet(htable, key, hv) < 0)
        return "";

    fd = accessFd(hv.file_id);
    if (fd < 0)
    {
        panic("get error--can not get file(fid = " + to_string(hv.file_id));
        return "";
    }

    lseek(fd, hv.offset, SEEK_SET);
    if (read(fd, buf, hv.record_size) <= 0)
    {
        panic("get error--record not found");
        return "";
    }

    rs = string(buf, hv.record_size);

    recordentry rc = strToRecord(rs);
    return rc.value;
}

int expire(string &key)
{
    hashvalue hv;
    time_t ctime = time(NULL);
    string expired_value = "E";

    /**
     * There are two query for MemIndex and they can be integrated by
     * "hashGetAndDel". But if IO fails later, the MemIndex needs to be
     * recover.
     */
    if (hashGet(htable, key, hv) < 0)
        return -1;

    int state = 0;
    if ((state = persistRecord(ctime, key, expired_value, STRING, 1)) < 0)
    {
        panic("expire error state_-2\n");
        return -2;
    }

    if (hashDel(htable, key) < 0)
        return -3;
    return 0;
}

int merge()
{
    printf("merge start\n");
    if(mergeDBfiles() < 0)
        return -1;
    printf("merge completed\n");
    dbReadMata();
    dbLoadMem();
    openfds.clear();
    return 0;
}