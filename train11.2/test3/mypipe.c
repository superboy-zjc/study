#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/sched.h>
#include<linux/string.h>
#include<linux/uaccess.h>
#include<linux/delay.h>
#include<linux/string.h>

#define HELLO_MAJOR 252
#define CONTENT_SIZE 100
static dev_t hello_devno;
static struct cdev *hello_cdev;
static struct device *dev;
static struct class *cls;
static char content[CONTENT_SIZE]; //模拟设备文件容量为100字节
static struct semaphore sem;
static spinlock_t spin;
//0 read / 1 write
static int pipe[2] = {0};
static int rp, wp = 0;

static wait_queue_head_t my_quehead;

static int hello_open(struct inode *pnode, struct file *filp)
{
		if (!(filp->f_flags & O_NONBLOCK) && !(0xf & filp->f_flags)) {
				printk(KERN_INFO "以阻塞方式只读打开\n");
				if(pipe[1] > 0) {
						pipe[0] ++;
				} else {
						pipe[0] ++;
						wait_event_interruptible(my_quehead, pipe[1] > 0);
				}
		} else if ((filp->f_flags &  O_NONBLOCK) && !(0xf & filp->f_flags)) {
				printk(KERN_INFO "以非阻塞只读方式打开\n");
				pipe[0] ++;
		} else if (!(filp->f_flags & O_NONBLOCK) && (O_WRONLY & filp->f_flags)) {
				printk(KERN_INFO "以阻塞只写方式打开\n");
				if (pipe[0] > 0) {
						pipe[1] ++;
				} else {
						pipe[1] ++;
						wait_event_interruptible(my_quehead, pipe[0] > 0);
				}
		} else if(filp->f_flags & (O_WRONLY | O_NONBLOCK)) {	
				printk(KERN_INFO "以非阻塞只写方式打开\n");
				if (pipe[0] > 0) {
						pipe[1] ++;
				} else {
						return -ENXIO;
				}
		}
		printk(KERN_ERR "This pipe[0][1] is %d,%d \n", pipe[0], pipe[1]);
		wake_up_interruptible(&my_quehead);
		return 0;
}
static ssize_t hello_write(struct file *filp, const char *buf, size_t size, loff_t *p)
{
		int ret = 0;
		int i = 0;
		//写入字节数大于最大的容量
		if (size > CONTENT_SIZE - 1) {
				if (filp->f_flags & O_NONBLOCK) {
						//写满为止
						while ((wp + 1) % CONTENT_SIZE != rp) {
								down(&sem);
								ret = copy_from_user((void*)(content + wp), (void*) (buf + i), 1);	
								if(ret) {
										printk(KERN_ERR "3write error!\n");
										ret = -1;
										break;
								}
								spin_lock(&spin);
								wp = (wp + 1) % CONTENT_SIZE;
								spin_unlock(&spin);
								i++;
								up(&sem);
								ret = i;
						}
				} else {
						printk("right! size = %d\n", size);
						//阻塞写则一直写，直到写完
						while(i < size) {
								if((wp + 1) % CONTENT_SIZE != rp) {
										down(&sem);
										ret = copy_from_user((void*)(content + wp), (void*) (buf + i), 1);	
										if(ret) {
												printk(KERN_ERR "3write error!\n");
												ret = -1;
												break;
										}
										spin_lock(&spin);
										wp = (wp + 1) % CONTENT_SIZE;
										spin_unlock(&spin);
										i++;
										ret = size;
										up(&sem);
								} else {
										printk("满了！i = %d\n", i);
										msleep(500);
								}
						}
				}
		} else {
				//写入字节数不大于最大容量

				//目前的空间不够写
				if(size > CONTENT_SIZE - 1 - (wp - rp + CONTENT_SIZE) % CONTENT_SIZE) {
						if (filp->f_flags & O_NONBLOCK) {
								return -EAGAIN; 
						} else {
								wait_event_interruptible(my_quehead, (CONTENT_SIZE - 1 - (wp - rp + CONTENT_SIZE) % CONTENT_SIZE) >= size);
						}
				}
				//资源够写
				down(&sem);
				for (i = 0; i < size; i++) {
						ret = copy_from_user((void*)(content + wp), (void*) (buf + i), 1);	
						if(ret) {
								printk(KERN_ERR "3write error!\n");
								ret = -1;
								break;
						}
						spin_lock(&spin);
						wp = (wp + 1) % CONTENT_SIZE;
						spin_unlock(&spin);
						ret = size;
				}
				up(&sem);
		}



		//如果设备文件中有内容，唤醒等待队列
		if((wp - rp + CONTENT_SIZE) % CONTENT_SIZE > 0)
				wake_up_interruptible(&my_quehead);
		//成功返回写入字节数，错误返回-1
		return ret;
}

static ssize_t hello_read(struct file *filp, char *buf, size_t size, loff_t *p)
{
		int ret = 0;
		int i = 0;
		//判断设备文件打开方式是阻塞还是非阻塞
		if (filp->f_flags & O_NONBLOCK) {
				printk(KERN_INFO "non-block read!\n");
				//如果无内容直接返回错误码，有内容则向下执行copy操作
				if ((wp - rp + CONTENT_SIZE) % CONTENT_SIZE == 0)
						return -EAGAIN;
		} else {
				if(pipe[1] <=0 ) {
						wait_event_interruptible(my_quehead, pipe[1] > 0);
				}
				printk(KERN_INFO "block read!\n");
				//会先判断参数2：缓冲区有无内容，为真向下执行，为假则进程进入睡眠态，由等待队列维护此进程
				wait_event_interruptible(my_quehead, (wp - rp + CONTENT_SIZE) % CONTENT_SIZE > 0);
		}


		//是否发生读溢出，loff是否值合理 ; loff + 读取字节不应大于设备文件容量的数组分界
		if(size <= 0) {
				return -1;
		} else if(size > ((wp - rp + CONTENT_SIZE) % CONTENT_SIZE)) {
				printk(KERN_ERR "2SIZE TO BIG!\n");
				size = (wp - rp + CONTENT_SIZE) % CONTENT_SIZE;
		}

		for (i = 0; i < size; i++) {
				ret = copy_to_user((void*)(buf + i), (void*)(content + rp), 1);
				if (ret) {
						printk(KERN_ERR "3read error!\n");
						ret = -1;
						break;
				}		
				spin_lock(&spin);
				rp = (rp + 1) % CONTENT_SIZE;
				spin_unlock(&spin);
				ret = size;
		}
		//有内容读出就唤醒队列
		if(ret != -1) {
				wake_up_interruptible(&my_quehead);
		}
		//成功返回读出字节数，错误返回-1
		return ret;
}
static int hello_close(struct inode *pnode, struct file *filp)
{
		if (!(filp->f_flags & 0xf)) {
				printk(KERN_INFO "只读关闭\n");
				pipe[0] --;
		} else if (filp->f_flags & O_WRONLY) {
				printk(KERN_INFO "只写关闭\n");
				pipe[1] --;
		}
		return 0;
}
//注册驱动文件操作集
struct file_operations hello_ops = {
		.owner = THIS_MODULE,
		.open = hello_open,
		.read = hello_read,
		.write = hello_write,
		.release = hello_close,
};
//初始化内核模块
static int init_hello(void)
{
		int ret;
		//初始化文件数组
		memset(content, 0, CONTENT_SIZE);
		//初始化自旋锁
		spin_lock_init(&spin);
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
