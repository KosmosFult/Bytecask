#include "bytecask.h"
#include <iostream>
#include <fstream>
#include <random>
#include <string>

std::string generateRandomString(int length) {
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.length() - 1);

    for (int i = 0; i < length; ++i) {
        int index = distribution(generator);
        result += charset[index];
    }

    return result;
}

int generateRandomInteger(int min, int max) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}


int basicTest1()
{
    int keylen, vallen;
    std::string key, val;
    std::ofstream file("/home/kosmos/workspace/Bytecask/log/genlog.txt");
    std::cout << "basicTest1 executing..." << std::endl;
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return -1;
    }

    for(int i=0; i<1000; i++)
    {
        keylen = generateRandomInteger(4,32);
        vallen = generateRandomInteger(8,32);
        key = generateRandomString(keylen);
        val = generateRandomString(vallen);
        set(key, val);
        file << key << "," << val << std::endl;
    }

    file.close();
    std::cout << "basicTest1 end" << std::endl;
    return 0;
} 