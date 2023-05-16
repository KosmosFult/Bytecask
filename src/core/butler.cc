#include "butler.h"
#include "util.h"

extern string dbpath;
extern dbhash htable;
/**
 * read entry according to the offset. 
 * Not using buffer for globel processing(e.g. 4K for one read). 
 * The performance depends on the OS read cache or using mmap to 
 * rewrite this function later.
*/
int walk(int fd, off_t &offset, recordentry &entry)
{
    char hbuffer[4096]; // asserting key-value less than 4K bytes.
    ssize_t state;
    
    if((state = read(fd, hbuffer, ENTRY_HEADER_SIZE))<0)
        return -1;
    else if(state==0)
        return -2;
    
    string header_str(hbuffer, ENTRY_HEADER_SIZE);
    int pos = 0;
    entry.crc = strToCRC(header_str.substr(pos, CRC_WIDTH / 8));
    pos += CRC_WIDTH / 8;
    entry.tstamp = strToTime(header_str.substr(pos, TIMESTAMP_SIZE));
    pos += TIMESTAMP_SIZE;
    entry.k_size = stoi(header_str.substr(pos, V_KSIZE_LEN));
    pos += V_KSIZE_LEN;
    entry.v_size = stoi(header_str.substr(pos, V_VSIZE_LEN));
    pos += V_VSIZE_LEN;
    entry.type = *((const valuetype *)(header_str.substr(pos, VALUETYPE_SIZE).data()));
    pos += VALUETYPE_SIZE;
    entry.expired = *(const uint8_t *)(header_str.substr(pos, EXPIRED_SIZE).data());
    pos += EXPIRED_SIZE;

    if((state = read(fd, hbuffer, entry.k_size + entry.v_size))<=0)
        return -1;
    entry.key = string(hbuffer, entry.k_size);
    entry.value = string(hbuffer + entry.k_size, entry.v_size);
    offset += ENTRY_HEADER_SIZE + entry.k_size + entry.v_size;
    return 0;
}

// 注意没有判断新旧记录（理论上按时间排序不需要），没有判断expired
int buildMemIndexByFiles(vector<string> &files, dbhash &index, int clear)
{
    int fd;
    off_t offset;
    recordentry rc;
    hashvalue hv;
    int fid;
    if(clear)
        hashClear(index);
    for(auto &fname : files)
    {
        if((fd = open(fname.data(), O_RDONLY))<0)
            return -1;
        offset = 0;
        fid = fname2fid(fname);
        while(walk(fd, offset, rc)>=0)
        {
            if(rc.expired)
            {
                hashDel(index, rc.key);
                continue;
            }
            hv.file_id = fid;
            hv.record_size = ENTRY_HEADER_SIZE + rc.k_size + rc.v_size;
            hv.offset = offset - hv.record_size;
            hv.tstamp = rc.tstamp;
            hashSet(index, rc.key, hv);
        }
    }
    return 0;
}

int recoverMemIndex()
{
    vector<string> files = getDBFiles(dbpath);
    if(!files.empty())
        buildMemIndexByFiles(files, htable, 1);
    return 0;
}