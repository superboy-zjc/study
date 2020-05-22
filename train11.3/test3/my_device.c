#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include"advanced_device.h"

static int reg = 666;
//设备的清理方法，一般要求要写，即使什么也不做
static void my_release(struct device *dev)
{
	printk(KERN_INFO "In %s \n", __func__);
}
//初始化设备资源
static struct resource my_resource[] = {
	{
		.name = "设备寄存器地址",
		.start = &reg,
		.end = &reg + 0x04,
		.flags = IORESOURCE_MEM,
	}
};
//初始化设备对象，设备id号、声明名字、清理方法，总线在注册时会自动指明
static struct advanced_device my_dev = {
	.id = 1,
	.resource = my_resource,
	.dev = {
		.init_name = "jit_dev",
		.release = my_release,
	},
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello device module!\n");
	//注册自定义的设备，会自动指定总线
	ret = advanced_device_register(&my_dev);
	if (ret) {
		printk(KERN_ERR "dev_register error!\n");
		return ret;
	}
	return ret;
}
static void cleanup_hello(void)
{
	advanced_device_unregister(&my_dev);
	printk(KERN_INFO "Bye device module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
