#include "bytecask.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>


int procedureCmd(const string &command, string &response)
{
    return 0;
}


int serverRun()
{
    // 创建 Socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定 IP 和端口
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(10088);
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // 监听连接
    listen(serverSocket, 5);

    while (true) {
        // 接受连接请求
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        cout << "Client connected" << endl;

        // 处理连接请求
        string command;
        string response;
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        while (true) {
            
        }

        // 关闭连接
        close(clientSocket);
        cout << "Client disconnected" << endl;
    }

    // 关闭 Socket
    close(serverSocket);
    return 0;
}