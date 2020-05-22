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
	pid_t fork_fd;
	int ret = 0;
	char *wri = "Hello device!";

	fd = open("/dev/testdev", O_RDWR);
	if (fd < 0) {
			perror("open");
			exit(EXIT_FAILURE);
	}
		fork_fd = fork();
		if(fork_fd < 0) {
				printf("fork error!\n");
				return -1;
		}

	//调用驱动写函数
	ret = write(fd, wri, strlen(wri) + 1);
	//驱动写函数 -1为error 其他为写入字节数
	if (-1 == ret) {
			printf("write error! errno = %d\n", ret);
	}
	printf("Write success!\n");

	return 0;
}
