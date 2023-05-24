#include "butler.h"
#include "util.h"

int active_compact_id = -1;
/**
 * Read entry according to the offset. 
 * Not using buffer for globel process(e.g. 4K for one read). 
 * The performance depends on the OS read cache or using mmap to rewrite this function later.
 * 
 * This function read an entry according to fd and offset. Result of the read happens on
 * the reference varible 'entry' and the varible 'offset' updates automatically.
 * The offset of fd needs to match the variable 'offset' which can save the
 * cost of lseek(trap to kernel)
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


int walk_s(int fd, off_t &offset, recordentry &entry, string &record)
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
    record = header_str + entry.key + entry.value;
    return 0;
}

// 注意没有判断新旧记录（理论上文件按创建时间排序不需要）
int buildMemIndexFromFiles(vector<string> &files, dbhash &index, int clear)
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
        while(walk(fd, offset, rc) >= 0)
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
        close(fd);
    }
    return 0;
}

int recoverMemIndex()
{
    vector<string> files = getDBFiles(dbpath);
    if(!files.empty())
        buildMemIndexFromFiles(files, htable, 1);
    return 0;
}


/**
 * Stop the key-value store and compact the old files.
 * It will not change the memory index in this version(hint file will be implement later).
*/
int _static_compact(vector<string> &files)
{
    // for print info
    int total = files.size();
    int progress = 0;

    int new_fd, old_fd;
    off_t new_file_offset = FILE_SIZE_TRIG + 1;
    off_t old_file_offset = 0;
    vector<string> new_files;
    string fpath, record_str;
    active_compact_id = -1;
    int state = 0;
    recordentry rc;
    hashvalue hv;
    

    for(auto &fname : files)
    {
        if((old_fd = open(fname.data(), O_RDONLY)) < 0)
        {
            state = -1;
            break;
        }

        old_file_offset = 0;
        while(walk_s(old_fd, old_file_offset, rc, record_str) >= 0)
        {
            if(rc.expired)
                continue;

            if(new_file_offset > FILE_SIZE_TRIG)
            {
                if(active_compact_id >= 0)
                    close(new_fd);
                fpath = toAbpath("cpfile_" + to_string(++active_compact_id) + ".cpf");
                if((new_fd = open(fpath.data(), O_CREAT | O_WRONLY | O_APPEND, 0644)) < 0)
                {
                    state = -1;
                    break;
                }
                new_files.push_back(fpath);
                new_file_offset = 0;
            }
            
            // The record is valid if the timestamp is consistent with memory index
            if((hashGet(htable, rc.key, hv) == 0) && (hv.tstamp == rc.tstamp))
            {
                if (write(new_fd, record_str.data(), record_str.length()) < 0)
                {
                    state = -1;
                    break;
                }
                new_file_offset += record_str.length();
            }
        }
        close(old_fd);

        printProgressBar(++progress, total);
    }

    // if error, delete all compact transient files
    if(state < 0)
    {
        for(auto &fname : new_files)
            remove(fname.data());
        return -1;
    }

    if(active_compact_id >= 0)
        close(new_fd);
    return 0;
}

int mergeDBfiles()
{
    // close all opened fds
    close(act_file_fd);
    for(auto i : openfds)
    {
        if(i.second.fd != act_file_fd)
            close(i.second.fd);
    }
 
    vector<string> dbfiles = getDBFiles(dbpath);
    sortFilesById(dbfiles);

    // if faild, reopen files
    if(_static_compact(dbfiles)!=0)
    {
        act_file_fd = open(fid2fname(act_file_id).data(), O_APPEND | O_WRONLY);
        for(auto i : openfds)
        {
            if(i.first != act_file_id)
            {
                i.second.fd = open(fid2fname(i.first).data(), O_APPEND | O_WRONLY);
            }
        }
        return -1;
    }

    // Remove all the data files
    for(auto &fname : dbfiles)
        remove(fname.data());
    
    vector<string> cpfiles = getCPFiles(dbpath);
    sortFilesById(cpfiles);
    // Rename these files from id 0
    int id = 0;
    string new_name;
    for(auto &fname : cpfiles)
    {
        new_name = toAbpath("dbfile_" + to_string(id++) + ".dbf");
        rename(fname.data(), new_name.data());
    }

    return 0;
}