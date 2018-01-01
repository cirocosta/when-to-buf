#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./conn.h"
#include "./timing.h"

#define SRC_BUFSIZE (1 << 30)

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
	int err = 0;
	uint64_t to_write = bufsize;
	uint64_t total_written = 0;

	char* write_buf;

	if (bufsize > 0) {
		write_buf = malloc(bufsize * sizeof(char));
		if (write_buf == NULL) {
			perror("failed to allocate write_buf");
			return 1;
		}

		err = setvbuf(connection->tx, write_buf, _IOFBF, bufsize);
		if (err != 0) {
			perror("failed to set block buffer - setvbuf");
			return 1;
		}
	} else {
		err = setvbuf(connection->tx, NULL, _IONBF, bufsize);
		if (err != 0) {
			perror("failed to remove buffering - setvbuf");
			return 1;
		}
	}

	for (;;) {
		if (total_written + bufsize > SRC_BUFSIZE) {
			to_write = SRC_BUFSIZE - total_written;
		}

		n = fwrite(SOURCE_BUFFER + total_written,
		           sizeof(char),
		           to_write,
		           connection->tx);
		if (n == 0) {
			perror("failed to write BUFSIZE bytes - fwrite");
			return 1;
		}

		total_written += n;

		if (total_written == SRC_BUFSIZE) {
			return 0;
		}
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
	t_timing timing = {
		.start = (struct timeval){}, .end = (struct timeval){},
	};

	err = init_client_conn(&connection, addr, 1337);
	if (err == 1) {
		printf("failed to initialize connection.\n");
		exit(1);
	}

	timing_start(&timing);
	work_on_connection(&connection, atoi(bufsize));
	timing_finish(&timing);

	printf("%s,%d\n", bufsize, timing_get_elapsed(&timing));

	destroy_conn(&connection);
	return 0;
}
