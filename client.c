#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LISTEN_BACKLOG 128
#define MAXLINE 4096

int
main(int argc, char** argv)
{
	int sock_fd;
	FILE* conn;
	struct sockaddr_in server_addr;

	if (argc == 1) {
		printf("ERROR: an argument must be supplied.\n");
		printf("Usage:  ./client <address>\n");
		exit(1);
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&server_addr, sizeof(server_addr), 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1337);

	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	conn = fdopen(sock_fd, "w+");
	if (conn == NULL) {
		printf("failed to associate socket with stream\n");
		perror("fdopen");
		exit(1);
	}

	fprintf(conn, "loooooooool");

	fclose(conn);

	return 0;
}
