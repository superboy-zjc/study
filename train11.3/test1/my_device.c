#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>

//设备的清理方法，一般要求要写，即使什么也不做
static void my_release(struct device *dev)
{
	printk(KERN_INFO "In %s \n", __func__);
}
//从内核中导入我们需要的总线对象
extern struct bus_type my_bus;
//初始化设备对象，声明名字、总线对象与清理方法
static struct device my_dev = {
	.init_name = "jit_dev",
	.bus = &my_bus,
	.release = my_release,
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello device module!\n");
	//注册设备
	ret = device_register(&my_dev);
	if (ret) {
		printk(KERN_ERR "dev_register error!\n");
		return ret;
	}
	return ret;
}
static void cleanup_hello(void)
{
	device_unregister(&my_dev);
	printk(KERN_INFO "Bye device module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
