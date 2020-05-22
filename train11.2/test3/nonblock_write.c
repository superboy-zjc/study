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
	char test2[200];

	fd = open("/dev/testdev", O_WRONLY | O_NONBLOCK);
	if (fd < 0) {
			perror("open");
			exit(EXIT_FAILURE);
	}
	for(int i = 0; i < 200; i ++) {
			test2[i] = 'a';
	}
	//调用驱动写函数
//	ret = write(fd, test2, 200);
	ret = write(fd, wri, 20);
	//驱动写函数 -1为error 其他为写入字节数
	if (0 > ret) {
			perror("write error!\n");
	} else{
				printf("Write success!\n");
	}
	sleep(1);
close(fd);
	return 0;
}
