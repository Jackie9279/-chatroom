

#include "server.h"

int server::StartServer() {
    //��ʼ���׽���
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //�жϰ汾
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //�����׽���
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET) {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }

    //���÷�������ַ
    ServerAddr.sin_family = AF_INET;//���ӷ�ʽ
    ServerAddr.sin_port = htons(uPort);//�����������˿�
    ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//�κοͻ��˶����������������

    //�󶨷�����
    if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr))) {
        printf("bind failed with error code: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        return -1;
    }
    //���ü����ͻ���������
    if (SOCKET_ERROR == listen(ServerSocket, 20000)) {
        printf("listen failed with error code: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }

    printtopbar();
    initrecordfile();

    _beginthreadex(NULL, 0, ThreadAccept, NULL, 0, 0);
    for (int k = 0; k < 100; k++) //�����߳�����
        Sleep(99999999);

    //�ر��׽���
    for (int j = 0; j < 2; j++) {
        if (Clientlist[j].clientsocket != INVALID_SOCKET)
            closesocket(Clientlist[j].clientsocket);
    }
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}

void server::printtopbar() {
    cout << "=========================���������������==============================" << endl;
    cout << "========================�����¼������c��record.txt====================" << endl;
}

void server::initrecordfile() {
    ofstream x("c://record.txt", ios_base::out | ios_base::trunc);     //ɾ����ǰ����
    x.close();
    ofstream record;
    record.open("c://record.txt", ios::out | ios::app);
    record << "=========================�����¼==============================\n";
    record.close();
}

int main() {
    server s;
    s.StartServer();
    return 0;
}