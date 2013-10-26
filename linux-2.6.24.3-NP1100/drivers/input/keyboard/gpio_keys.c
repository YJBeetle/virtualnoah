/*
 * linux/drivers/input/keyboard/gpio_keys.c
 *
 * JZ GPIO Buttons driver for JZ4740 PAVO
 *
 * User applications can access to this device via /dev/input/eventX.
 *
 * Copyright (c) 2005 - 2008  Ingenic Semiconductor Inc.
 *
 * Author: Richard <cjfeng@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <asm/gpio.h>
#include <asm/jzsoc.h>


#define SCAN_INTERVAL       (10)

/*
 * GPIO Buttons
 */
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button pavo_buttons[] = {
	{
		.gpio		= 96,
		.code        	= KEY_1,
		.desc		= "Button 0",
		.active_low	= 1,
	},
	{
		.gpio		= 97,
		.code   	= KEY_2,
		.desc		= "Button 1",
		.active_low	= 1,
	},
	{
		.gpio		= 98,
		.code   	= KEY_3,
		.desc		= "Button 2",
		.active_low	= 1,
	},
	{
		.gpio		= 99,
		.code   	= KEY_4,
		.desc		= "Button 3",
		.active_low	= 1,
	}
};

static struct timer_list button_timer;
static spinlock_t gpio_lock;
static int button_no;

static struct gpio_keys_platform_data pavo_button_data = {
	.buttons	= pavo_buttons,
	.nbuttons	= ARRAY_SIZE(pavo_buttons),
};

static struct platform_device pavo_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &pavo_button_data,
	}
};

static void __init pavo_add_device_buttons(void)
{
	__gpio_as_input(96);
	__gpio_as_irq_fall_edge(96);

	__gpio_as_input(97);
	__gpio_as_irq_fall_edge(97);

	__gpio_as_input(98);
	__gpio_as_irq_fall_edge(98);

	__gpio_as_input(99);
	__gpio_as_irq_fall_edge(99);

	platform_device_register(&pavo_button_device);
}
#else
static void __init pavo_add_device_buttons(void) {}
#endif

static void __init pavo_board_init(void)
{
	/* Push Buttons */
	pavo_add_device_buttons();
}

static void button_timer_callback(unsigned long data)
{
	unsigned long flags;
	int gpio = pavo_buttons[button_no].gpio;
	int code = pavo_buttons[button_no].code;
	struct platform_device *pdev = (struct platform_device *)data;
	struct input_dev *input = platform_get_drvdata(pdev);
	int state;

	spin_lock_irqsave(&gpio_lock, flags);
	state = __gpio_get_pin(gpio);
        
	if (state == 0) {
		/* press down */
		input_report_key(input, code, 1);
		input_sync(input);
		mod_timer(&button_timer, jiffies + SCAN_INTERVAL);
	} else {
		/* up */
		input_report_key(input, code, 0);
		input_sync(input);
		udelay(1000);
		__gpio_as_irq_fall_edge(gpio);
	}
	spin_unlock_irqrestore(&gpio_lock, flags);
}

static irqreturn_t gpio_keys_isr(int irq, void *dev_id)
{
	int i;
	struct platform_device *pdev = dev_id;
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;

	__gpio_ack_irq(irq - IRQ_GPIO_0);
	for (i = 0; i < pdata->nbuttons; i++) {
		struct gpio_keys_button *button = &pdata->buttons[i];
		int gpio = button->gpio;

		if (irq == (gpio + IRQ_GPIO_0) ) {
			/* start timer */
			__gpio_as_input(gpio);
			button_no = i;
			mod_timer(&button_timer, jiffies +  2 * SCAN_INTERVAL);
			break;
		}
	}

	return IRQ_HANDLED;
}

