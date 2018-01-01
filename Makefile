CFLAGS := -D_GNU_SOURCE -O2

all: build

build:
	gcc $(CFLAGS) -o ./client ./client.c
	gcc $(CFLAGS) -o ./server ./server.c

fmt:
	find . -name "*.c" -o -name "*.h" | \
		xargs clang-format -style=file -i

clean:
	rm ./client
	rm ./server

.PHONY: build fmt clean

