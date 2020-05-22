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
		//** close open 让文件内指针指到文件头 **//
		close(fd);
		fd = open("/dev/testdev", O_RDWR);
		//////////////////////////////////////////
		//调用设备读函数，错误返回-1，正确返回读出字节数
		ret = read(fd, a, strlen(wri) + 1); 
		if (-1 == ret) {
			printf("read error! errno = %d \n", ret);
		}
		printf("File content: %s\n", a);
	}
	//调用ioctl设备控制函数，我们定义cmd为0时，清空文件内容
	ioctl(fd, 0);
	//** close open 让文件内指针指到文件头 **//
	close(fd);
	fd = open("/dev/testdev", O_RDWR);
	//////////////////////////////////////////
	//测试是否清空
	ret = read(fd, a, strlen(wri) + 1); 
	if (-1 == ret) {
		printf("read error! errno = %d \n", ret);
	}
	close(fd);
	printf("File content: %s\n", a);
	return 0;
}
