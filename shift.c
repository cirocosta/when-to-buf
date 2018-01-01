#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
	if (argc != 3) {
		printf("ERROR: Unexpected number of arguments.\n");
		printf("Usage:  ./shift <number> <shifts>.\n");
		exit(1);
	}

	int number = atoi(argv[1]);
	int shifts = atoi(argv[2]);

	printf("%d\n", (number << shifts));

	return 0;
}
