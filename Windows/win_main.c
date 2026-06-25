#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>


#include <stdio.h>
#include <stdlib.h>


int main()
{
	printf("hello!\n");
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("Failed to initialize windows sockets api");
		return -1;
	}
	PIP_ADAPTER_ADDRESSES firstAdapter;
	ULONG adapterSize = 20000;
	while (1)
	{
		firstAdapter = malloc(adapterSize);
		if (firstAdapter == NULL)
			return -1;
		int res = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, firstAdapter, &adapterSize);
		if (res == ERROR_BUFFER_OVERFLOW)
		{
			free(firstAdapter);
			continue;
		}
		else if (res == ERROR_SUCCESS)
		{
			break;
		}
		else
		{
			free(firstAdapter);
			return -1;
		}
	}
	
	PIP_ADAPTER_ADDRESSES current = firstAdapter;
	while (current)
	{
		printf("AdapterName: %S\n", current->FriendlyName);
		PIP_ADAPTER_UNICAST_ADDRESS currAddress = current->FirstUnicastAddress;
		while (currAddress)
		{
			printf("\t%s", currAddress->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");
			char ap[100];
			getnameinfo(currAddress->Address.lpSockaddr, currAddress->Address.iSockaddrLength, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
			currAddress = currAddress->Next;
		}
		current = current->Next;
	}
	free(firstAdapter);
	WSACleanup();
	printf("OK");
	return 0;
}