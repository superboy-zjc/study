obj-m += hello_char.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc read.c -o read
	gcc write.c -o write
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm write read
