#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <errno.h>

int main(int argc, char** argv)
{
	int listenfd, connfd;
	int i;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char buff[LINE_MAX];
	time_t ticks;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		fprintf(stderr, "sock error");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "bind error\n");
		perror(strerror(errno));
		exit(1);
	}

	if (listen(listenfd, 1024) < 0) {
		fprintf(stderr, "listen error");
		exit(1);
	}

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
		printf("Connection from %s, port %d\n", 
			inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
			ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		for (i = 0; i < strlen(buff); i++) {
			write(connfd, &buff[i], 1);
			//sleep(1);
		}
		
		printf("Output bytes: %d\n", i);
		close(connfd);
	}
}

