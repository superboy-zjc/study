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
	char *wri = "Hello device!";

	fd = open("/dev/testdev", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	} else {
		//调用驱动写函数
		ret = write(fd, wri, strlen(wri) + 1);
		//驱动写函数 -1为error 其他为写入字节数
		if (-1 == ret) {
			printf("write error! errno = %d\n", ret);
		}
		printf("Write success!\n");
	}
	close(fd);
	return 0;
}
