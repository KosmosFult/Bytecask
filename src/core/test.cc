#include "bytecask.h"
#include <iostream>
#include <fstream>
#include <random>
#include "test.h"

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


int basicTest1()
{
    int keylen, vallen;
    std::string key, val;
    std::ofstream file("/home/kosmos/workspace/Bytecask/log/genlog.txt");
    std::cout << "basicTest1 executing..." << std::endl;
    if (!file.is_open())
    {
        std::cerr << "Failed to open file." << std::endl;
        return -1;
    }

    for (int i = 0; i < 1000; i++)
    {
        keylen = generateRandomInteger(4, 32);
        vallen = generateRandomInteger(8, 32);
        key = generateRandomString(keylen);
        val = generateRandomString(vallen);
        set(key, val);
        file << key << "," << val << std::endl;
    }

    file.close();
    std::cout << "basicTest1 end" << std::endl;
    return 0;
}

int compactTest()
{
    string cmd = "python3 " + test_dir + "compact_test.py";
    system(cmd.data());
    ifstream trace(test_dir + "trace/trace_compact.txt");
    ofstream dbans(test_dir + "trace/trace_compact_dbans.txt");

    if (trace.is_open() && dbans.is_open())
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

        trace.close();
        dbans.close();
    }
    else
    {
        std::cerr << "Failed to open the file." << std::endl;
    }

    return 0;
}