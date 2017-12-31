all: build

build:
	gcc -O2 -o ./client ./client.c
	gcc -O2 -o ./server ./server.c

fmt:
		find . -name "*.c" -o -name "*.h" | \
			xargs clang-format -style=file -i

clean:
	rm ./client
	rm ./server

.PHONY: build fmt clean
 
