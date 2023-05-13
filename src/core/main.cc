#include "bytecask.h"
#include <iostream>


int main(int argc, char *argv[])
{
    if(dbinit(argv[1])<0)
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
        }
        else if(cmd == "quit")
        {
            dbclose();
        }
        cout<<"-------\n";
    }
    return 0;
}