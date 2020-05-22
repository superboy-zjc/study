obj-m += spinlock_dev.o
obj-m += mypipe.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc block_read.c -o block_read
	gcc nonblock_read.c -o nonblock_read
	gcc block_write.c -o block_write
	gcc nonblock_write.c -o nonblock_write
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm block_write nonblock_write block_read nonblock_read
