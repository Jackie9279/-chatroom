#ifndef CHATROOM1_0_SERVER_H
#define CHATROOM1_0_SERVER_H

#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;


int issend = 0;
SOCKET ServerSocket = INVALID_SOCKET;         //服务端套接字
SOCKADDR_IN ClientAddr = {0};             //客户端地址
int ClientAddrLen = sizeof(ClientAddr);
HANDLE hRecv1 = nullptr;
HANDLE hRecv2 = nullptr;

struct Client {
    SOCKET clientsocket;      //客户端套接字
    char buf[128];         //数据缓冲区
    char username[16];   //客户端用户名
    char ip[20];         //客户端IP
    UINT_PTR id;       //标记客户端，用来区分不同的客户端
};

Client Clientlist[2] = {0};                  //创建一个客户端结构体

class server {
private:
    WSADATA wsaData = {0};
    SOCKADDR_IN ServerAddr = {0};                //服务端地址
    USHORT uPort = 18000;                        //服务器监听端口
public:
    int StartServer();

    void printtopbar();

    void initrecordfile();
};


//发送数据线程
unsigned __stdcall ThreadSend(void *param) {
    int ret = 0;
    int flag = *(int *) param;
    SOCKET client = INVALID_SOCKET;                    //创建一个临时套接字来存放要转发的客户端套接字
    char temp[128] = {0};                            //创建一个临时的数据缓冲区，用来存放接收到的数据
    memcpy(temp, Clientlist[!flag].buf, sizeof(temp));
    sprintf(Clientlist[flag].buf, "%s: %s", Clientlist[!flag].username, temp);//添加一个用户名头
    if (strlen(temp) != 0 && issend == 0) //如果数据不为空且还没转发则转发
        ret = send(Clientlist[flag].clientsocket, Clientlist[flag].buf, sizeof(Clientlist[flag].buf), 0);
    if (ret == SOCKET_ERROR)
        return 1;
    issend = 1;   //转发成功后设置状态为已转发
    return 0;
}

//接受数据
unsigned __stdcall ThreadRecv(void *param) {
    SOCKET client = INVALID_SOCKET;
    int flag = 0;
    if (*(int *) param == Clientlist[0].id)            //判断是哪个客户端发来的消息
    {
        client = Clientlist[0].clientsocket;
        flag = 0;
    } else if (*(int *) param == Clientlist[1].id) {
        client = Clientlist[1].clientsocket;
        flag = 1;
    }
    char temp[128] = {0};  //临时数据缓冲区
    while (true) {
        memset(temp, 0, sizeof(temp));
        int ret = recv(client, temp, sizeof(temp), 0); //接收数据
        if (ret == SOCKET_ERROR)
            continue;
        issend = 0;                                 //设置转发状态为未转发
        flag = client == Clientlist[0].clientsocket ? 1 : 0;        //这个要设置，否则会出现自己给自己发消息的BUG

        //聊天记录保存
        ofstream record;
        record.open("c://record.txt", ios::out | ios::app);
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        string t;
        t = to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec) + "\n";
        record << t << Clientlist[!flag].username << ":" << temp << "\n";
        record.close();

        memcpy(Clientlist[!flag].buf, temp, sizeof(Clientlist[!flag].buf));
        _beginthreadex(NULL, 0, ThreadSend, &flag, 0, NULL); //开启一个转发线程,flag标记着要转发给哪个客户端
        //这里也可能是导致CPU使用率上升的原因。
    }

    return 0;
}

//管理连接
//两秒钟发一次空字符串 如果接受判断是否还连接着
unsigned __stdcall ThreadManager(void *param) {
    while (true) {
        if (send(Clientlist[0].clientsocket, "", sizeof(""), 0) == SOCKET_ERROR) {
            if (Clientlist[0].clientsocket != 0) {
                CloseHandle(hRecv1); //这里关闭了线程句柄，但是测试结果断开连C/S接后CPU仍然疯涨
                CloseHandle(hRecv2);
                printf("Disconnect from ip: %s,UserName: %s\n", Clientlist[0].ip, Clientlist[0].username);
                closesocket(Clientlist[0].clientsocket);
                Clientlist[0] = {0};
            }
        }
        if (send(Clientlist[1].clientsocket, "", sizeof(""), 0) == SOCKET_ERROR) {
            if (Clientlist[1].clientsocket != 0) {
                CloseHandle(hRecv1);
                CloseHandle(hRecv2);
                printf("Disconnect from ip: %s,UserName: %s\n", Clientlist[1].ip, Clientlist[1].username);
                closesocket(Clientlist[1].clientsocket);
                Clientlist[1] = {0};
            }
        }
        Sleep(2000); //2s检查一次
    }

    return 0;
}

//接受请求
unsigned __stdcall ThreadAccept(void *param) {

    int i = 0;
    int t1 = 0, t2 = 0;
    _beginthreadex(NULL, 0, ThreadManager, NULL, 0, NULL);
    while (true) {
        while (i < 2) {
            if (Clientlist[i].id != 0) {  //
                ++i;
                continue;
            }
            //如果有客户端申请连接就接受连接
            if ((Clientlist[i].clientsocket = accept(ServerSocket, (SOCKADDR *) &ClientAddr, &ClientAddrLen)) ==
                INVALID_SOCKET)  //接受链接 clientsocket
            {
                printf("accept failed with error code: %d\n", WSAGetLastError());
                closesocket(ServerSocket);
                WSACleanup();
                return -1;
            }
            recv(Clientlist[i].clientsocket, Clientlist[i].username, sizeof(Clientlist[i].username), 0); //接收用户名
            printf("Successfully got a connection from ip:%s ,Port: %d,UerName: %s\n",
                   inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port), Clientlist[i].username);

            //写入连接数据
            ofstream record;
            record.open("c://record.txt", ios::out | ios::app);
            record << "Successfuuly got a connection from ip:" << inet_ntoa(ClientAddr.sin_addr) << " ,Port: "
                   << htons(ClientAddr.sin_port) << ",UserName: " << Clientlist[i].username << "\n";


            memcpy(Clientlist[i].ip, inet_ntoa(ClientAddr.sin_addr), sizeof(Clientlist[i].ip)); //记录客户端IP
            Clientlist[i].id = Clientlist[i].clientsocket; //不同的socke有不同UINT_PTR类型的数字来标识
            i++;
        }
        i = 0;
        if (Clientlist[0].id != 0 && Clientlist[1].id != 0)                     //当两个用户都连接上服务器后才进行消息转发
        {
            ofstream record;
            record.open("c://record.txt", ios::out | ios::app);
            record << "\n";
            record.close();
            if (Clientlist[0].id != t1) {
                if (hRecv1)
                    CloseHandle(hRecv1);
                hRecv1 = (HANDLE) _beginthreadex(nullptr, 0, ThreadRecv, &Clientlist[0].id, 0, nullptr);
            }
            if (Clientlist[1].id != t2) {
                if (hRecv2)
                    CloseHandle(hRecv2);
                hRecv2 = (HANDLE) _beginthreadex(nullptr, 0, ThreadRecv, &Clientlist[1].id, 0, nullptr);
            }
        }
        t1 = Clientlist[0].id;
        t2 = Clientlist[1].id;
        Sleep(2000);
    }
    return 0;
}

#endif //CHATROOM1_0_SERVER_H
