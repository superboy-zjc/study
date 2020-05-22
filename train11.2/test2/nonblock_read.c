#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stropts.h>
int main()
{
	int fd;
	int ret = 0;
	char a[500];

	fd = open("/dev/testdev", O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	ret = read(fd, a, 13 + 1); 
	if (ret < 0) {
			perror("read");
	} else {
		printf("File content: %s\n", a);
	}

	close(fd);
	return 0;
}
