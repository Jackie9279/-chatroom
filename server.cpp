

#include "server.h"

int server::StartServer() {
    //初始化套接字
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //判断版本
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //创建套接字
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET) {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }

    //设置服务器地址
    ServerAddr.sin_family = AF_INET;//连接方式
    ServerAddr.sin_port = htons(uPort);//服务器监听端口
    ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//任何客户端都能连接这个服务器

    //绑定服务器
    if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr))) {
        printf("bind failed with error code: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        return -1;
    }
    //设置监听客户端连接数
    if (SOCKET_ERROR == listen(ServerSocket, 20000)) {
        printf("listen failed with error code: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }

    printtopbar();
    initrecordfile();

    _beginthreadex(NULL, 0, ThreadAccept, NULL, 0, 0);
    for (int k = 0; k < 100; k++) //让主线程休眠
        Sleep(99999999);

    //关闭套接字
    for (int j = 0; j < 2; j++) {
        if (Clientlist[j].clientsocket != INVALID_SOCKET)
            closesocket(Clientlist[j].clientsocket);
    }
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}

void server::printtopbar() {
    cout << "=========================局域网聊天服务器==============================" << endl;
    cout << "========================聊天记录保存在c盘record.txt====================" << endl;
}

void server::initrecordfile() {
    ofstream x("c://record.txt", ios_base::out | ios_base::trunc);     //删除以前数据
    x.close();
    ofstream record;
    record.open("c://record.txt", ios::out | ios::app);
    record << "=========================聊天记录==============================\n";
    record.close();
}

int main() {
    server s;
    s.StartServer();
    return 0;
}