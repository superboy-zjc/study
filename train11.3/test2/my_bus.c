#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>

#include"advanced_device.h"

//注册挂载到总线上的驱动与设备对象之间的匹配方法
int my_match(struct device *dev, struct device_driver *drv)
{
	struct advanced_device *adev;
	struct advanced_driver *adrv;
	int i = 0;
	printk("In %s \n", __func__);
	//获得自定义的设备与驱动对象
	adev = container_of(dev, struct advanced_device, dev);
	adrv = container_of(drv, struct advanced_driver, driver);
	//在驱动中查表,id存在则匹配
	for (i = 0; i < PROBTBSIZE; i ++) {
		if(adev->id == adrv->prob_tb[i])
			return 1;
	}
	return 0;
}
//初始化总线对象
static struct bus_type my_bus = {
	.name = "my_bus",
	.match = my_match,
};
EXPORT_SYMBOL_GPL(my_bus);
//自定义advanced_device设备的注册方法，注册时指明了总线
int advanced_device_register(struct advanced_device *adev)
{
	adev->dev.bus = &my_bus;
	device_initialize(&adev->dev);
	return device_add(&adev->dev);
}
EXPORT_SYMBOL_GPL(advanced_device_register);

//自定义advanced_device设备的销毁方法
void advanced_device_unregister(struct advanced_device *adev)
{
	device_del(&adev->dev);
	put_device(&adev->dev);
}
EXPORT_SYMBOL_GPL(advanced_device_unregister);

//自定义advanced_driver驱动的注册方法，注册时指明了总线
int advanced_driver_register(struct advanced_driver *adrv)
{
	adrv->driver.bus = &my_bus;
	return driver_register(&adrv->driver);
}
EXPORT_SYMBOL_GPL(advanced_driver_register);

//自定义advanced_driver驱动的销毁方法
void advanced_driver_unregister(struct advanced_driver *adrv)
{
	driver_unregister(&adrv->driver);
}
EXPORT_SYMBOL_GPL(advanced_driver_unregister);

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
