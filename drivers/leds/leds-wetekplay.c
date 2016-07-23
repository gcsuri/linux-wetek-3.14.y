/*
 * LEDs driver for the "User LED" on WeTek.Play
 *
 * Copyright (C) 2015 Memphiz <memphiz@kodi.tv>
 *
 * Based on leds.rb532
 */

#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/amlogic/aml_gpio_consumer.h>
#include <linux/amlogic/gpio-amlogic.h>
#include <linux/amlogic/sd.h>
#include <linux/amlogic/iomap.h>
#include <dt-bindings/gpio/gxbb.h>


#define GPIO_OWNER_WIFILED "WIFILED"
#define GPIO_OWNER_ETHLED "ETHLED"
#define GPIO_OWNER_PWRLED "PWRLED"

int wifiLedGpio = -1;
int ethLedGpio = -1;
int pwrLedGpio = -1;

static void wetekplay_powerled_set(struct led_classdev *cdev,
			  enum led_brightness brightness)
{

	if (pwrLedGpio > 0)
		gpio_direction_output(pwrLedGpio, brightness > 0 ? 1 : 0);
}

static enum led_brightness wetekplay_powerled_get(struct led_classdev *cdev)
{
	if (gpio_get_value(pwrLedGpio))
	    return 255;
	else
	    return 0;
}

static void wetekplay_wifiled_set(struct led_classdev *cdev,
			  enum led_brightness brightness)
{
	if (wifiLedGpio > 0)
		gpio_direction_output(wifiLedGpio, brightness > 0 ? 1 : 0);
}

static enum led_brightness wetekplay_wifiled_get(struct led_classdev *cdev)
{
	if (gpio_get_value(wifiLedGpio))
	    return 255;
	else
	    return 0;
}

static void wetekplay_ethled_set(struct led_classdev *cdev,
			  enum led_brightness brightness)
{
	if (ethLedGpio > 0)
		gpio_direction_output(ethLedGpio, brightness > 0 ? 1 : 0);
}

static enum led_brightness wetekplay_ethled_get(struct led_classdev *cdev)
{
	if (gpio_get_value(ethLedGpio))
	    return 255;
	else
	    return 0;
}



static struct led_classdev wetekplay_powerled = {
	.name = "wetek:blue:powerled",
	.brightness_set = wetekplay_powerled_set,
	.brightness_get = wetekplay_powerled_get,
	.default_trigger = "default-on",
};

static struct led_classdev wetekplay_wifiled = {
	.name = "wetek:blue:wifiled",
	.brightness_set = wetekplay_wifiled_set,
	.brightness_get = wetekplay_wifiled_get,
	.default_trigger = "wifilink",
};

static struct led_classdev wetekplay_ethled = {
	.name = "wetek:blue:ethled",
	.brightness_set = wetekplay_ethled_set,
	.brightness_get = wetekplay_ethled_get,
	.default_trigger = "ethlink",
};

static int wetekplay_led_probe(struct platform_device *pdev)
{
	int ret;
	const char *str;
	struct gpio_desc *desc;
	
	ret = of_property_read_string(pdev->dev.of_node,
		"eth_led", &str);
	if (!ret) {
		desc = of_get_named_gpiod_flags(pdev->dev.of_node,
			"eth_led", 0, NULL);
		ethLedGpio = desc_to_gpio(desc);
		gpio_request(ethLedGpio, GPIO_OWNER_ETHLED);
	}
	
	ret = of_property_read_string(pdev->dev.of_node,
		"wifi_led", &str);
	if (!ret) {						
		desc = of_get_named_gpiod_flags(pdev->dev.of_node,
			"wifi_led", 0, NULL);
		wifiLedGpio = desc_to_gpio(desc);
		gpio_request(wifiLedGpio, GPIO_OWNER_WIFILED);
	}
	ret = of_property_read_string(pdev->dev.of_node,
		"pwr_led", &str);
	if (!ret) {						
		desc = of_get_named_gpiod_flags(pdev->dev.of_node,
			"pwr_led", 0, NULL);
		pwrLedGpio = desc_to_gpio(desc);
		gpio_request(pwrLedGpio, GPIO_OWNER_PWRLED);
	}

	if (wifiLedGpio > 0)
		gpio_direction_output(wifiLedGpio, 0);
	if (ethLedGpio > 0)
		gpio_direction_output(ethLedGpio, 0);
	led_classdev_register(&pdev->dev, &wetekplay_powerled);
	led_classdev_register(&pdev->dev, &wetekplay_wifiled);
	return led_classdev_register(&pdev->dev, &wetekplay_ethled);
}

static int wetekplay_led_remove(struct platform_device *pdev)
{

	led_classdev_unregister(&wetekplay_powerled);
	led_classdev_unregister(&wetekplay_wifiled);
	led_classdev_unregister(&wetekplay_ethled);
	return 0;
}

static const struct of_device_id amlogic_wetekplayled_dt_match[]={
	{	.compatible = "amlogic,wetekplay-led",
	},
	{},
};


static struct platform_driver wetekplay_led_driver = {
	.probe = wetekplay_led_probe,
	.remove = wetekplay_led_remove,
	.driver = {
		.name = "wetekplay-led",
		.of_match_table = amlogic_wetekplayled_dt_match,
	},
};

static int __init
wetekplay_led_init_module(void)
{
    int err;

    printk("wetekplay_led_init_module\n");
    if ((err = platform_driver_register(&wetekplay_led_driver))) {
        return err;
    }

    return err;

}

static void __exit
wetekplay_led_remove_module(void)
{
    platform_driver_unregister(&wetekplay_led_driver);
    printk("wetekplay-led module removed.\n");
}


//module_platform_driver(wetekplay_led_driver);
module_init(wetekplay_led_init_module);
module_exit(wetekplay_led_remove_module);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Power LED support for WeTek.Play");
MODULE_AUTHOR("Memphiz <memphiz@kodi.tv>");
MODULE_ALIAS("platform:wetekplay-led");
