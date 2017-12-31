#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "./conn.h"

#define LISTEN_BACKLOG 128
#define MAXLINE 4096

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

int
work_on_connection(t_conn* connection)
{
        int null_fd = open("/dev/null", O_WRONLY);
        int run = 1;
        int err = 0;

        while (run) {
                err = sendfile(null_fd, connection->fd, NULL, 1024);
                if (err == -1) {
                        perror("failed to transmit bytes to null fd - sendfile");
                        close(null_fd);
                        return 1;
                }
        }

        close(null_fd);
        return 0;
}

int
main()
{
	int listen_fd;
	struct sockaddr_in server_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, sizeof(server_addr), 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1337);

	bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(listen_fd, LISTEN_BACKLOG);

	for (;;) {
		t_conn connection = { 0 };
		init_server_conn(&connection, listen_fd);
		work_on_connection(&connection);
		destroy_conn(&connection);
	}

	return 0;
}
