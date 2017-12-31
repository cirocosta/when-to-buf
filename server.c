#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./conn.h"

#define DST_BUFSIZE (1 << 20)
#define LISTEN_BACKLOG 128

int
init_server_conn(t_conn* connection, int listen_fd)
{
	int conn_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	conn_fd =
	  accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
	if (conn_fd == -1) {
		perror("failed to accept connection - accept");
		return 1;
	}

	connection->fd = conn_fd;
	connection->rx = fdopen(conn_fd, "r");
	if (connection->rx == NULL) {
		perror("failed to associate socket with rx stream - fdopen");
		return 1;
	}

	connection->tx = fdopen(dup(conn_fd), "w");
	if (connection->tx == NULL) {
		perror("failed to associate socket with tx stream - fdopen");
		return 1;
	}

	return 0;
}

char dest_buf[DST_BUFSIZE] = { 0 };

int
work_on_connection(t_conn* connection, int bufsize)
{
	size_t n;
	int run = 1;

	if (bufsize > 0) {
		setvbuf(connection->rx, NULL, _IOFBF, bufsize);
	} else {
		setvbuf(connection->rx, NULL, _IONBF, bufsize);
	}

	while (run) {
		n = fread(dest_buf, sizeof(char), DST_BUFSIZE, connection->rx);
		if (n == 0) {
			if (feof(connection->rx)) {
				return 0;
			}

			perror("failed to read contents - fread");
			return 1;
		}

		printf("read=%ld\n", n);
	}

	return 0;
}

int
main(int argc, char** argv)
{
	int listen_fd;
	int err = 0;
	struct sockaddr_in server_addr;
	char* bufsize = argv[1];

	if (argc != 2) {
		printf("ERROR: an argument must be supplied.\n");
		printf("Usage: ./server <rx_bufsize>\n");
		exit(1);
	}

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, sizeof(server_addr), 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1337);

	int reuse = 1;
	if (setsockopt(listen_fd,
	               SOL_SOCKET,
	               SO_REUSEADDR,
	               (const char*)&reuse,
	               sizeof(reuse)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
	if (setsockopt(listen_fd,
	               SOL_SOCKET,
	               SO_REUSEPORT,
	               (const char*)&reuse,
	               sizeof(reuse)) < 0)
		perror("setsockopt(SO_REUSEPORT) failed");
#endif

	err =
	  bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err == -1) {
		perror("failed to bind to port 1337 - bind");
		exit(1);
	}

	err = listen(listen_fd, LISTEN_BACKLOG);
	if (err == -1) {
		perror("failed to transition socket to passive state - listen");
		exit(1);
	}

	for (;;) {
		t_conn connection = { 0 };
		init_server_conn(&connection, listen_fd);
		work_on_connection(&connection, atoi(bufsize));
		destroy_conn(&connection);
	}

	return 0;
}
