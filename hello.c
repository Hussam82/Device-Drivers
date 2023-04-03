#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hussam ");
MODULE_DESCRIPTION("A hello world Pseudo device driver"); 


dev_t device_number;
struct cdev st_characterDevice;
struct class *myClass;
struct device *myDevice;
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
    // 1-allocate device number
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
    // 2- define driver character or block or network
    cdev_init(&st_characterDevice, &fops);
    retval = cdev_add(&st_characterDevice, device_number, 1);
    if(retval != 0)
    {
        printk("Registering the device to the kernel failed!\n");
        /* In case of failure, you have to delete everything made before */
        goto CHARACTER_ERROR;

    }
    // 3- generate file (class - device)
    if( (myClass = class_create(THIS_MODULE, "test_class")) == NULL )
    {
        printk("Device class can not be created!\n");
        /* In case of failure, you have to delete everything made before */
        goto CLASS_ERROR;
    }
    myDevice = device_create(myClass, NULL, device_number, NULL, "test_file");
    if(myDevice == NULL)
    {
        printk("Device can not be created!\n");
        /* In case of failure, you have to delete everything made before */
        goto DEVICE_ERROR;

    }
    printk("Device Driver is created successfully !\n");
    return 0;
    
DEVICE_ERROR:
    class_destroy(myClass);
CLASS_ERROR:
    cdev_del(&st_characterDevice);
CHARACTER_ERROR:
    unregister_chrdev_region(device_number, 1);
    return -1;
}



static void __exit hellodriver_exit(void)
{
    cdev_del(&st_characterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    unregister_chrdev_region(device_number, 1);
    printk("Good bye Kernel\n");
}

module_init(hellodriver_init);
module_exit(hellodriver_exit);