#include "bytecask.h"
#include <iostream>

extern int basicTest1();
extern int printOpenfds();

int main(int argc, char *argv[])
{
    if(dbinit(argv[1])<0)
        return -1;
    string cmd;
    string key;
    string value;
    basicTest1();

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
        else if(cmd == "quit")
        {
            dbclose();
            return 0;
        }
        cout<<"-------\n";
    }
    return 0;
}