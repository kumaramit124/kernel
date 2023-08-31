/***************************************************************************//**
*  file       mod_sys.c
*
*  details    Driver with (sysfs) entry explore
*
*  author     AmitK
*
*  Tested with Ubuntu Linux kernel version 6.5.0-rc2+
*
*******************************************************************************/
#include <linux/kobject.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/sysfs.h> 
#include <linux/err.h>
 
volatile int mod_value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev mod_cdev;
struct kobject *kobj_ref;

/**
 * Function prototype
 */
static int __init mod_driver_init(void);
static void __exit mod_driver_exit(void);

/**
 * Driver Functions
 */
static int mod_open(struct inode *inode, struct file *file);
static int mod_release(struct inode *inode, struct file *file);
static ssize_t mod_read(struct file *filp, 
	                char __user *buf, size_t len,loff_t * off);
static ssize_t mod_write(struct file *filp, 
		         const char *buf, size_t len, loff_t * off);
 
/*************** Sysfs functions **********************/
static ssize_t sysfs_show(struct kobject *kobj, 
		          struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, 
		           struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute mod_attr = __ATTR(mod_value, 0660, sysfs_show, sysfs_store);

/**
 * File operation structure
 */
static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= mod_read,
	.write		= mod_write,
	.open		= mod_open,
	.release	= mod_release,
};

/**
 * This function called when we read sysfs file
 */
static ssize_t sysfs_show(struct kobject *kobj,
                struct kobj_attribute *attr, char *buf)
{
	pr_info("Sysfs - Read!!!\n");
	return sprintf(buf, "%d", mod_value);
}

/**
 * This function called when we write sysfs file
 */
static ssize_t sysfs_store(struct kobject *kobj,
                struct kobj_attribute *attr,const char *buf, size_t count)
{
	pr_info("Sysfs - Write!!!\n");
	sscanf(buf, "%d", &mod_value);
	return count;
}

/**
 * This function called when we open the device file 
 */
static int mod_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/**
 * This function will be called whrn we close the device file
 */
static int mod_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/**
 * This function called when we read the device file
 */
static ssize_t mod_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read function\n");
        return 0;
}

/**
 * This function called when we write the device file
 */
static ssize_t mod_write(struct file *filp,
                const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
        return len;
}

/**
 * This is driver initilization routine
 */
static int __init mod_driver_init(void)
{
	/* Allocate major number */
	if((alloc_chrdev_region(&dev, 0, 1, "mod_dev")) < 0)
	{
		pr_info("Cannot allocate Major number!!\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/* Create cdev struct */
	cdev_init(&mod_cdev, &fops);

	/* Adding char device to system */
	if((cdev_add(&mod_cdev, dev, 1)) < 0)
	{
		pr_info("Cannot add the device in system.\n");
		goto r_class;
	}

	/* Create struct class */
	if(IS_ERR(dev_class = class_create("mod_class")))
	{
		pr_info("Cannot create struct class.\n");
		goto r_class;
	}

	/* Create device */
	if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "mod_device")))
	{
		pr_info("Cannot create device.\n");
		goto r_device;
	}

	/* Creating sys entry under the /sys/kernel/ directory */
	kobj_ref = kobject_create_and_add("mod_sysfs", kernel_kobj);

	/* Creating sysfs file for mod_value */
	if(sysfs_create_file(kobj_ref, &mod_attr.attr))
	{
		pr_err("Cannot create sysfs file.\n");
		goto r_sysfs;
	}

	pr_info("Driver with sysfs entry Insert... Done!!!\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &mod_attr.attr);

r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev, 1);
	cdev_del(&mod_cdev);
	return -1;
}

/**
 * This is module exit function
 */
static void __exit mod_driver_exit(void)
{
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &mod_attr.attr);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&mod_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device Driver Remove...Done!!!\n");
}

/**
 * Call the init exit function from macro
 */
module_init(mod_driver_init);
module_exit(mod_driver_exit);

/**
 * Add the module information
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AmitK <kumaramit124@gmail.com>");
MODULE_DESCRIPTION("Linux device driver with (sysfs) entry");
MODULE_VERSION("1.0");
