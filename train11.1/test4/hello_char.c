#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/mm.h>

#define HELLO_MAJOR 252
#define CONTENT_SIZE 1000

static dev_t hello_devno;
static struct cdev *hello_cdev;
static struct device *dev;
static struct class *cls;
static char content[CONTENT_SIZE] ; //模拟设备文件容量为100字节
static int hello_open(struct inode *pnode, struct file *filp)
{
		printk(KERN_ERR "This inode's Id is %lu \n", pnode->i_ino);
		return 0;
}
static int hello_mmap(struct file *file, struct vm_area_struct *vma)  
{  
     	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

        unsigned long pfn_start = (virt_to_phys(content) >> PAGE_SHIFT) + vma->vm_pgoff;

        unsigned long virt_start = (unsigned long)content + offset;

        unsigned long size = vma->vm_end - vma->vm_start;
	
	int ret = 0;
 
     	printk("phy: 0x%lx, offset: 0x%lx, size: 0x%lx\n", pfn_start << PAGE_SHIFT, offset, size);
 
     	ret = remap_pfn_range(vma, vma->vm_start, pfn_start, size, vma->vm_page_prot);
     	if (ret)
	{
		printk("%s: remap_pfn_range failed at [0x%lx  0x%lx]\n",__func__, vma->vm_start, vma->vm_end);
	}else{

         	printk("%s: map 0x%lx to 0x%lx, size: 0x%lx\n", __func__, virt_start,vma->vm_start, size);
	}
 
     	return ret;
}  
struct file_operations hello_ops = {
		.owner = THIS_MODULE,
		.open = hello_open,
		.mmap = hello_mmap,
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
