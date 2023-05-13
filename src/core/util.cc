#include "util.h"
#include <iostream>

int hashSet(dbhash &ht, string &key, hashvalue &value)
{
    ht.insert(make_pair(key, value));
    return 0;
}

int hashGet(dbhash &ht, string &key, hashvalue &rvalue)
{
    auto itr = ht.find(key);
    if(itr != ht.end())
    {
        rvalue = itr->second;
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