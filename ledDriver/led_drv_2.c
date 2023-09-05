/***************************************************************************//**
*  file       led_drv.c
*
*  details    Sample LED driver access with driver modle led framework
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
#include <linux/ioport.h>
#include <linux/leds.h>              //copy_to/from_user()
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
struct leddrv_data_st {
	void __iomem *regbase;
	struct led_classdev led_cdev;
};

static struct leddrv_data_st *leddrv_data;

/**
 * Driver Functions
 */
static void leddrv_setled(unsigned int status)
{
	u32 val;

	/* Set value */
	val = readl(leddrv_data->regbase + GPIO1_REG_DATA);
	if (LED_ON == status)
		val |= GPIO_BIT;
	else if (LED_OFF == status)
		val &= ~GPIO_BIT;
	writel(val, leddrv_data->regbase + GPIO1_REG_DATA);
}

static void leddrv_setdirection(void)
{
	u32 val;

	val = readl(leddrv_data->regbase + GPIO1_REG_DATA);
	val |= GPIO_BIT;
	writel(val, leddrv_data->regbase + GPIO1_REG_DATA);
}

static void leddrv_change_state(struct led_classdev *led_cdev,
	       	enum led_brightness brightness)
{
	if (brightness)
		leddrv_setled(LED_ON);
	else
		leddrv_setled(LED_OFF);
}

/**
 * This is driver initilization routine
 */
static int __init leddrv_init(void)
{
	int result = 0;

	leddrv_data = kzalloc(sizeof(*leddrv_data), GFP_KERNEL);
	if (!leddrv_data) {
		result = -ENOMEM;
		goto ret_err_kzalloc;
	}

	if (request_mem_region(GPIO1_BASE, GPIO1_SIZE, DRIVER_NAME)) {
		pr_err("%s: Error requesting I/O\n", DRIVER_NAME);
		result = -EBUSY;
		goto ret_err_request_mem_region;
	}

	leddrv_data->regbase = ioremap_uc(GPIO1_BASE, GPIO1_SIZE);
	if (!leddrv_data->regbase) {
		pr_err("%s: Error mapping I/O\n", DRIVER_NAME);
		result = -ENOMEM;
		goto err_ioremap;
	}

	leddrv_data->led_cdev.name = "ipe:red:user";
	leddrv_data->led_cdev.brightness_set = leddrv_change_state;

	result = led_classdev_register(NULL, &leddrv_data->led_cdev);
	if (result) {
		pr_err("%s: Error registering led\n", DRIVER_NAME);
		goto ret_err_led_classdev_register;
	}


	leddrv_setdirection();

	leddrv_setled(LED_OFF);

	pr_info("LED: Driver Insert... Done!!!\n");
	goto ret_ok;

ret_err_led_classdev_register:
	vunmap(leddrv_data->regbase);
err_ioremap:
	release_mem_region(GPIO1_BASE, GPIO1_SIZE);
ret_err_request_mem_region:
	kfree(leddrv_data);
ret_err_kzalloc:	
ret_ok:	
	return result;
}

/**
 * This is module exit function
 */
static void __exit leddrv_exit(void)
{
	led_classdev_unregister(&leddrv_data->led_cdev);
	vunmap(leddrv_data->regbase);
	release_mem_region(GPIO1_BASE, GPIO1_SIZE);
	kfree(leddrv_data);
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
