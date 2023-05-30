#include "bytecask.h"
#include <iostream>
#include <fstream>
#include <random>
#include "test.h"
#include "util.h"

const string test_dir = "/home/kosmos/workspace/Bytecask/test/";

std::string generateRandomString(int length)
{
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.length() - 1);

    for (int i = 0; i < length; ++i)
    {
        int index = distribution(generator);
        result += charset[index];
    }

    return result;
}

int generateRandomInteger(int min, int max)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}


TraceLine::TraceLine(string line) : val("")
{
    size_t dot1, dot2, dot3;
    dot1 = line.find(',');
    dot2 = line.find(',', dot1+1);
    dot3 = line.find(',', dot2+1);
    no = stoi(line.substr(0, dot1));
    op = line.substr(dot1+1, dot2-dot1-1);
    if(dot3 == string::npos)
        key = line.substr(dot2+1);
    else
    {
        key = line.substr(dot2+1, dot3-dot2-1);
        val = line.substr(dot3+1);
    }
    
}

KVLine::KVLine(string line)
{
    size_t dot;
    dot = line.find(',');
    key = line.substr(0, dot);
    val = line.substr(dot+1);
}


int compactTest()
{
    string cmd = "python3 " + test_dir + "compact_test.py";
    system(cmd.data());
    string trace_fpath = test_dir + "trace/trace_compact.txt";
    string kvs_fpath = test_dir + "trace/trace_compact_kvs.txt";
    string dbans_fpath = test_dir + "trace/trace_compact_dbans.txt";
    string kvans_fpath = test_dir + "trace/trace_compacted_dbkvs.txt";
    ifstream trace(trace_fpath);
    ifstream kvs(kvs_fpath);
    ofstream dbans(dbans_fpath);
    ofstream kvans(kvans_fpath);

    if (trace.is_open() && dbans.is_open() && kvs.is_open() && kvans.is_open())
    {
        string line;
        while (getline(trace, line))
        {
            TraceLine tl(line);
            if(tl.op == "set")
                set(tl.key, tl.val);
            else if(tl.op == "get")
                dbans << to_string(tl.no) << "," << get(tl.key) << endl;
            else
                expire(tl.key);
        }

        std::cout << "trace completed." << std::endl; 

        if(merge() < 0)
        {
            std::cout << "compact test failed in merge" << std::endl;
            return -1;
        }

        while(getline(kvs, line))
        {
            KVLine tkv(line);
            kvans << tkv.key << "," << get(tkv.key) << endl;
        }

        kvs.close();
        kvans.close();
        trace.close();
        dbans.close();
    }
    else
    {
        std::cerr << "Failed to open the file." << std::endl;
        return -1;
    }

    string traceasn_fpath = test_dir + "trace/trace_compact_ans.txt";
    std::cout << "\ncomparing traces..." << std::endl;
    system(("cmp " + traceasn_fpath + " " + dbans_fpath).data());
    std::cout << "\ncomparing compacted key-value..." << std::endl;
    system(("cmp " + kvs_fpath + " " + kvans_fpath).data());
    return 0;
}