#include "util.h"
#include <iostream>
#include <dirent.h>

int hashSet(dbhash &ht, string &key, hashvalue &value)
{
    ht.insert(make_pair(key, value));
    return 0;
}

int hashGet(dbhash &ht, string &key, hashvalue &value)
{
    auto itr = ht.find(key);
    if(itr != ht.end())
    {
        value = itr->second;
        return 0;
    }
    else
        return -1;
}


crc_t calCRC(string &body)
{
    return 0;
}

string crcToStr(crc_t crc)
{
    return "0";
}

crc_t strToCRC(string crcstr)
{
    return 0;
}

int validCRC(string &body)
{
    return 0;
}

int panic(string info)
{
    cout<<"panic: "<<info<<endl;
    return 0;
}

int printOpenfds()
{
    cout<<"openfds(fid,fd,time):[";
    extern map<int, openinfo> openfds;
    for(auto i = openfds.begin(); i!=openfds.end(); i++)
    {
        cout<<"("<<i->first<<","<<i->second.fd<<","<<i->second.tstamp<<"), ";
    }
    cout<<"]"<<endl;
    return 0;

}


int directoryExists(const char *path)
{
    DIR *directory = opendir(path);
    if (directory != NULL)
    {
        closedir(directory);
        return 0;
    }
    return -1;
}

string fid2fname(int fid)
{
    extern string dbpath;
    return dbpath + "dbfile_" + to_string(fid) + ".dbf";
}

string padWithZero(int num, int width)
{
    std::string str = std::to_string(num);
    if (str.length() < width)
    {
        str = std::string(width - str.length(), '0') + str;
    }
    return str;
}
