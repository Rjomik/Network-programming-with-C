#include "TcpServerClient.h"
int dummyClient()
{
#if defined(_WIN32)
    WSADATA d;
    WSAStartup(MAKEWORD(2, 2), &d);
#endif
    printf("Enter host name or IP to connect to\n");
    char hostName[50];
    fgets(hostName, sizeof(hostName), stdin);
    printf("Enter port or service\n");
    char port[50];
    fgets(port, sizeof(port), stdin);
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo* dst;
    printf("Parsing anddress\n");
    hostName[strcspn(hostName, "\r\n")] = 0;
    port[strcspn(port, "\r\n")] = 0;
    if(getaddrinfo(hostName, port, &hints, &dst))
    {
        printf("Fail:%d", GETLASTERROR);
        return -1;
    }
    printf("Creating socket object\n");
    SOCKET sock = socket(dst->ai_family, dst->ai_socktype, dst->ai_protocol);
    if(sock < 1)
    {
        printf("Fail:%d", GETLASTERROR);
        return -2;
    }
    printf("Connecting to remote host\n");
    if(connect(sock, dst->ai_addr, dst->ai_addrlen))
    {
        printf("Fail:%d", GETLASTERROR);
        return -3;
    }
    freeaddrinfo(dst);
    printf("You have connected to whatever, start typing...\n");
    int largest = sock;
    fd_set setMaster;
    FD_ZERO(&setMaster);
    FD_SET(sock, &setMaster);
#if !defined(_WIN32)
    FD_SET(fileno(stdin), &setMaster);
#endif
    while(1)
    {
        fd_set copy = setMaster;
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        select(largest + 1, &copy, 0, 0, &tv);
#if defined(_WIN32)
        if(_kbhit())
#else
        if (FD_ISSET(fileno(stdin), &copy))
#endif
        {
            char consoleInput[1024];
            fgets(consoleInput, sizeof(consoleInput), stdin);
            send(sock, consoleInput, strlen(consoleInput), 0);
            if(consoleInput[0] == 'q')
                break;
        }
        if(FD_ISSET(sock, &copy))
        {
            char recvBuffer[4096];
            int got = (int)recv(sock, recvBuffer, sizeof(recvBuffer), 0);
            if (got < 1)
            {
                printf("Remote cosed\n");
                break;
            }
                
            printf("Got a message:\n%.*s", (int)got, recvBuffer);
        }
    }
    printf("leaving...");
    CLOSESOCKET(sock);
#if defined(_WIN32)
    WSACleanup();
#endif
    return 0;
}