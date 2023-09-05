/***************************************************************************//**
*  file       led_drv.c
*
*  details    Sample LED driver access with GPIO pin
*
*  author     AmitK
*
*  Tested with Ubuntu Linux kernel version 6.5.0-rc2+
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>              //copy_to/from_user()
#include <asm-generic/io.h>

/**
 * Define the driver name and other macro
 */
#define DRIVER_NAME "leddrv"

#define LED_OFF		0
#define LED_ON		1

#define GPIO1_BASE	0x40
#define GPIO1_SIZE	8

#define GPIO1_REG_DATA	8
#define GPIO1_REG_DIR	4

#define GPIO_BIT	(1 << 9)

/**
 * Declare and define the driver structure 
 */
static struct {
	dev_t dev_num;
	struct cdev cdev;
	unsigned int led_status;
	void __iomem *regbase;
} leddrv_data;

/**
 * Driver Functions
 */
static int leddrv_open(struct inode *inode, struct file *file);
static int leddrv_release(struct inode *inode, struct file *file);
static ssize_t leddrv_read(struct file *filp, 
	                char __user *buf, size_t len,loff_t * off);
static ssize_t leddrv_write(struct file *filp, 
		         const char *buf, size_t len, loff_t * off);
 

static void leddrv_setled(unsigned int status)
{
	u32 val;

	/* Set value */
	val = readl(leddrv_data.regbase + GPIO1_REG_DATA);
	if (LED_ON == status)
		val |= GPIO_BIT;
	else if (LED_OFF == status)
		val &= ~GPIO_BIT;
	writel(val, leddrv_data.regbase + GPIO1_REG_DATA);

	/* Update status */
	leddrv_data.led_status = status;
}

static void leddrv_setdirection(void)
{
	u32 val;

	val = readl(leddrv_data.regbase + GPIO1_REG_DATA);
	val |= GPIO_BIT;
	writel(val, leddrv_data.regbase + GPIO1_REG_DATA);
}

/**
 * File operation structure
 */
static struct file_operations leddrv_fops =
{
	.owner		= THIS_MODULE,
	.read		= leddrv_read,
	.write		= leddrv_write,
	.open		= leddrv_open,
	.release	= leddrv_release,
};

/**
 * This function called when we open the device file 
 */
static int leddrv_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/**
 * This function will be called whrn we close the device file
 */
static int leddrv_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/**
 * This function called when we read the led status 
 */
static ssize_t leddrv_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read LED status function\n");
	static const char * const msg[] = {"OFF\n", "ON\n"};
	int size;

	/**
	 * check if EOF
	 */
	if (*off > 0)
		return 0;

	size = strlen(msg[leddrv_data.led_status]);
	if (size > len)
		size = len;

	if (copy_to_user(buf, msg[leddrv_data.led_status], size))
		return -EFAULT;

	*off += size;

        return size;
}

/**
 * This function called when we write the device file
 */
static ssize_t leddrv_write(struct file *filp,
                const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
	char kbuf = 0;

	if (copy_from_user(&kbuf, buf, 1))
		return -EFAULT;

	if (kbuf == '1') {
		leddrv_setled(LED_ON);
		pr_info("LED ON!\n");
	} else if (kbuf == '0') {
		leddrv_setled(LED_OFF);
		pr_info("LED OFF!\n");
	}
		
        return len;
}

/**
 * This is driver initilization routine
 */
static int __init leddrv_init(void)
{
	int result = 0;

	if (request_mem_region(GPIO1_BASE, GPIO1_SIZE, DRIVER_NAME)) {
		pr_err("%s: Error requesting I/O\n", DRIVER_NAME);
		result = -EBUSY;
		goto ret_err_request_mem_region;
	}

	leddrv_data.regbase = ioremap_uc(GPIO1_BASE, GPIO1_SIZE);
	if (!leddrv_data.regbase) {
		pr_err("%s: Error mapping I/O\n", DRIVER_NAME);
		result = -ENOMEM;
		goto err_ioremap;
	}

	/* Allocate major number */
	if((alloc_chrdev_region(&leddrv_data.dev_num, 0, 1, DRIVER_NAME)) < 0)
	{
		pr_info("Cannot allocate Major number!!\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(leddrv_data.dev_num), MINOR(leddrv_data.dev_num));

	/* Create cdev struct */
	cdev_init(&leddrv_data.cdev, &leddrv_fops);

	/* Adding char device to system */
	if((cdev_add(&leddrv_data.cdev, leddrv_data.dev_num, 1)) < 0)
	{
		pr_info("Cannot add the device in system.\n");
	}

	leddrv_setdirection();

	leddrv_setled(LED_OFF);

	pr_info("LED: Driver Insert... Done!!!\n");
	goto ret_ok;

err_ioremap:
	release_mem_region(GPIO1_BASE, GPIO1_SIZE);
ret_err_request_mem_region:
ret_ok:	
	return result;
}

/**
 * This is module exit function
 */
static void __exit leddrv_exit(void)
{
	cdev_del(&leddrv_data.cdev);
	vunmap(leddrv_data.regbase);
	unregister_chrdev_region(leddrv_data.dev_num, 1);
	pr_info("LED: Driver Removed...Done!!!\n");
}

/**
 * Call the init exit function from macro
 */
module_init(leddrv_init);
module_exit(leddrv_exit);

/**
 * Add the module information
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AmitK <kumaramit124@gmail.com>");
MODULE_DESCRIPTION("Linux LED device driver");
MODULE_VERSION("1.0");
