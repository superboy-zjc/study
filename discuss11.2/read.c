#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stropts.h>
int main()
{
	int fd;
	int ret = 0;
	char a[500];

	fd = open("/dev/myBR_file", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	} else {
			ret = read(fd, a, 13 + 1); 
		if (-1 == ret) {
			printf("read error! errno = %d \n", ret);
		}
	}
	close(fd);
	printf("File content: %s\n", a);
	return 0;
}
