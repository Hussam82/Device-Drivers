#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hussam ");
MODULE_DESCRIPTION("A hello world Pseudo device driver"); 


/* Trial 1*/
/* int cnt = 0;
//module_param(cnt, int, 0664);
module_param(cnt, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(cnt, "An Integer Parameter"); */

/* Trial 2*/
int number = 0;
module_param(number, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(number, "Major Number");
int major_number = 0;
dev_t device_number;
static int driver_open(struct inode *device_file, struct file *instance)
{
    printk("%s mytest_driver - open was called", __FUNCTION__);
    return 0;
}
static int driver_close(struct inode *device_file, struct file *instance)
{
    printk("%s mytest_driver - close was called", __FUNCTION__);
    return 0;   
}

struct file_operations fops = 
{
    .owner=THIS_MODULE,
    .open=driver_open,
    .release=driver_close
};

/* Trial 2*/
static int __init hellodriver_init(void)
{
    int retval;
    major_number = number;
    printk("Hello Kernel\n");
    retval = register_chrdev(major_number, "mytest_driver", &fops);
    if(retval == 0)
    {
        /* Major number allocation is success */
        printk("%s retval=0 - registered device Major number: %d, Minor Number: %d\n", __FUNCTION__,major_number, 0);
    }
    else
    {
        printk("Could not register device number!\n");
        return -1;
    }
    
    return 0;
}

static void __exit hellodriver_exit(void)
{
    unregister_chrdev(major_number, "mytest_driver");
    printk("Good bye Kernel\n");
}

/* Trial 1*/
/* static int __init hellodriver_init(void)
{
    int i = 0;
    for(i = 0; i < cnt; i++)
    {
        printk("Hello World");
    }
    return 0;
}
static void __exit hellodriver_exit(void)
{
    int i = 0;
    for(i = 0; i < cnt; i++)
    {
        printk("GOODBYE World");
    }
} */
module_init(hellodriver_init);
module_exit(hellodriver_exit);