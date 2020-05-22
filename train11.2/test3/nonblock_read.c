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

	fd = open("/dev/testdev", O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		goto error1;
	}
	ret = read(fd, a, 20); 
	if (ret < 0) {
			perror("read");
			goto error1;
	} else {
		printf("File content: %s\n", a);
	}
	sleep(1);
error1:
	close(fd);
		exit(EXIT_FAILURE);
	return 0;
}
