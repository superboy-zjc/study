#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stropts.h>
#include <sys/mman.h>
int main()
{
	int fd;
	int ret = 0;
	char test[1000];
	char* map_addr = NULL;

	fd = open("/dev/testdev", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	} else {
		map_addr = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_SHARED, 
				fd, 0);
		if (map_addr == MAP_FAILED) {
			printf("map error!\n");
			return -1;
		}
	}
	strcpy(map_addr, "Driver test");
	strcpy(test, map_addr);
	printf("File content: %s\n", map_addr);
	munmap(map_addr, 1000);
	close(fd);
	return 0;
}
