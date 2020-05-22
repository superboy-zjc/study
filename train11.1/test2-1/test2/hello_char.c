#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>

#define HELLO_MAJOR 252
static dev_t hello_devno;
static struct cdev *hello_cdev;
static struct device *dev;
static struct class *cls;

static int hello_open (struct inode *pnode, struct file *filp)
{
		printk(KERN_ERR "hello_open! \n");
		return 0;
}

struct file_operations hello_ops = {
		.owner = THIS_MODULE,
		.open = hello_open,
};

static int init_hello(void)
{
		int ret;
		hello_devno = MKDEV(HELLO_MAJOR, 0);

		ret = register_chrdev_region(hello_devno, 1, "zjcchar");
		if (ret) {
				ret = alloc_chrdev_region(&hello_devno, 0, 1, "zjcchar");
				if (ret) {
						printk(KERN_ERR "allocchr error!\n");
						goto error0;
				}
		}

		hello_cdev = cdev_alloc();
		if (hello_cdev == NULL) {
				printk(KERN_ERR "alloc error!\n");
				ret = -1;
				goto error1;	
		}
		cdev_init(hello_cdev, &hello_ops);
		ret = cdev_add(hello_cdev, hello_devno, 1);
		if (ret) {
				printk(KERN_ERR "add error!\n");
				goto error2;
		}
		cls = class_create(THIS_MODULE, "testclass");
		if (IS_ERR(cls)) {
				ret = PTR_ERR(dev);
				printk(KERN_ERR "class_create error!\n");
				goto error2;
		}
			dev = device_create(cls, NULL, hello_cdev->dev, NULL, "testdev");
				if (IS_ERR(dev)) {
				ret = PTR_ERR(dev);
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
}
static void cleanup_hello(void)
{
		device_destroy(cls, hello_devno);
		class_destroy(cls);
		cdev_del(hello_cdev);
		unregister_chrdev_region(hello_devno, 1);
		printk(KERN_ERR "Goodbye, world!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
