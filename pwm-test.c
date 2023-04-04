#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hussam ");
MODULE_DESCRIPTION("A hello world Pseudo device driver");

#define SIZE 15
#define SIZE_READ 3
#define FREQ 1000000000

dev_t device_number;
struct cdev st_characterDevice;
struct class *myClass;
struct device *myDevice;
static unsigned char buffer[SIZE] = "";

/* >>>>>>>>>> PWM <<<<<<<<<<<<<<<<< */
struct pwm_device *pwm0 = NULL;
uint32_t pwm_high = 500000000; // 500MHz for a frequency 1GHz 50% duty cycle

// echo "hello world from Egypt" > /dev/test_file --> writes hello in buffer
ssize_t driver_write(struct file *file, const char __user *userBuffer, size_t count, loff_t *offset)
{
    int not_copied;
    long value = 0;
    printk("%s: the count to write %ld\n", __func__, count);
    printk("%s: the offset %lld\n", __func__, *offset);
    /* we need to check if the user wants more that the size or not */
    /* This if condition guarantees that the count never exceeds the size of the buffer */
    if (count + (*offset) > SIZE)
    {
        count = SIZE - (*offset);
    }
    if (!count) // no more memory space left to guarantee that user won't write in a space outside the buffer space
    {
        printk("No space left");
        return -1;
    }
    /* Returns number of not copied bytes */
    not_copied = copy_from_user(&buffer[*offset], userBuffer, count);
    if (not_copied)
    {
        return -1;
    }
    // convert string that user has written into the device file to integer to be put as the pwm_high
    // User enters the value in ms
    if (kstrtol_from_user(userBuffer, count - 1, 0, &value) == 0) // base 0 --> automatically converts number to decimal
    {
        printk("%s the value is %ld\n", __func__, value);
    }
    if (value > 1000) // make sure that the value is not greater than the frequency
    {
        printk("Invalid Value\n");
    }
    else
    {
        pwm_config(pwm0, value * 1000000, FREQ);
    }

    *offset = count; // update offset to ensure that it starts reading from the last known location
    printk("%s: number of written bytes: %ld\n", __func__, count);
    printk("%s: message: %s\n", __func__, buffer);
    return count; // return number of successfully read bytes
}

ssize_t driver_read(struct file *file, char __user *userBuffer, size_t count, loff_t *offset)
{
    int not_copied;
    char tmp[SIZE_READ] = "";
    // printk("Driver read is called right now\n");
    printk("%s: the count to read %ld\n", __func__, count);
    printk("%s: the offset %lld\n", __func__, *offset);
    /* we need to check if the user wants more that the size or not */
    /* This if condition guarantees that the count never exceeds the size of the buffer */
    if (count + (*offset) > SIZE_READ)
    {
        count = SIZE_READ - (*offset);
    }
    /* Returns number of not copied bytes */
    not_copied = copy_to_user(userBuffer, &tmp[*offset], count);
    if (not_copied)
    {
        return -1;
    }
    *offset = count; // update offset to ensure that it starts reading from the last known location
    printk("%s: number of not copied bytes: %d\n", __func__, not_copied);
    printk("%s: message: %s\n", __func__, userBuffer);
    return count; // return number of successfully read bytes
}

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
        .owner = THIS_MODULE,
        .open = driver_open,
        .release = driver_close,
        .read = driver_read,
        .write = driver_write};

static int __init hellodriver_init(void)
{
    int retval;
    printk("Hello Kernel\n");
    // 1-allocate device number
    retval = alloc_chrdev_region(&device_number, 0, 1, "test_devicenumber");
    if (retval == 0)
    {
        printk("%s retval=0 - registered device Major number: %d, Minor Number: %d\n", __FUNCTION__, MAJOR(device_number), MINOR(device_number));
    }
    else
    {
        printk("Could not register device number");
        return -1;
    }
    // 2- define driver character or block or network
    cdev_init(&st_characterDevice, &fops);
    retval = cdev_add(&st_characterDevice, device_number, 1);
    if (retval != 0)
    {
        printk("Registering the device to the kernel failed!\n");
        /* In case of failure, you have to delete everything made before */
        goto CHARACTER_ERROR;
    }
    // 3- generate file (class - device)
    if ((myClass = class_create(THIS_MODULE, "test_class")) == NULL)
    {
        printk("Device class can not be created!\n");
        /* In case of failure, you have to delete everything made before */
        goto CLASS_ERROR;
    }
    myDevice = device_create(myClass, NULL, device_number, NULL, "test_file");
    if (myDevice == NULL)
    {
        printk("Device can not be created!\n");
        /* In case of failure, you have to delete everything made before */
        goto DEVICE_ERROR;
    }
    printk("Device Driver is created successfully !\n");
    pwm0 = pwm_request(0, "mypwm");
    if (pwm0 == NULL)
    {
        printk("PWM0 can not be created\n");
        goto GPIO_REQUEST_ERROR;
    }
    pwm_config(pwm0, pwm_high, FREQ); // pwm0 configured with duty cycle 50% with period of 1/1GHz (ns)
    pwm_enable(pwm0);
    return 0;

GPIO_REQUEST_ERROR:
    device_destroy(myClass, device_number);
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
    pwm_disable(pwm0);
    pwm_free(pwm0);
    cdev_del(&st_characterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    unregister_chrdev_region(device_number, 1);
    printk("Good bye Kernel\n");
}

module_init(hellodriver_init);
module_exit(hellodriver_exit);