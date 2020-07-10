#include "client.h"

int client::ConnectServer() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //�ж��׽��ְ汾
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //�����׽���
    serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serversocket == INVALID_SOCKET) {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //���������IP
    printf("Please input server ip:");
    char IP[32] = { 0 };
    cin>>IP;
//    char ip[32] = "127.0.0.1";
    //���÷�������ַ
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);//�������˿�
    ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);//��������ַ

    printf("connecting......\n");
    //���ӷ�����
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
    printf("==============================����ESC��ʼ�����============================== ");
    printf("\n\n");
    _beginthreadex(NULL, 0, ThreadRecv, &serversocket, 0, NULL); //�������պͷ�����Ϣ�߳�
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
//��1����������ȫ���ԣ�NULLΪĬ�ϰ�ȫ����
//��2��������ָ���̶߳�ջ�Ĵ�С�����Ϊ0�����̶߳�ջ��С�ʹ��������̵߳���ͬ��һ����0
//��3��������ָ���̺߳����ĵ�ַ��Ҳ�����̵߳���ִ�еĺ�����ַ(�ú������Ƽ��ɣ��������ƾͱ�ʾ��ַ��ע����Ǻ������ʷ�ʽһ����__stdcall����������ֵһ����unsigned����������һ����void*)
//��4�����������ݸ��̵߳Ĳ�����ָ�룬����ͨ����������ָ�룬���̺߳�������ת��Ϊ��Ӧ���ָ��
//��5���������̳߳�ʼ״̬��0:�������У�CREATE_SUSPEND�����ң��������״̬����Ϊ���ң���Ҫ����ResumeThread(HANDLE) �������̵߳����У�
//��6�����������ڼ�¼�߳�ID�ĵ�ַ



