#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

void str_cli(int sockfd)
{
	char sendbuf[LINE_MAX], recvbuf[LINE_MAX];
	while (fgets(sendbuf, sizeof(sendbuf), stdin)) {
		write(sockfd, sendbuf, strlen(sendbuf));
		if (read(sockfd, recvbuf, sizeof(recvbuf)) == 0) {
			fprintf(stderr, "server terminated prematurely");
		}
		puts(recvbuf);
	}
}

int main(int argc, char* argv[]) 
{
	int connfd;
	struct sockaddr_in sa;
	char buf[1024];
	ssize_t bytes_read, bytes_echo;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s address port\n", argv[0]);
		exit(1);
	}

	/* Fill server address */
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	if (!inet_pton(AF_INET, argv[1], &sa.sin_addr.s_addr)) {
		perror(strerror(errno));
		exit(1);
	}
	sa.sin_port = htons(atoi(argv[2]));

	/* Create socket */
	connfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server */
	if (connect(connfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
		perror(strerror(errno));
		close(connfd);
		exit(1);
	}

	str_cli(connfd);
	close(connfd);
	return 0;
}
