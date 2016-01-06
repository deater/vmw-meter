/* pi-ps2gpio.c -- Linux kernel driver for ps2pi PS/2 keyboard/GPIO device
 *	by Vince Weaver <vincent.weaver _at_ maine.edu>
 *
 * This is mostly proof-of concept code, it needs a lot of work to
 * become a proper driver (and to reduce the latency).
 *
 * Based on the ps2pi.c driver by the pi hacker
 *   https://sites.google.com/site/thepihacker
 *
 * Other documentation that was a big help:
 *   GPIO in the kernel: an introduction by Jonathan Corbet
 *   https://lwn.net/Articles/532714/
 *
 *
 *
 *   This file is subject to the terms and conditions of the GNU General Public
 *   License. See the file COPYING in the Linux kernel source for more details.
 *
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <linux/gpio.h>

static int irq_num;

/* Default for the VMW ps2pi board */
int gpio_clk = 23;
int gpio_data = 24;

static unsigned keyup = 0;
static unsigned escape = 0;
static unsigned pause = 0;

static struct input_dev *ps2;

static unsigned char translate[256] = {

/* Raw SET 2 scancode table */

/* 00 */  KEY_RESERVED, KEY_F9,        KEY_RESERVED,  KEY_F5,        KEY_F3,        KEY_F1,       KEY_F2,        KEY_F12,
/* 08 */  KEY_ESC,      KEY_F10,       KEY_F8,        KEY_F6,        KEY_F4,        KEY_TAB,      KEY_GRAVE,     KEY_RESERVED,
/* 10 */  KEY_RESERVED, KEY_LEFTALT,   KEY_LEFTSHIFT, KEY_RESERVED,  KEY_LEFTCTRL,  KEY_Q,        KEY_1,         KEY_RESERVED,
/* 18 */  KEY_RESERVED, KEY_RESERVED,  KEY_Z,         KEY_S,         KEY_A,         KEY_W,        KEY_2,         KEY_RESERVED, 
/* 20 */  KEY_RESERVED, KEY_C,         KEY_X,         KEY_D,         KEY_E,         KEY_4,        KEY_3,         KEY_RESERVED,
/* 28 */  KEY_RESERVED, KEY_SPACE,     KEY_V,         KEY_F,         KEY_T,         KEY_R,        KEY_5,         KEY_RESERVED,
/* 30 */  KEY_RESERVED, KEY_N,         KEY_B,         KEY_H,         KEY_G,         KEY_Y,        KEY_6,         KEY_RESERVED,
/* 38 */  KEY_RESERVED, KEY_RIGHTALT,  KEY_M,         KEY_J,         KEY_U,         KEY_7,        KEY_8,         KEY_RESERVED,
/* 40 */  KEY_RESERVED, KEY_COMMA,     KEY_K,         KEY_I,         KEY_O,         KEY_0,        KEY_9,         KEY_RESERVED,
/* 48 */  KEY_RESERVED, KEY_DOT,       KEY_SLASH,     KEY_L,         KEY_SEMICOLON, KEY_P,        KEY_MINUS,     KEY_RESERVED,
/* 50 */  KEY_RESERVED, KEY_RESERVED,  KEY_APOSTROPHE,KEY_RESERVED,  KEY_LEFTBRACE, KEY_EQUAL,    KEY_RESERVED,  KEY_RESERVED,
/* 58 */  KEY_CAPSLOCK, KEY_RIGHTSHIFT,KEY_ENTER,     KEY_RIGHTBRACE,KEY_RESERVED,  KEY_BACKSLASH,KEY_RESERVED,  KEY_RESERVED,
/* 60 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_BACKSPACE, KEY_RESERVED,
/* 68 */  KEY_RESERVED, KEY_KP1,       KEY_RESERVED,  KEY_KP4,       KEY_KP7,       KEY_RESERVED, KEY_HOME,      KEY_RESERVED,
/* 70 */  KEY_KP0,      KEY_KPDOT,     KEY_KP2,       KEY_KP5,       KEY_KP6,       KEY_KP8,      KEY_ESC,       KEY_NUMLOCK,
/* 78 */  KEY_F11,      KEY_KPPLUS,    KEY_KP3,       KEY_KPMINUS,   KEY_KPASTERISK,KEY_KP9,      KEY_SCROLLLOCK,KEY_RESERVED,
/* 80 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_F7,        KEY_SYSRQ,     KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* 88 */  KEY_PAUSE,    KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* 90 */  KEY_RESERVED, KEY_RIGHTALT,  KEY_RESERVED,  KEY_RESERVED,  KEY_RIGHTCTRL, KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* 98 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* a0 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* a8 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* b0 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* b8 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* c0 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* c8 */  KEY_RESERVED, KEY_RESERVED,  KEY_KPSLASH,   KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* d0 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* d8 */  KEY_RESERVED, KEY_RESERVED,  KEY_KPENTER,   KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* e0 */  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* e8 */  KEY_RESERVED, KEY_END,       KEY_RESERVED,  KEY_LEFT,      KEY_HOME,      KEY_RESERVED, KEY_RESERVED,  KEY_RESERVED,
/* f0 */  KEY_INSERT,   KEY_DELETE,    KEY_DOWN,      KEY_RESERVED,  KEY_RIGHT,     KEY_UP,       KEY_RESERVED,  KEY_RESERVED,
/* f8 */  KEY_RESERVED, KEY_RESERVED,  KEY_PAGEDOWN,  KEY_RESERVED,  KEY_PRINT,     KEY_PAGEUP,   KEY_RESERVED,  KEY_RESERVED

};

