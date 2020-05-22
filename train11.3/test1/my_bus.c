#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>
//注册挂载到总线上的驱动与设备对象之间的匹配方法
int my_match(struct device *dev, struct device_driver *drv)
{
	printk("In %s \n", __func__);
	//按照名字相同的方式匹配
	return (strcmp(dev_name(dev), drv->name) == 0);
}
//初始化总线对象
static struct bus_type my_bus = {
	.name = "my_bus",
	.match = my_match,
};
//将总线对象导出供内核其他模块中的设备对象和驱动对象引用
EXPORT_SYMBOL(my_bus);
static int init_my_bus(void)
{
	int ret = 0;
	printk(KERN_INFO "Hello bus module!\n");
	//在内核中注册总线
	ret = bus_register(&my_bus);
	if (ret) {
		printk(KERN_ERR "bus_register error!\n");
		return ret;
	}
	return ret;
}
//清理并卸载内核模块,注意清理顺序是倒序的，因为各结构体之间有依赖关系
static void cleanup_my_bus(void)
{
	bus_unregister(&my_bus);
	printk(KERN_INFO "Bye module!\n");
}
module_init(init_my_bus);
module_exit(cleanup_my_bus);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
