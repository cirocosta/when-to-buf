#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LISTEN_BACKLOG 128
#define MAXLINE 4096

int
main()
{
	int listen_fd;
	int conn_fd;
	pid_t child_pid;

	socklen_t client_len;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;

	ssize_t rn;
	char buf[MAXLINE];
	uint64_t bytes_read;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, sizeof(server_addr), 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1337);

	bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(listen_fd, LISTEN_BACKLOG);

	for (;;) {
		client_len = sizeof(client_addr);
		conn_fd = accept(
		  listen_fd, (struct sockaddr*)&client_addr, &client_len);

		if ((child_pid = fork()) == 0) {
			close(listen_fd);
			bytes_read = 0;
			rn = 0;

			while ((rn = read(conn_fd, buf, MAXLINE)) > 0) {
				bytes_read += rn;
				printf("bytes_read=%ld\n", bytes_read);
			}

			exit(0);
		}

		close(conn_fd);
	}

	return 0;
}
