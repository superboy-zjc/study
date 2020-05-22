#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stropts.h>
#include<unistd.h>
int main()
{
	int fd;
	pid_t fork_fd;
	int ret = 0;
	char *wri = "Hello device!";
	char test2[2000];
	for(int i = 0;i < 200; i++) {
			test2[i] = 'a';
	}

	fd = open("/dev/testdev", O_WRONLY);
	if (fd < 0) {
			perror("open");
			goto error1;
	}
/*		fork_fd = fork();
		if(fork_fd < 0) {
				printf("fork error!\n");
				return -1;
		}
*/
	//调用驱动写函数
	//ret = write(fd, wri, strlen(wri) + 1);
	ret = write(fd, test2,  200);
	//驱动写函数 -1为error 其他为写入字节数
	if (-1 == ret) {
			perror("write: ");
	} else {
	printf("Write success!\n");
	}
	sleep(60);
error1:
close(fd);
	return 0;
}
