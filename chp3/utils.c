#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * bind wild addr and temp port to sockfd
 * Return 0 for success, -1 for failed
**/
int sock_bind_wild(int sockfd, int family) {
	struct sockaddr_in ipv4_addr;
	struct sockaddr_in6 ipv6_addr;

	bzero(&ipv4_addr, sizeof(ipv4_addr));
	bzero(&ipv6_addr, sizeof(ipv6_addr));

	switch (family) {
		case AF_INET:
			ipv4_addr.sin_family = AF_INET;
			ipv4_addr.sin_addr.s_addr = INADDR_ANY;
			ipv4_addr.sin_port = htons(0);
			if (bind(sockfd, (struct sockaddr*)&ipv4_addr, sizeof(ipv4_addr)))
					return -1;
			else
					return 0;
		case AF_INET6:
			ipv6_addr.sin6_family = AF_INET6;
			// ipv6_addr.sin6_addr.s6_addr = IN6ADDR_ANY_INIT;
			// ipv4_addr.sin_port = ;
			if (bind(sockfd, (struct sockaddr*)&ipv6_addr, sizeof(ipv6_addr)))
					return -1;
			else
					return 0;
		default:
			return -1;
	}

}

/*
 * Compare two address, if they have same family and address,
 * return 0, else return none zero.
 */
int sock_cmp_addr(const struct sockaddr* sockaddr1, const struct sockaddr* sockaddr2, socklen_t addrlen) {
	sa_family_t family1 = sockaddr1->sa_family,
		family2 = sockaddr2->sa_family;

	if (family1 != family2) {
		return -1;
	}

	switch (family1) {
		case (AF_INET):	{	/* IPv4 */
			return (memcmp( &(((struct sockaddr_in*)sockaddr1)->sin_addr), 
							&(((struct sockaddr_in*)sockaddr2)->sin_addr), 
							sizeof(struct in_addr)));
		}
		case (AF_INET6): {
			return (memcmp( &(((struct sockaddr_in6*)sockaddr1)->sin6_addr), 
							&(((struct sockaddr_in6*)sockaddr2)->sin6_addr), 
							sizeof(struct in6_addr)));
		}
		default:
			return -1;
	}
}

int sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t addrlen) {
	if (sa1->sa_family != sa2->sa_family)
		return -1;

	switch (sa1->sa_family) {
		case (AF_INET): {
			return memcmp ( &((struct sockaddr_in*)sa1)->sin_port,
							&((struct sockaddr_in*)sa2)->sin_port,
							sizeof(in_port_t));
		}
		case (AF_INET6): {
			return memcmp ( &((struct sockaddr_in6*)sa1)->sin6_port,
							&((struct sockaddr_in6*)sa2)->sin6_port,
							sizeof(in_port_t));
		}
		default:
			return -1;
	}
}

/* Get port number for ipv4 & ipv6 */
int sock_get_port(const struct sockaddr *sa, socklen_t addrlen) {
	switch (sa->sa_family) {
		case (AF_INET):
			return ((struct sockaddr_in*)sa)->sin_port;
		case (AF_INET6):
			return ((struct sockaddr_in6*)sa)->sin6_port;
		default:
			return -1;
	}
}


char* sock_ntop_host(const struct sockaddr* sockaddr, socklen_t addrlen) {
	char *pa;
	switch (sockaddr->sa_family) {
		case (AF_INET): {
			pa = (char*)malloc(INET_ADDRSTRLEN);
			struct sockaddr_in *sa4 = (struct sockaddr_in*)sockaddr;
			return (char*)inet_ntop(AF_INET, (const void*)&sa4->sin_addr, pa, sizeof(pa));
		}
		case (AF_INET6): {
			pa = (char*)malloc(INET6_ADDRSTRLEN);
			struct sockaddr_in6 *sa6 = (struct sockaddr_in6*)sockaddr;
			return (char*)inet_ntop(AF_INET6, (const void*)&sa6->sin6_addr, pa, sizeof(pa));
		}
		default:
			return NULL;
	}
}

