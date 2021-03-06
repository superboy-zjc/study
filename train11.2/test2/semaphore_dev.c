#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/sched.h>
#include<linux/string.h>
#include<linux/uaccess.h>
#include<linux/delay.h>

#define HELLO_MAJOR 252
#define CONTENT_SIZE 1000
static dev_t hello_devno;
static struct cdev *hello_cdev;
static struct device *dev;
static struct class *cls;
static char content[CONTENT_SIZE]; //模拟设备文件容量为100字节
static struct semaphore sem;

static wait_queue_head_t my_quehead;

static int hello_open(struct inode *pnode, struct file *filp)
{
	printk(KERN_ERR "This inode's Id is %lu \n", pnode->i_ino);
	return 0;
}
static ssize_t hello_write(struct file *filp, const char *buf, size_t size, loff_t *p)
{
	int ret;
	//是否发生写溢出，loff是否值合理 ; loff + 写入字节不应大于设备文件容量的数组分界
	if(*p < 0) {
		return -1;
		printk(KERN_ERR "1write error!\n");
	} else if(*p + size > CONTENT_SIZE ) {
		printk(KERN_ERR "2write error!\n");
		return -1;
	}
	printk(KERN_INFO "pid%d 抢信号量..\n", current->pid);
	down(&sem);
	printk(KERN_INFO "pid%d 抢信号量\n", current->pid);
	//睡眠5秒保证可观测到预期结果
	msleep(5000);
	//此函数可以交换内核与userspace的数据
	ret = copy_from_user((void*)(content + *p), (void*) buf, size);	
	printk(KERN_INFO "pid%d 释放信号量\n", current->pid);
	up(&sem);
	if(ret) {
		printk(KERN_ERR "3write error!\n");
		return ret;
	}
	//移动文件指针
	*p += size;
	//如果设备文件中有内容，唤醒等待队列
	if(strlen(content) > 0)
		wake_up_interruptible(&my_quehead);
	//成功返回写入字节数，错误返回-1
	return ret;
}
static ssize_t hello_read(struct file *filp, char *buf, size_t size, loff_t *p)
{
	int ret;
	//判断设备文件打开方式是阻塞还是非阻塞
	if (filp->f_flags & O_NONBLOCK) {
		printk(KERN_INFO "non-block read!\n");
		//如果无内容直接返回错误码，有内容则向下执行copy操作
		if (strlen(content) == 0)
			return -EAGAIN;
	} else {
		printk(KERN_INFO "block read!\n");
		//会先判断参数2：condition是否为真，为真向下执行，为假则进程进入睡眠态，由等待队列维护此进程
		wait_event_interruptible(my_quehead, strlen(content) > 0);
	}


	//是否发生读溢出，loff是否值合理 ; loff + 读取字节不应大于设备文件容量的数组分界
	if (*p < 0) {
		printk(KERN_ERR "1read error!\n");
		return -1;
	} else if(*p + size > CONTENT_SIZE ) {
		printk(KERN_ERR "2read error!\n");
		return -1;
	}
	ret = copy_to_user((void*)buf, (void*)(content + *p), size);
	if (ret) {
		printk(KERN_ERR "3read error!\n");
		return ret;
	}
	//成功返回读出字节数，错误返回-1
	return size;
}
//注册驱动文件操作集
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = hello_open,
	.read = hello_read,
	.write = hello_write,
};
//初始化内核模块
static int init_hello(void)
{
	int ret;
	//初始化等待队列
	init_waitqueue_head(&my_quehead);
	//初始化信号量，并且设置初始值为1（资源可用）
	sema_init(&sem, 1);
	//宏转换设备号为系统可识别的32位二进制
	hello_devno = MKDEV(HELLO_MAJOR, 0);

	//自定义向内核申请驱动设备号
	ret = register_chrdev_region(hello_devno, 1, "zjcchar");
	if (ret) {
		//如申请失败，则动态由内核分配
		ret = alloc_chrdev_region(&hello_devno, 0, 1, "zjcchar");
		if (ret) {
			printk(KERN_ERR "allocchr error!\n");
			goto error0;
		}
	}
	//动态创建一个驱动设备结构体
	hello_cdev = cdev_alloc();
	if (hello_cdev == NULL) {
		printk(KERN_ERR "alloc error!\n");
		ret = -1;
		goto error1;	
	}
	//初始化驱动设备结构体，与驱动文件操作集捆绑
	cdev_init(hello_cdev, &hello_ops);
	//驱动设备结构体与设备号关联，并交由内核相关链表中维护
	ret = cdev_add(hello_cdev, hello_devno, 1);
	if (ret) {
		printk(KERN_ERR "add error!\n");
		goto error2;
	}
	//创建一个class类与当前模块绑定,供创建设备节点的API使用
	cls = class_create(THIS_MODULE, "testclass");
	if (IS_ERR(cls)) {
		ret = PTR_ERR(dev);
		printk(KERN_ERR "class_create error!\n");
		goto error2;
	}
	//创建一个设备节点（文件）,指明设备节点与hello_cdev中的设备号相关联
	//并取名为testdev(会创建在/dev/testdev)
	dev = device_create(cls, NULL, hello_cdev->dev, NULL, "testdev");
	if (IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		printk(KERN_ERR "device_create error!\n");
		goto error3;
	}
	printk(KERN_ERR "Hello, World!\n");
	return 0;
//一个简洁高效的错误处理机制
error3:
	class_destroy(cls);
error2:
	cdev_del(hello_cdev);
error1:	
	unregister_chrdev_region(hello_devno, 1);
error0:
	return ret;
}
//清理并卸载内核模块,注意清理顺序是倒序的，因为各结构体之间有依赖关系
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
