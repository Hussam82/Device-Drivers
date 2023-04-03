#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hussam ");
MODULE_DESCRIPTION("A hello world Pseudo device driver"); 



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


static int __init hellodriver_init(void)
{
    int retval;
    printk("Hello Kernel\n");
    retval = alloc_chrdev_region(&device_number, 0, 1, "test_devicenumber");
    if(retval == 0)
    {
        printk("%s retval=0 - registered device Major number: %d, Minor Number: %d\n", __FUNCTION__,MAJOR(device_number), MINOR(device_number));

    }
    else
    {
        printk("Could not register device number");
        return -1;
    }
    return 0;
}

static void __exit hellodriver_exit(void)
{
    unregister_chrdev_region(device_number, 1);
    printk("Good bye Kernel\n");
}

module_init(hellodriver_init);
module_exit(hellodriver_exit);