static int __devinit gpio_keys_probe(struct platform_device *pdev)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	struct input_dev *input;
	int i, error;
	int wakeup = 0;

	input = input_allocate_device();
	if (!input)
		return -ENOMEM;

	platform_set_drvdata(pdev, input);

	spin_lock_init(&gpio_lock);

	input->name = pdev->name;
	input->phys = "gpio-keys/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;
	input->evbit[0] = BIT(EV_KEY) | BIT(EV_SYN) | BIT(EV_REP);

	for (i = 0; i < pdata->nbuttons; i++) {
		struct gpio_keys_button *button = &pdata->buttons[i];
		int irq;
		unsigned int type = button->type ?: EV_KEY;

		irq = IRQ_GPIO_0 + button->gpio;
		if (irq < 0) {
			error = irq;
			pr_err("gpio-keys: Unable to get irq number"
				" for GPIO %d, error %d\n",
				button->gpio, error);
			goto fail;
		}

		error = request_irq(irq, gpio_keys_isr,
				    IRQF_SAMPLE_RANDOM | IRQF_DISABLED,
				    button->desc ? button->desc : "gpio_keys",
				    pdev);
		if (error) {
			pr_err("gpio-keys: Unable to claim irq %d; error %d\n",
				irq, error);
			goto fail;
		}

		if (button->wakeup)
			wakeup = 1;
		input_set_capability(input, type, button->code);
	}

	/* Init timer */
	init_timer(&button_timer);
	button_timer.data = (unsigned long)&pavo_button_device;
	button_timer.function = button_timer_callback;

	error = input_register_device(input);
	if (error) {
		pr_err("gpio-keys: Unable to register input device, "
			"error: %d\n", error);
		goto fail;
	}

	device_init_wakeup(&pdev->dev, wakeup);

	return 0;

 fail:
	while (--i >= 0) {
		free_irq(pdata->buttons[i].gpio + IRQ_GPIO_0 , pdev);
	}

	platform_set_drvdata(pdev, NULL);
	input_free_device(input);

	return error;
}

static int __devexit gpio_keys_remove(struct platform_device *pdev)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	struct input_dev *input = platform_get_drvdata(pdev);
	int i;

	device_init_wakeup(&pdev->dev, 0);

	for (i = 0; i < pdata->nbuttons; i++) {
		int irq = pdata->buttons[i].gpio + IRQ_GPIO_0;
		free_irq(irq, pdev);
	}

	input_unregister_device(input);

	return 0;
}


#ifdef CONFIG_PM
static int gpio_keys_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	int i;

	if (device_may_wakeup(&pdev->dev)) {
		for (i = 0; i < pdata->nbuttons; i++) {
			struct gpio_keys_button *button = &pdata->buttons[i];
			if (button->wakeup) {
				int irq = button->gpio + IRQ_GPIO_0;
				enable_irq_wake(irq);
			}
		}
	}

	return 0;
}

static int gpio_keys_resume(struct platform_device *pdev)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	int i;

	if (device_may_wakeup(&pdev->dev)) {
		for (i = 0; i < pdata->nbuttons; i++) {
			struct gpio_keys_button *button = &pdata->buttons[i];
			if (button->wakeup) {
				int irq = button->gpio + IRQ_GPIO_0;
				disable_irq_wake(irq);
			}
		}
	}

	return 0;
}
#else
#define gpio_keys_suspend	NULL
#define gpio_keys_resume	NULL
#endif

struct platform_driver gpio_keys_device_driver = {
	.probe		= gpio_keys_probe,
	.remove		= __devexit_p(gpio_keys_remove),
	.suspend	= gpio_keys_suspend,
	.resume		= gpio_keys_resume,
	.driver		= {
		.name	= "gpio-keys",
	}
};

static int __init gpio_keys_init(void)
{
	pavo_board_init();
	return platform_driver_register(&gpio_keys_device_driver);
}

static void __exit gpio_keys_exit(void)
{
	platform_device_unregister(&pavo_button_device);
	platform_driver_unregister(&gpio_keys_device_driver);
}

module_init(gpio_keys_init);
module_exit(gpio_keys_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Keyboard driver for CPU GPIOs");
