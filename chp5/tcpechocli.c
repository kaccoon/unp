#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void sig_pip(int signo)
{
	printf("Receive SIG_PIP\n");
	exit(1);
}

void str_cli(int sockfd)
{
	char sendbuf[1024], recvbuf[1024];
	int read_bytes;
	while (fgets(sendbuf, sizeof(sendbuf), stdin)) {
		write(sockfd, sendbuf, 1);
		sleep(1);
		write(sockfd, sendbuf + 1, strlen(sendbuf) - 1);
		sleep(1);
		if ((read_bytes = read(sockfd, recvbuf, sizeof(recvbuf)-1)) == 0) {
			fprintf(stderr, "server terminated prematurely");
		}
		recvbuf[read_bytes] = '\0';
		fputs(recvbuf, stdout);
	}
}

int main(int argc, char* argv[]) 
{
	int connfd;
	int fds[5];
	int i;
	struct sockaddr_in sa;
	char buf[1024];
	ssize_t bytes_read, bytes_echo;

	struct sigaction act;
	act.sa_handler = sig_pip;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGPIPE, &act, NULL) < 0) {
		perror(strerror(errno));
		exit(1);
	}

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
	for (i=0; i<1; i++) {
		fds[i] = socket(AF_INET, SOCK_STREAM, 0);

			/* Connect to server */
			if (connect(fds[i], (struct sockaddr*)&sa, sizeof(sa)) < 0) {
				perror(strerror(errno));
				close(fds[i]);
				exit(1);
			}
	}

	str_cli(fds[0]);
	//close(connfd);
	exit(0);
}
