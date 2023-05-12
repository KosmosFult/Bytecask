#include "bytecask.h"
#include <iostream>


int main()
{
    string key;
    string value;
    while(1){
        cin>>key>>value;
        set(key, value);
        cout<<"-------\n";
    }
    return 0;
}