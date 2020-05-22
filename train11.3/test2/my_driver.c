#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include "advanced_device.h"

//从内核中导入我们需要的总线对象
extern struct bus_type my_bus;
//总线匹配成功驱动与设备后，会启动驱动的初始化函数,probe(也称探测函数)
static int my_probe(struct device *dev)
{
	printk(KERN_INFO "In %s \n", __func__);
	return 0;
}
static int my_remove(struct device *dev)
{
	printk(KERN_INFO "In %s \n", __func__);
	return 0;
}
//初始化自定义的驱动对象，声明支持的设备号表、名字、探测函数方法与卸载方法,会在注册时指定总线
static struct advanced_driver my_drv = {
	.prob_tb = {1, 2, 3, 4 ,5},
	.driver = {
		.name = "jit_dev",
		.probe = my_probe,
		.remove = my_remove,
	},
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello driver module!\n");
	//在内核中注册驱动
	ret = advanced_driver_register(&my_drv);
	if (ret) {
		printk(KERN_ERR "drv_register error!\n");
		return ret;
	}
	return ret;
}
static void cleanup_hello(void)
{
	advanced_driver_unregister(&my_drv);
	printk(KERN_INFO "Bye driver module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
