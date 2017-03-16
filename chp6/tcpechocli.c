#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

void str_cli(int fd)
{
	int bytes, ready_num;
	int write_flag = 1;
	char sendbuf[1024], recvbuf[1024];

	fd_set read_set;

	while(1) {
		FD_ZERO(&read_set);
		FD_SET(fd, &read_set);
		if (write_flag)
			FD_SET(fileno(stdin), &read_set);

		select(fd+1, &read_set, NULL, NULL, NULL);

		if (FD_ISSET(fileno(stdin), &read_set)) {
			if (!fgets(sendbuf, sizeof(sendbuf), stdin)) {
				shutdown(fd, SHUT_WR);
				write_flag = 0;
			} else {
				write(fd, sendbuf, strlen(sendbuf));
			}
		}

		if (FD_ISSET(fd, &read_set)) {
			bytes = read(fd, recvbuf, sizeof(recvbuf));
			if (bytes > 0) {
				recvbuf[bytes >= sizeof(recvbuf) ? sizeof(recvbuf) - 1 : bytes] = '\0';
				printf("%s", recvbuf);
			} else if (bytes == 0) {
				if (write_flag == 0) {
					return;
				} else {
					fprintf(stderr, "server terminated prematurely");
					return;
				}
			}
		}
	}
}

int main(int argc, char* argv[])
{
	int fd;
	struct sockaddr_in sa;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s address port\n", argv[0]);
		exit(1);
	}

	/* set up address */
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &sa.sin_addr.s_addr) != 1) {
		perror(strerror(errno));
		exit(1);
	}
	sa.sin_port = htons(atoi(argv[2]));

	/* create socket */
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror(strerror(errno));
		exit(1);
	}

	if (connect(fd, (const struct sockaddr*)&sa, sizeof(sa))) {
		perror(strerror(errno));
		close(fd);
		exit(1);
	}

	str_cli(fd);

	return 0;
}
