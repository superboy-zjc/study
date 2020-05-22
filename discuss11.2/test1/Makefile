obj-m += hello_char.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc block_read.c -o block_read
	gcc nonblock_read.c -o nonblock_read
	gcc write.c -o write
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm write block_read nonblock_read
