#include "client.h"

int client::ConnectServer() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //判断套接字版本
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //创建套接字
    serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serversocket == INVALID_SOCKET) {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //输入服务器IP
    printf("Please input server ip:");
    char IP[32] = { 0 };
    cin>>IP;
//    char ip[32] = "127.0.0.1";
    //设置服务器地址
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);//服务器端口
    ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);//服务器地址

    printf("connecting......\n");
    //连接服务器
    if (SOCKET_ERROR == connect(serversocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr))) {
        printf("connect failed with error code: %d\n", WSAGetLastError());
        closesocket(serversocket);
        WSACleanup();
        return -1;
    }
    printf("Connecting server successfully ip:%s Port:%d\n",
           IP, htons(ServerAddr.sin_port));
    printf("Please input your UserName: ");
    cin >> userName;
    send(serversocket, userName, sizeof(userName), 0);
    printf("==============================按下ESC开始聊天吧============================== ");
    printf("\n\n");
    _beginthreadex(NULL, 0, ThreadRecv, &serversocket, 0, NULL); //启动接收和发送消息线程
    _beginthreadex(NULL, 0, ThreadSend, &serversocket, 0, NULL);
    for (int k = 0; k < 1000; k++)
        Sleep(10000000);
    closesocket(serversocket);
    WSACleanup();
    return 0;
}


int main() {
    client c;
    c.ConnectServer();
    return 0;
}


//unsigned long _beginthreadex( void *security, unsigned stack_size, unsigned ( __stdcall *start_address )( void * ), void *arglist, unsigned initflag, unsigned *thrdaddr );
//第1个参数：安全属性，NULL为默认安全属性
//第2个参数：指定线程堆栈的大小。如果为0，则线程堆栈大小和创建它的线程的相同。一般用0
//第3个参数：指定线程函数的地址，也就是线程调用执行的函数地址(用函数名称即可，函数名称就表示地址，注意的是函数访问方式一定是__stdcall，函数返回值一定是unsigned，函数参数一定是void*)
//第4个参数：传递给线程的参数的指针，可以通过传入对象的指针，在线程函数中再转化为对应类的指针
//第5个参数：线程初始状态，0:立即运行；CREATE_SUSPEND：悬挂（如果出事状态定义为悬挂，就要调用ResumeThread(HANDLE) 来激活线程的运行）
//第6个参数：用于记录线程ID的地址



