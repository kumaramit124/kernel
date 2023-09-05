/***************************************************************************//**
*  file       led_drv.c
*
*  details    Sample LED driver using gpiolib API
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
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>

/**
 * Define the driver name and other macro
 */
#define DRIVER_NAME "leddrv"

#define LED_OFF		0
#define LED_ON		1

#define GPIO_NUM	9

/**
 * Declare and define the driver structure 
 */
struct leddrv_data_st {
	struct gpio_desc *desc;
	struct led_classdev led_cdev;
};

static struct leddrv_data_st *leddrv_data;

/**
 * Driver Functions
 */
static void leddrv_setled(unsigned int status)
{
	if (status == LED_ON)
		gpiod_set_value(leddrv_data->desc, 1);
	else
		gpiod_set_value(leddrv_data->desc, 0);
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

	result = gpio_request(GPIO_NUM, DRIVER_NAME);
	if (result) {
		pr_err("%s: Error requesting gpio\n", DRIVER_NAME);
		goto ret_err_gpio_request;
	}

	leddrv_data->desc = gpio_to_desc(GPIO_NUM);

	leddrv_data->led_cdev.name = "ipe:red:user";
	leddrv_data->led_cdev.brightness_set = leddrv_change_state;

	result = led_classdev_register(NULL, &leddrv_data->led_cdev);
	if (result) {
		pr_err("%s: Error registering led\n", DRIVER_NAME);
		goto ret_err_led_classdev_register;
	}

	gpiod_direction_output(leddrv_data->desc, 0);

	pr_info("LED: Driver Insert... Done!!!\n");
	goto ret_ok;

ret_err_led_classdev_register:
	gpio_free(GPIO_NUM);
ret_err_gpio_request:
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
	gpio_free(GPIO_NUM);
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
