#if defined(_WIN32)
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