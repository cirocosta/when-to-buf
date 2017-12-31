#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./conn.h"

#define LISTEN_BACKLOG 128
#define MAXLINE 4096
#define SRC_BUFSIZE (1 << 20)

char SOURCE_BUFFER[SRC_BUFSIZE] = { 0 };

int
init_client_conn(t_conn* connection, char* addr, int port)
{
	int sock_fd;
	int err;
	struct sockaddr_in server_addr;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("failed to create AF_INET SOCK_STREAM socket - socket");
		return 1;
	}

	memset(&server_addr, sizeof(server_addr), 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	err = inet_pton(AF_INET, addr, &server_addr.sin_addr);
	if (err != 1) {
		printf("invalid address for SOCK_STREAM: %s\n", addr);
		return 1;
	}

	err =
	  connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err == -1) {
		perror("connect");
		printf("failed to connect to %s:%d\n", addr, port);
		return 1;
	}

	connection->fd = sock_fd;
	connection->rx = fdopen(sock_fd, "r");
	if (connection->rx == NULL) {
		perror("failed to associate socket with rx stream - fdopen");
		return 1;
	}

	connection->tx = fdopen(dup(sock_fd), "w");
	if (connection->tx == NULL) {
		perror("failed to associate socket with tx stream - fdopen");
		return 1;
	}

	return 0;
}

int
work_on_connection(t_conn* connection, int bufsize)
{
	size_t n = 0;

	if (bufsize > 0) {
		setvbuf(connection->tx, NULL, _IOFBF, bufsize);
	} else {
		setvbuf(connection->tx, NULL, _IONBF, bufsize);
	}

	n = fwrite(connection->tx, sizeof(char), SRC_BUFSIZE, connection->tx);
	if (n == 0) {
		perror("failed to write BUFSIZE bytes - fwrite");
		return 1;
	}

	return 0;
}

int
main(int argc, char** argv)
{
	if (argc != 3) {
		printf("ERROR: two arguments must be supplied.\n");
		printf("Usage:  ./client <address> <tx_bufsize>\n");
		exit(1);
	}

	int err = 0;
	char* addr = argv[1];
	char* bufsize = argv[2];

	t_conn connection = { 0 };

	err = init_client_conn(&connection, addr, 1337);
	if (err == 1) {
		printf("failed to initialize connection.\n");
		exit(1);
	}

	work_on_connection(&connection, atoi(bufsize));

	destroy_conn(&connection);
	return 0;
}
