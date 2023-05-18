#ifndef COMMAND_H
#define COMMAND_H

#define MAXLINE 4096
#define MAXARGS 16


/**
 * command number
 * The first argument of each command will be encoded to fixed length data.
*/
#define CMD_help    1
#define CMD_set     2
#define CMD_get     3
#define CMD_del     4
#define CMD_connect 5
#define CMD_close   6
#define CMD_quit    7


#define SPLIT_BYTE  0x3
#define SPILT_CHAR  (char(SPLIT_BYTE))
#define END_BYTE    0x4
#define END_CHAR    (char(END_BYTE))

int cmdRun();

#endif