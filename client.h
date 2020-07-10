
#ifndef CHATROOM1_0_CLIENT_H
#define CHATROOM1_0_CLIENT_H

#include <WinSock2.h>
#include <process.h>
#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <ctime>
#include<windows.h>

using namespace std;
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

int isrecv = 0;
char userName[16] = {0};

class client {
private:
    WSADATA wsaData = {0};//存放套接字信息
    SOCKET serversocket = INVALID_SOCKET;//客户端套接字
    SOCKADDR_IN ServerAddr = {0};//服务端地址
    USHORT Port = 18000;//服务端端口

public:
    int ConnectServer();
};

void printnowtime() {
    time_t now = time(0);
    tm *nowtime = localtime(&now);
    cout << nowtime->tm_hour << ":" << nowtime->tm_min << ":" << nowtime->tm_sec << endl;
}

unsigned __stdcall ThreadSend(void *param) {
    char buf[128] = {0};
    int ret = 0;
    while (true) {
        if (KEY_DOWN(27)) {
            printf("%s: ", userName);
            cin >> buf;
            ret = send(*(SOCKET *) param, buf, sizeof(buf), 0);
            if (ret == SOCKET_ERROR)
                return 1;
        }
    }
    return 0;
}

unsigned __stdcall ThreadRecv(void *param) {
    char buf[128] = {0};
    while (true) {
        int ret = recv(*(SOCKET *) param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR) {
            Sleep(500);
            continue;
        }

        if (strlen(buf) != 0) {
            printnowtime();
            printf("%s\n\n", buf);
            isrecv = 1;
        } else
            Sleep(100);
    }
    return 0;
}


#endif //CHATROOM1_0_CLIENT_H
