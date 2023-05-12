#include "util.h"

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


