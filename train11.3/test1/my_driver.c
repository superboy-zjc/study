#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>

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
//初始化驱动对象，声明名字、总线对象、探测函数方法与卸载方法
static struct device_driver my_drv = {
	.name = "jit_dev",
	.bus = &my_bus,
	.probe = my_probe,
	.remove = my_remove,
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello driver module!\n");
	//在内核中注册驱动
	ret = driver_register(&my_drv);
	if (ret) {
		printk(KERN_ERR "drv_register error!\n");
		return ret;
	}
	return ret;
}
static void cleanup_hello(void)
{
	driver_unregister(&my_drv);
	printk(KERN_INFO "Bye driver module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
