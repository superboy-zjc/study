#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>

#define HELLO_MAJOR 252
dev_t hello_devno;
struct cdev *hello_cdev;

int hello_open (struct inode *pnode, struct file *filp)
{
	printk(KERN_INFO "hello_open! \n");
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
			printk(KERN_ERR "error!\n");
			goto error;
		}
	}
	hello_cdev = cdev_alloc();
	if (hello_cdev == NULL) {
		printk(KERN_INFO "alloc error!\n");
		ret = -1;
		goto error;	
	}
	cdev_init(hello_cdev, &hello_ops);
	ret = cdev_add(hello_cdev, hello_devno, 1);
	if (ret) {
		printk(KERN_ERR "add error!\n");
		goto error;
	}
	printk(KERN_INFO "Hello, World!\n");
	return 0;
error:	
	unregister_chrdev_region(hello_devno, 1);
	cdev_del(hello_cdev);
	return ret;
}
static void cleanup_hello(void)
{
	unregister_chrdev_region(hello_devno, 1);
	cdev_del(hello_cdev);
	printk(KERN_INFO "Goodbye, world!\n");
}
module_init(init_hello);
module_exit(cleanup_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc");
MODULE_DESCRIPTION("A test project");
