#ifndef H__WHEN_TO_BUF
#define H__WHEN_TO_BUF

#include <stdio.h>

typedef struct conn {
	FILE* rx;
	FILE* tx;
	int fd;
} t_conn;

void
destroy_conn(t_conn* conn)
{
	if (conn == NULL) {
		return;
	}

	if (conn->tx != NULL) {
		fclose(conn->tx);
		conn->tx = NULL;
	}

	if (conn->rx != NULL) {
		fclose(conn->rx);
		conn->rx = NULL;
	}

	return;
}

#endif // ! H__WHEN_TO_BUF
