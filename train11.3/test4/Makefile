#obj-m += my_bus.o
obj-m += my_driver.o
obj-m += my_device.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#	sudo insmod my_bus.ko
	sudo insmod my_driver.ko
	sudo insmod my_device.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod my_device
	sudo rmmod my_driver
#	sudo rmmod my_bus
