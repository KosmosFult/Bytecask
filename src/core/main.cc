#include "bytecask.h"
#include <iostream>
#include <cstring>
#include "test.h"
extern int printOpenfds();

int main(int argc, char *argv[])
{
    if(dbinit(argc, argv)<0)
        return -1;
    string cmd;
    string key;
    string value;

    while(1){
        cin>>cmd;
        if(cmd == "set")
        {
            cout<<"key:";
            cin>>key;
            cout<<"val:";
            cin>>value;
            set(key, value);
        }
        else if(cmd == "get")
        {
            cout<<"key:";
            cin>>key;
            value = get(key);
            cout<<"getval:"<<value<<endl;
            printOpenfds();
        }
        else if(cmd == "del")
        {
            cout<<"key:";
            cin>>key;
            expire(key);
        }
        else if(cmd == "quit")
        {
            dbclose();
            return 0;
        }
        else if(cmd == "merge")
        {
            merge();
        }
        else if(cmd == "cptest")
        {
            compactTest();
        }
        cout<<"-------\n";
    }
    return 0;
}