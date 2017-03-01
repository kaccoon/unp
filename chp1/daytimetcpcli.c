#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

int main(int argc, char** argv)
{
	int sockfd, n;
	int count;
	char recvline[LINE_MAX + 1];
	char buf[1024];
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(cliaddr);

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//sockfd = socket(9999, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "sock error: %d\n", errno);
		perror(strerror(errno));
		exit(1);
	}

	/* set address */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	if (inet_pton(AF_INET, argv[1], &(servaddr.sin_addr)) <= 0) {
		fprintf(stderr, "inet_pton error");
		exit(1);
	}

	/* connect to server */
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "connect error: %d %s\n", errno, strerror(errno));
		exit(1);
	}

	if (getsockname(sockfd, (struct sockaddr*)&cliaddr, &len) < 0) {
		fprintf(stderr, "Get sock name error: %d %s\n", errno, strerror(errno));
		exit(1);
	} else {
		printf("cli address: %s\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)));
		printf("sock port: %d\n", ntohs(cliaddr.sin_port));
	}

	/* read */
	count = 0;
	while ( (n = read(sockfd, recvline, LINE_MAX)) > 0 ) {
		count++;
		recvline[n] = 0;
		if (fputs(recvline, stdout) == EOF) {
			fprintf(stderr, "fputs error");
			exit(1);
		}
	}

	if (n < 0) {
		fprintf(stderr, "read error");
		exit(1);
	}

	printf("Count: %d\n", count);
	return 0;
}
