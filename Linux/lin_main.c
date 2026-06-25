#define _GNU_SOURCE

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
	printf("hello\n");
	struct ifaddrs *addresses;
	if (getifaddrs(&addresses) == -1)
	{
		perror("Failed to getifaddr()");
		return -1;
	}

	struct ifaddrs *curr = addresses;
	while (curr)
	{
		int family = curr->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6)
		{
			printf("%s\t", curr->ifa_name);
			printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
			char ap[100];
			const int familySize = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
			getnameinfo(curr->ifa_addr, familySize, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
		}
		curr = curr->ifa_next;
	}

	freeifaddrs(addresses);
	return 0;
}