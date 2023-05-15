#include <map>
#include "type.h"
#include "cache.h"

extern map<int, openinfo> openfds;
extern string fid2fname(int fid);



/**
 * it only perform well when read intensively for writing is 
 * designed to "append" mode.
*/
int fdsLRU(int fid, int fd)
{
    auto minitr = openfds.begin();
    for(auto itr = openfds.begin(); itr != openfds.end(); itr++)
    {
        if(itr->second.tstamp < minitr->second.tstamp)
            minitr = itr;
    }
    close(minitr->second.fd);
    openfds.erase(minitr);
    openfds.insert(make_pair(fid, openinfo{fd, time(NULL)}));
    return 0;
}


/*--------------------------*/

int accessFd(int fid)
{
    int fd = -1;
    auto itr = openfds.find(fid);
    if(itr != openfds.end())
    {
        // 若已打开则直接返回，并更新访问时间
        itr->second.tstamp = time(NULL);
        return itr->second.fd;
    }
    else
    {
        // 否则则打开文件，且如果打开文件超过一定数量则LRU替换
        if((fd = open(fid2fname(fid).c_str(), O_RDONLY))<0)
            return -1;
        if(openfds.size()>=MAX_OPENFDS)
            fdsLRU(fid, fd);
        else
            openfds.insert(make_pair(fid, openinfo{fd, time(NULL)}));
    }
    return fd;
}