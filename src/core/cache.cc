#include <map>
#include "type.h"
#include "cache.h"

extern map<int, openinfo> openfds;
extern string fid2fname(int fid);


int fdsLRU(int fid, int fd)
{
    auto minitr = openfds.begin();
    for(auto itr = openfds.begin(); itr != openfds.end(); itr++)
    {
        if(itr->second.tstamp < minitr->second.tstamp)
            minitr = itr;
    }
    openfds.erase(minitr);
    openfds.insert(make_pair(fid, openinfo{fd, time(NULL)}));
    return 0;
}


/*--------------------------*/

int accessFd(int fid)
{
    int fd;
    auto itr = openfds.find(fid);
    if(itr != openfds.end())
    {
        // 若已打开则直接返回，并更新访问时间
        itr->second.tstamp = time(NULL);
        return itr->second.fd;
    }
    else
    {
        // 否则
        if((fd = open(fid2fname(fid).c_str(), O_RDONLY))<0)
            return -1;
        if(openfds.size()>=MAX_OPENFDS)
            fdsLRU(fid, fd);
        else
            openfds.insert(make_pair(fid, openinfo{fd, time(NULL)}));
    }
    return 0;
}