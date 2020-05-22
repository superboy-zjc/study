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

	fd = open("/dev/testdev", O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	} else {
//			while(1) {
					ret = read(fd, a, 20); 
					if (-1 == ret) {
							printf("read error! errno = %d \n", ret);
					}
					printf("ret = %d", ret);
	printf("File content: ");
					for(int i = 0; i < ret ; i ++)
							printf("%c", a[i]);
			}
	printf("\n");
//	}
	close(fd);
	return 0;
}