module_param(gpio_clk,int,0);
module_param(gpio_data,int,0);

/* Handle GPIO interrupt, get keycode, send to event subsystem */

/* Pretty horrible code, as our state machine is not re-entrant */
/* and we busy-poll in the interrupt leading to potentially high latency */

irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *regs) {

	static unsigned key;

	int clk_value;
	int data_value;

	static int parity=0;
	static int clock_bits=0;
	static int message=0;

	/* Sanity check clock line is low? */
//	clk_value=gpio_get_value(gpio_clk);

	/* read the data line */
	clock_bits++;
	data_value=gpio_get_value(gpio_data);

	/* Shift in backwards as protocol is LSB first */
	parity+=data_value;
	message|=(data_value<<11);
	message>>=1;

	if (clock_bits!=11) {
		return 0;
	}

	/* We received our 11 bits */
	clock_bits=0;

	if (message&0x1) printk(KERN_INFO "Invalid start bit %x\n",message);
	if (!(message&0x400)) printk(KERN_INFO "Invaid stop bit %x\n",message);

	if ( ( ((message&0x200>>8)&0x1) + (parity&0x1) ) &0x1) {
		printk(KERN_INFO "Parity error\n");
	}

	key = (message>>1) & 0xff;

//	printk(KERN_INFO "key: %x\n",key);

	message=0;

	if (key == 0xf0) {
		keyup = 1;
		return 0;
	}

	if (key == 0xe0) {
		escape = 1;
		return 0;
	}

	if (key == 0xe1) {
		pause = 2;
		return 0;
	}

	if (pause == 2) {
		pause = 1;
		return 0;
	}

	if (pause == 1) {
		key = 0x88;
		pause = 0;
	}

	if (escape == 1) {
		key |= 0x80;
		escape = 0;
	}

	key = translate[key];

	if (keyup == 1) {
		input_report_key(ps2,key,0);
		keyup = 0;
	} else {
		input_report_key(ps2,key,1);
	}

	input_sync(ps2);

	return 0;

}

/* Initialize the Module */

int init_module(void) {

	static int i, retval;

	int result;

	/* Allocate GPIO23 and GPIO24 */
	/* TODO: make this configurable */

	result=gpio_request(gpio_clk, "ps2_clock");
	if (result<0) goto init_error;
	result=gpio_request(gpio_data, "ps2_data");
	if (result<0) goto init_error;

	/* Set to be inputs */
	gpio_direction_input(gpio_clk);
	gpio_direction_input(gpio_data);

	/* Get interrupt number for clock input */
	irq_num=gpio_to_irq(gpio_clk);
	if (irq_num<0) goto init_error;

	/* FIXME */
	/* should probe to make sure keyboard actually exists */

	/* Setup input device */
	ps2=input_allocate_device();
	ps2->name = "pi-ps2gpio";
	ps2->phys = "ps2/input0";
	ps2->id.bustype = BUS_HOST;
	ps2->id.vendor = 0x0001;
	ps2->id.product = 0x0001;
	ps2->id.version = 0x0100;
	ps2->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	ps2->keycode = translate;
	ps2->keycodesize = sizeof(unsigned char);
	ps2->keycodemax = ARRAY_SIZE(translate);
	for (i = 1; i < 0x256; i++) set_bit(i,ps2->keybit);
	retval = input_register_device(ps2);

	retval = request_irq(
		irq_num,
		(irq_handler_t)irq_handler,
		IRQF_TRIGGER_FALLING,
		"pi-ps2gpio",
		(void *)irq_handler);

	/* numlock on, should probaby query state first */
	input_report_key(ps2,KEY_NUMLOCK,1);
	input_sync(ps2);

	printk(KERN_INFO "pi-ps2gpio installed using GPIO23+24, irq %d\n",
		irq_num);

	return 0;

init_error:

	printk(KERN_INFO "pi-ps2gpio installation failed\n");

	return -ENODEV;

}

/* Remove module */
void cleanup_module(void) {

	input_unregister_device(ps2);
	free_irq(irq_num,(void *)irq_handler);

	gpio_free(gpio_data);
	gpio_free(gpio_clk);

	printk(KERN_INFO "pi-ps2gpio device removed\n");

	return;

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vince Weaver <vincent.weaver@maine.edu>");
