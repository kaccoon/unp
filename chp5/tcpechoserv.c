#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void str_echo (int sockfd)
{
	char buf[LINE_MAX];
	ssize_t bytes_read;

again:
	while ((bytes_read = read(sockfd, buf, LINE_MAX)) > 0) {
		printf("bytes_read = %d\n", bytes_read);
		write(sockfd, buf, bytes_read);
	}

	if (bytes_read < 0 && errno == EINTR)
		goto again;
	else if (bytes_read < 0)
		perror(strerror(errno));

	return;
}

int main(int argc, char *argv[]) 
{
	int listenfd, connfd;
	struct sockaddr_in sa, cli_sa;
	int sa_len;
	char buf[1024];
	pid_t c_pid;

	/* Create socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&sa, sizeof(sa));
	bzero(&cli_sa, sizeof(cli_sa));
	sa.sin_family = AF_INET;
	sa.sin_port = 4924;
	sa.sin_addr.s_addr = INADDR_ANY;

	/* bind address and port */
	if (bind(listenfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
		perror(strerror(errno));
		exit(1);
	}

	/* listen on socket */
	if (listen(listenfd, 10) < 0) {
		perror(strerror(errno));
		exit(1);
	}

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*)&cli_sa, &sa_len);
		c_pid = fork();
		if (c_pid == 0) {	/* child */
			str_echo(connfd);
			close(connfd);
			exit(0);
		} else {			/* parent */
			close(connfd);
		}
	}

	return 0;
}
