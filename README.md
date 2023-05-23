# Bytecask

A simple kv store inspired by Bitcask.\
It runs on linux/unix for the use of unistd.h.(You can also try some solutions to compile it on windows)

## Reqirement
+ gnu/g++ 
+ C++17
+ Cmake

## Build
```
mkdir build
cmake -S . -B build
cd build
make
```
## Usage

**dbconfig.ini**\
You need to create an initial-file "dbconfig.ini" in the same directory as your executable file "core". So far, the content is as follows
```
[path]
dbpath="your-database-path" 
```

**core**\
Just directly run the program like:
```
./core
``` 
If there is no "dbconfig.ini", it will create a default "dbcongfig.ini". Because the database path is unknown, it will raise error.\
You can also give a database path as the sencond argument and the program will update "dbconfig.ini".
```
./core <database path>
```

In the early stages of development, this project is currently performing some interface testing only in the `main`. If you want to use it in your own project, please refer to the API in `bytecask.h` and include it where necessary. Additional important features will be improved later, and TCP/IP connectivity support will be provided.

## Todo:
+ memory key-value buffer/cache.
+ merge module to compact data files.
+ server/client database mode.
+ sepecifically optimized data structure for in-memory index.
+ ...