#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/platform_device.h>

//初始化设备资源
static struct resource my_resource[] = {
	{
		.name = "设备寄存器地址",
		.start = 0x666666,
		.end = 0x888888,
		.flags = IORESOURCE_MEM,
	}
};
static struct platform_device pdev = {
		.name = "zjc_dev1",
		.resource = my_resource,
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello device module!\n");
		//注册platform_device设备,会默认指明platform_bus_type,并且自动填充release函数
		ret = platform_device_register(&pdev);
	return ret;
}
static void cleanup_hello(void)
{
	platform_device_unregister(&pdev);
	printk(KERN_INFO "Bye device module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
