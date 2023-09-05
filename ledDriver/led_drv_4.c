/***************************************************************************//**
*  file       led_drv.c
*
*  details    Sample LED driver using bus infrastructur 
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
#include <linux/leds.h>              //copy_to/from_user()
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>

/**
 * Define the driver name and other macro
 */
#define DRIVER_NAME "leddrv"

#define LED_OFF		0
#define LED_ON		1

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
 * This is probe routine called once ID is match
 */
static int leddrv_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child = NULL;
	int result, gpio;

	child = of_get_next_child(np, NULL);

	leddrv_data = kzalloc(sizeof(*leddrv_data), GFP_KERNEL);
	if (!leddrv_data) {
		return -ENOMEM;
	}

	gpio = of_get_named_gpio(child, "gpio-name", 0);

	result = devm_gpio_request(&pdev->dev, gpio, pdev->name);
	if (result) {
		dev_err(&pdev->dev, "Error requesting gpio\n");
		return result;
	}

	leddrv_data->desc = gpio_to_desc(gpio);

	leddrv_data->led_cdev.name = of_get_property(child, "label", NULL);
	leddrv_data->led_cdev.brightness_set = leddrv_change_state;


	dev_info(&pdev->dev, "Initialized!!!\n");
	return 0;
}

/**
 * Remove the module
 */
static int leddrv_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Exiting.\n");
	return 0;
}

static struct of_device_id of_leddrv_match[] = {
	{ .compatible = "rndworks,leddrv" },
	{}
};

static struct platform_driver leddrv_driver = {
	.driver = {
		.name	= "leddrv",
		.owner	= THIS_MODULE,
		.of_match_table	= of_leddrv_match,
	},
	.probe	= leddrv_probe,
	.remove	= leddrv_remove,
};

/**
 * Call the platform driver function from macro
 */
module_platform_driver(leddrv_driver);

/**
 * Add the module information
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AmitK <kumaramit124@gmail.com>");
MODULE_DESCRIPTION("Linux LED device driver");
MODULE_VERSION("1.0");
