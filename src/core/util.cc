#include "util.h"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

int hashSet(dbhash &ht, string &key, hashvalue &value)
{
    ht.insert(make_pair(key, value));
    return 0;
}

int hashGet(dbhash &ht, string &key, hashvalue &value)
{
    auto itr = ht.find(key);
    if (itr != ht.end())
    {
        value = itr->second;
        return 0;
    }
    else
        return -1;
}

int hashDel(dbhash &ht, string &key)
{
    auto itr = ht.find(key);
    if (itr != ht.end())
    {
        ht.erase(itr);
        return 0;
    }
    else
        return -1;
}

int hashClear(dbhash &ht)
{
    ht.clear();
    return 0;
}

crc_t calCRC(string &body)
{
    return 0;
}

string crcToStr(crc_t crc)
{
    string crc_str((char *)(&crc), CRC_WIDTH / 8);
    return crc_str;
}

crc_t strToCRC(string crcstr)
{
    return 0;
}

int validCRC(string &body)
{
    return 0;
}

time_t strToTime(string time_str)
{
    return *((time_t *)time_str.data());
}

string timeToStr(time_t timestamp)
{
    return string((char *)(&timestamp), TIMESTAMP_SIZE);
}

int panic(string info)
{
    cout << "panic: " << info << endl;
    return 0;
}

int printOpenfds()
{
    cout << "openfds(fid,fd,time):[";
    extern map<int, openinfo> openfds;
    for (auto i = openfds.begin(); i != openfds.end(); i++)
    {
        cout << "(" << i->first << "," << i->second.fd << "," << i->second.tstamp << "), ";
    }
    cout << "]" << endl;
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

int fname2fid(string fname)
{
    size_t dotpos = fname.rfind(".");
    size_t linepos = fname.rfind("_");
    if ((dotpos == string::npos) || (linepos == string::npos))
        return -1;

    return stoi(fname.substr(linepos + 1, dotpos - linepos - 1));
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

bool hasExtension(const string &fileName, const string &extension)
{
    if (fileName.length() < extension.length())
        return false;
    return (fileName.compare(fileName.length() - extension.length(), extension.length(), extension) == 0);
}

vector<string> getDBFiles(const string &directory)
{
    vector<string> files;

    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    if ((dir = opendir(directory.c_str())) != nullptr)
    {
        while ((entry = readdir(dir)) != nullptr)
        {
            string fileName = entry->d_name;
            string filePath = directory + (directory.back() == '/' ? "" : "/") + fileName;

            if (hasExtension(fileName, ".dbf") && stat(filePath.c_str(), &fileStat) == 0)
            {
                if (S_ISREG(fileStat.st_mode))
                {
                    files.push_back(filePath);
                }
            }
        }
        closedir(dir);
    }

    sort(files.begin(), files.end(), [](const string &a, const string &b)
         {
        struct stat statA, statB;
        stat(a.c_str(), &statA);
        stat(b.c_str(), &statB);
        return statA.st_mtime < statB.st_mtime; });

    return files;
}

off_t getFileSize(string fname)
{
    struct stat fileStat;
    if (stat(fname.data(), &fileStat) == 0)
        return fileStat.st_size;
    else
        return 0;
}