#ifndef TEST_H
#define TEST_H

#include <string>

class TraceLine
{
public:
    int no;
    std::string op;
    std::string key;
    std::string val;

    TraceLine(string line);
};

int compactTest();
int basicTest1();

#endif