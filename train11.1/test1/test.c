#include <stdio.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
	int fd;

	fd = open("zjcchar", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	} else {
		
	}
	return 0;
}
