#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/w1.h>
#include <linux/w1-gpio.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hussam ");
MODULE_DESCRIPTION("Device Driver for DS18B20 Digital Temperature Sensor");

#define DEVICE_NAME "ds18b20"
#define SUCCESS 0
#define SIZE 15


dev_t device_number;
struct cdev st_characterDevice;
struct class *myClass;
struct device *myDevice;
static struct w1_bus_master *w1_master;
static struct w1_slave *w1_slave;
static unsigned char buffer[SIZE] = "";

static int ds18b20_open(struct inode *device_file, struct file *instance)
{
    printk("%s ds18b20 - open was called", __FUNCTION__);
    return SUCCESS;
}

static int ds18b20_close(struct inode *device_file, struct file *instance)
{
    printk("%s ds18b20 - close was called", __FUNCTION__);
    return SUCCESS;
}

ssize_t ds18b20_read(struct file *file, char __user *userBuffer, size_t count, loff_t *offset)
{
    int not_copied;
    float temperature;
    char scratchpad[9];
    char message[32];
    int raw_value;
    w1_reset_select_slave(w1_master, w1_slave);
    w1_write_byte(w1_master, 0x44); // Start temperature conversion
    msleep(1000); // Wait for conversion to complete

    w1_reset_select_slave(w1_master, w1_slave);
    w1_write_byte(w1_master, 0xBE); // Read scratchpad

    for (int i = 0; i < 9; i++)
    {
        scratchpad[i] = w1_read_byte(w1_master);
    }
    raw_value = (scratchpad[1] << 8) | scratchpad[0];
    if (scratchpad[1] & 0x80)
    {
        raw_value = (~raw_value) + 1;
    }
    temperature = raw_value / 16.0;
    sprintf(message, "Temperature: %.2f Celsius\n", temperature); // copy temperature into a string
    /* we need to check if the user wants more that the size or not */
    /* This if condition guarantees that the count never exceeds the size of the buffer */
    if (count + (*offset) > SIZE)
    {
        count = SIZE - (*offset);
    }
    /* Returns number of not copied bytes */
    not_copied = copy_to_user(userBuffer, &message[*offset], count);
    if (not_copied)
    {
        return -1;
    }
    *offset = count; // update offset to ensure that it starts reading from the last known location
    printk("%s: number of not copied bytes: %d\n", __func__, not_copied);
    printk("%s: message: %s\n", __func__, userBuffer);
    return count; // return number of successfully read bytes
}



struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .open = ds18b20_open,
        .release = ds18b20_close,
        .read = ds18b20_read};

static int __init ds18b20_init(void)
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

    if (gpio_request(LED_PIN, "rpi_gpio_2"))
    {
        printk("Cannot allocate GPIO2\n");
        goto GPIO_REQUEST_ERROR;
    }

    if (gpio_direction_output(LED_PIN, 0))
    {
        printk("Cannot set the pin direction to be output\n");
        goto GPIO_DIR_ERROR;
    }
    return 0;

GPIO_DIR_ERROR:
    gpio_free(LED_PIN);
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

static void __exit ds18b20_exit(void)
{
    gpio_set_value(LED_PIN, 0);
    gpio_free(LED_PIN);
    cdev_del(&st_characterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    unregister_chrdev_region(device_number, 1);
    printk("Good bye Kernel\n");
}

module_init(ds18b20_init);
module_exit(ds18b20_exit);