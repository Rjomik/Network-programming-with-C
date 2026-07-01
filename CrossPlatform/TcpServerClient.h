#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#define CLOSESOCKET(x) (closesocket(x))
#define GETLASTERROR  (WSAGetLastError())
#else
#define _GNU_SOURCE
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    #define SOCKET int
    #define CLOSESOCKET(x) (close(x))
    #define GETLASTERROR  errno
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dummyClient();
int dummyServer();