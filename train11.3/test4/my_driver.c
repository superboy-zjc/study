#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/platform_device.h>
#include<linux/cdev.h>

#define HELLO_MAJOR 252
static dev_t hello_devno;
static struct cdev *hello_cdev;
static struct class *cls;
static struct device *my_dev;

static int my_open(struct inode *pnode, struct file *filp)
{
	printk(KERN_INFO "In %s \n", __func__);
	return 0;
}
//字符设备的文件操作集
static struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = my_open,
};
//总线匹配成功驱动与设备后，会启动驱动的初始化函数,probe(也称探测函数)
static int my_probe(struct platform_device *dev)
{
	printk(KERN_INFO "In %s \n", __func__);
	//打印资源
	printk(KERN_INFO "Resource name is %s, start is 0x%x, end is 0x%x\n", dev->resource[0].name,
			dev->resource[0].start, dev->resource[0].end);
	int ret;
	//利用宏将主次设备号转化为32位的设备号，供注册函数使用
	hello_devno = MKDEV(HELLO_MAJOR, 0);
	//注册字符设备号
	ret = register_chrdev_region(hello_devno, 1, "zjcchar");
	if (ret) {
		//注册失败则由系统分配一个设备号
		ret = alloc_chrdev_region(&hello_devno, 0, 1, "zjcchar");
		if (ret) {
			printk(KERN_ERR "allocchr error!\n");
			goto error0;
		}
	}
	//动态分配空间给cdev字符设备对象
	hello_cdev = cdev_alloc();
	if (hello_cdev == NULL) {
		printk(KERN_ERR "alloc error!\n");
		ret = -1;
		goto error1;	
	}
	//初始化cdev字符设备对象并将操作集赋给它
	cdev_init(hello_cdev, &hello_ops);
	//将cdev字符设备对象注册到内核中
	ret = cdev_add(hello_cdev, hello_devno, 1);
	if (ret) {
		printk(KERN_ERR "add error!\n");
		goto error2;
	}
	//创建class供创建设备节点时使用
	cls = class_create(THIS_MODULE, "testclass");
	if (IS_ERR(cls)) {
		ret = PTR_ERR(dev);
		printk(KERN_ERR "class_create error!\n");
		goto error2;
	}
	//创建设备节点在/dev/下,名字为testdev
	my_dev = device_create(cls, NULL, hello_cdev->dev, NULL, "testdev");
	if (IS_ERR(my_dev)) {
		ret = PTR_ERR(my_dev);
		printk(KERN_ERR "device_create error!\n");
		goto error3;
	}
	printk(KERN_ERR "Hello, World!\n");
	return 0;
error3:
	class_destroy(cls);
error2:
	cdev_del(hello_cdev);
error1:	
	unregister_chrdev_region(hello_devno, 1);
error0:
	return ret;
	return 0;
}
//删除字符设备
static int my_remove(struct platform_device *dev)
{
	device_destroy(cls, hello_devno);
	class_destroy(cls);
	cdev_del(hello_cdev);
	unregister_chrdev_region(hello_devno, 1);
	printk(KERN_INFO "In %s \n", __func__);
	return 0;
}
//初始化驱动对象，声明名字、总线对象、探测函数方法与卸载方法
static struct platform_driver my_drv = {
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.name = "zjc_dev1",
	}
};
static int init_hello(void)
{
	int ret = 0;
	printk(KERN_INFO "hello driver module!\n");
	//在内核中注册驱动
	ret = platform_driver_register(&my_drv);
	if (ret) {
		printk(KERN_ERR "drv_register error!\n");
		return ret;
	}
	return ret;
}
static void cleanup_hello(void)
{
	platform_driver_unregister(&my_drv);
	printk(KERN_INFO "Bye driver module!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
