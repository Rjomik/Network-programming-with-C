#if defined(_WIN32)
	#pragma comment(lib, "ws2_32.lib")
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0600
	#endif
	#include <WinSock2.h>
	#include <WS2tcpip.h>

	#define ISVALIDSOCKET(x) ((x) != INVALID_SOCKET)
	#define CLOSESOCKET(x) (closesocket(x))
	#define GETERRORCODE (WSAGetLastError())
#else

#endif
#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize windows sockets api\n");
		return -1;
	}
#endif
	//addr
	printf("Configuring local address...\n");
	ADDRINFOA hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	ADDRINFOA* bindAddress;
	getaddrinfo(0, "8080", &hints, &bindAddress);

	//socket
	printf("Creating socket...\n");
	SOCKET serverSocket = socket(bindAddress->ai_family,bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (!ISVALIDSOCKET(serverSocket))
	{
		fprintf(stderr, "Failed to create a socket. Code: %d\n", GETERRORCODE);
		return -2;
	}
	//bind
	printf("Binding socket to local address...\n");
	if (bind(serverSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
	{
		fprintf(stderr, "Failed to bind to a port. Code: %d\n", GETERRORCODE);
		return -3;
	}
	freeaddrinfo(bindAddress);
	//listen
	printf("Listening...\n");
	if (listen(serverSocket, 10) < 0)
	{
		fprintf(stderr, "Failed to enter listen mode. Code: %d\n", GETERRORCODE);
		return -4;
	}
	//accept
	printf("Waiting for incoming connection...\n");
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	SOCKET clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientAddrLen);
	if (!ISVALIDSOCKET(clientSocket))
	{
		fprintf(stderr, "Failed to initiate connection. Code: %d\n", GETERRORCODE);
		return -4;
	}
	
	//print client to console
	char buff[1024];
	getnameinfo((struct sockaddr*)&clientAddr, clientAddrLen, buff, sizeof(buff), 0, 0, NI_NUMERICHOST);
	printf("Client connected: %s\n", buff);

	//get request
	printf("Reading request...\n");
	int bytesReceived = recv(clientSocket, buff, sizeof(buff), 0);
	if (bytesReceived < 1)
	{
		fprintf(stderr, "Failed to receive the first request. Code: %d\n", GETERRORCODE);
		return -5;
	}
	printf("Got a request of %d bytes: %.*s",bytesReceived, bytesReceived, buff);

	//Http response
	printf("Sending response...\n");
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Connection-Type: text/plain\r\n\r\n"
		"Local time is: ";
	int bytesSend = send(clientSocket, response, strlen(response), 0);
	if (bytesSend < 1)
	{
		fprintf(stderr, "Failed to send the prefix response. Code: %d\n", GETERRORCODE);
		return -6;
	}
	printf("Sent %d bytes of %d bytes\n", bytesSend, (int)strlen(response));
	//time suffix
	time_t timer;
	time(&timer);
	char* timeString = ctime(&timer);
	bytesSend = send(clientSocket, timeString, strlen(timeString), 0);
	if (bytesSend < 1)
	{
		fprintf(stderr, "Failed to send the suffix response. Code: %d\n", GETERRORCODE);
		return -7;
	}
	printf("Sent %d bytes of %d bytes\n", bytesSend, (int)strlen(timeString));

	//terminating
	printf("Closing client socket...\n");
	CLOSESOCKET(clientSocket);
	printf("Sending listening socket...\n");
	CLOSESOCKET(serverSocket);

#if defined(_WIN32)
	WSACleanup();
#endif
}