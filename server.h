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
SOCKET ServerSocket = INVALID_SOCKET;         //������׽���
SOCKADDR_IN ClientAddr = {0};             //�ͻ��˵�ַ
int ClientAddrLen = sizeof(ClientAddr);
HANDLE hRecv1 = nullptr;
HANDLE hRecv2 = nullptr;

struct Client {
    SOCKET clientsocket;      //�ͻ����׽���
    char buf[128];         //���ݻ�����
    char username[16];   //�ͻ����û���
    char ip[20];         //�ͻ���IP
    UINT_PTR id;       //��ǿͻ��ˣ��������ֲ�ͬ�Ŀͻ���
};

Client Clientlist[2] = {0};                  //����һ���ͻ��˽ṹ��

class server {
private:
    WSADATA wsaData = {0};
    SOCKADDR_IN ServerAddr = {0};                //����˵�ַ
    USHORT uPort = 18000;                        //�����������˿�
public:
    int StartServer();

    void printtopbar();

    void initrecordfile();
};


//���������߳�
unsigned __stdcall ThreadSend(void *param) {
    int ret = 0;
    int flag = *(int *) param;
    SOCKET client = INVALID_SOCKET;                    //����һ����ʱ�׽��������Ҫת���Ŀͻ����׽���
    char temp[128] = {0};                            //����һ����ʱ�����ݻ�������������Ž��յ�������
    memcpy(temp, Clientlist[!flag].buf, sizeof(temp));
    sprintf(Clientlist[flag].buf, "%s: %s", Clientlist[!flag].username, temp);//���һ���û���ͷ
    if (strlen(temp) != 0 && issend == 0) //������ݲ�Ϊ���һ�ûת����ת��
        ret = send(Clientlist[flag].clientsocket, Clientlist[flag].buf, sizeof(Clientlist[flag].buf), 0);
    if (ret == SOCKET_ERROR)
        return 1;
    issend = 1;   //ת���ɹ�������״̬Ϊ��ת��
    return 0;
}

//��������
unsigned __stdcall ThreadRecv(void *param) {
    SOCKET client = INVALID_SOCKET;
    int flag = 0;
    if (*(int *) param == Clientlist[0].id)            //�ж����ĸ��ͻ��˷�������Ϣ
    {
        client = Clientlist[0].clientsocket;
        flag = 0;
    } else if (*(int *) param == Clientlist[1].id) {
        client = Clientlist[1].clientsocket;
        flag = 1;
    }
    char temp[128] = {0};  //��ʱ���ݻ�����
    while (true) {
        memset(temp, 0, sizeof(temp));
        int ret = recv(client, temp, sizeof(temp), 0); //��������
        if (ret == SOCKET_ERROR)
            continue;
        issend = 0;                                 //����ת��״̬Ϊδת��
        flag = client == Clientlist[0].clientsocket ? 1 : 0;        //���Ҫ���ã����������Լ����Լ�����Ϣ��BUG

        //�����¼����
        ofstream record;
        record.open("c://record.txt", ios::out | ios::app);
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        string t;
        t = to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec) + "\n";
        record << t << Clientlist[!flag].username << ":" << temp << "\n";
        record.close();

        memcpy(Clientlist[!flag].buf, temp, sizeof(Clientlist[!flag].buf));
        _beginthreadex(NULL, 0, ThreadSend, &flag, 0, NULL); //����һ��ת���߳�,flag�����Ҫת�����ĸ��ͻ���
        //����Ҳ�����ǵ���CPUʹ����������ԭ��
    }

    return 0;
}

//��������
//�����ӷ�һ�ο��ַ��� ��������ж��Ƿ�������
unsigned __stdcall ThreadManager(void *param) {
    while (true) {
        if (send(Clientlist[0].clientsocket, "", sizeof(""), 0) == SOCKET_ERROR) {
            if (Clientlist[0].clientsocket != 0) {
                CloseHandle(hRecv1); //����ر����߳̾�������ǲ��Խ���Ͽ���C/S�Ӻ�CPU��Ȼ����
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
        Sleep(2000); //2s���һ��
    }

    return 0;
}

//��������
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
            //����пͻ����������Ӿͽ�������
            if ((Clientlist[i].clientsocket = accept(ServerSocket, (SOCKADDR *) &ClientAddr, &ClientAddrLen)) ==
                INVALID_SOCKET)  //�������� clientsocket
            {
                printf("accept failed with error code: %d\n", WSAGetLastError());
                closesocket(ServerSocket);
                WSACleanup();
                return -1;
            }
            recv(Clientlist[i].clientsocket, Clientlist[i].username, sizeof(Clientlist[i].username), 0); //�����û���
            printf("Successfully got a connection from ip:%s ,Port: %d,UerName: %s\n",
                   inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port), Clientlist[i].username);

            //д����������
            ofstream record;
            record.open("c://record.txt", ios::out | ios::app);
            record << "Successfuuly got a connection from ip:" << inet_ntoa(ClientAddr.sin_addr) << " ,Port: "
                   << htons(ClientAddr.sin_port) << ",UserName: " << Clientlist[i].username << "\n";


            memcpy(Clientlist[i].ip, inet_ntoa(ClientAddr.sin_addr), sizeof(Clientlist[i].ip)); //��¼�ͻ���IP
            Clientlist[i].id = Clientlist[i].clientsocket; //��ͬ��socke�в�ͬUINT_PTR���͵���������ʶ
            i++;
        }
        i = 0;
        if (Clientlist[0].id != 0 && Clientlist[1].id != 0)                     //�������û��������Ϸ�������Ž�����Ϣת��
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
