#include <cstring>
#include "command.h"
#include <string>
#include <iostream>

using namespace std;

int is_connected;
int client_socket;

int initCMD()
{
    client_socket = -1;
    is_connected = 0;
    return 0;
}

int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */

    strcpy(buf, cmdline);
    buf[strlen(buf)] = ' ';
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'')
    {
        buf++;
        delim = strchr(buf, '\'');
    }
    else
    {
        delim = strchr(buf, ' ');
    }

    while (delim)
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* ignore spaces */
            buf++;

        if (*buf == '\'')
        {
            buf++;
            delim = strchr(buf, '\'');
        }
        else
        {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL;

    return argc;
}

int getCommandNumber(string mainArg)
{
    for(auto &c : mainArg)
        c = tolower(c);
    if(mainArg == "help")
        return CMD_help;
    else if(mainArg == "set")
        return CMD_set;
    else if(mainArg == "get")
        return CMD_get;
    else if(mainArg == "del")
        return CMD_del;
    else if(mainArg == "connect")
        return CMD_connect;
    else if(mainArg == "close")
        return CMD_close;
    else if(mainArg == "quit")
        return CMD_quit;
    else
        return -1;
}


bool argCheck(int argc, char **argv, int cmdn)
{
    switch (cmdn)
    {
    case CMD_help:
    case CMD_close:
    case CMD_quit:
        return argc == 1;
        break;
    case CMD_set:
        return argc == 3;
        break;
    case CMD_get:
    case CMD_del:
    case CMD_connect:
        return argc == 2;
        break;
    default:
        break;
    }
    return false;
}

string encode(int argc, char **argv, int cmdn)
{
    uint8_t cmdn_byte = cmdn;
    string message((char*)(&cmdn_byte), 1);
    for(int i = 1; i < argc; i++)
    {
        message += (SPILT_CHAR + string(argv[i]));
    }
    message += END_CHAR;
    return message;
}

int help()
{
    cout << "Usage:" << endl;
    cout << "    connect <ip>:<port>            | connect to the database server" << endl;
    cout << "    close                          | close current connection" << endl;
    cout << "    set <key> <value>              | set key-value pair" << endl;
    cout << "    get <key>                      | get the value by key" << endl;
    cout << "    del <key>                      | expire key-value pair"<<endl;      
    cout << "    quit                           | quit command line" << endl;
    return 0;
}

int connect(const string &addr)
{
    return 0;
}

int close()
{
    return 0;
}

int quit()
{
    if(is_connected)
        close();
    return 1;
}

int execute(const string &cmd)
{
    string message;
    static char *argv[MAXARGS];
    int argc = parseline(cmd.data(), argv);
    int cmdn = 0;
    if((cmdn = getCommandNumber(string(argv[0]))) < 0)
        return -1;
    
    if(!argCheck(argc, argv, cmdn))
        return -1;

    switch (cmdn)
    {
    case CMD_help:
        help();
        break;
    case CMD_connect:
        connect(string(argv[1]));
        break;
    case CMD_close:
        close();
        break;
    case CMD_quit:
        return quit();
    default:
        message = encode(argc, argv, cmdn);

        break;
    }
    return 0;
}