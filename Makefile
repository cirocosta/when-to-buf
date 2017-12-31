all: build

build:
	gcc -O2 -o ./client ./client.c
	gcc -O2 -o ./server ./server.c

clean:
	rm ./client
	rm ./server

.PHONY: build

