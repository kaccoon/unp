#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* handle SIG_CHILD */
void sig_child(int signo) 
{
	pid_t pid;
	int stat;
	
	pid = wait(&stat);
	printf("Child %d terminated\n", pid);
	
	return;
}

/* Read string from socket and write back */
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
	struct sigaction act, oldact;

	/* Set up signal handler function */
	act.sa_handler = sig_child;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	/* If we don't set this flag, syscall 'accept' will not
	 * be restarted and return an EINTR */
	act.sa_flags |= SA_RESTART;

	if (sigaction(SIGCHLD, &act, &oldact) < 0) {
		perror(strerror(errno));
		exit(1);
	}

	/* Create socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&sa, sizeof(sa));
	bzero(&cli_sa, sizeof(cli_sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(4924);
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
		if ( (connfd = accept(listenfd, (struct sockaddr*)&cli_sa, &sa_len)) < 0) {
			// We are not sure if SA_RESTART is work as expected in all platform
			if (errno != EINTR) {	
				perror(strerror(errno));
				exit(1);
			} else {
				continue;
			}
		}

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
