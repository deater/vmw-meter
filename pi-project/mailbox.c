/* based on info at https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/screen01.html */
/* https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes */
/* See also arch/arm/mach-bcm2708/vcio.c */

#include <stddef.h>
#include <stdint.h>

#include "printk.h"
#include "mmio.h"

#include "mailbox.h"

int mailbox_write(unsigned int value, unsigned int channel) {

	printk("MAILBOX_WRITE: writing value=%x channel %x\n",
		value,channel);

	/* Bottom 4 bits of value must be 0 */
	if (value&0xf) {
		printk("mailbox_write: bottom bits not zero %x\n",
			value);
		return -1;
	}

	/* Channel must fit in 4 bits */
	if (channel>15) {
		printk("mailbox_write: channel too high %x\n",
			channel);
		return -1;
	}

	/* Wait until mailbox is ready */

	while( (mmio_read(MAILBOX_STATUS) & MAIL_FULL) ) {
		printk("Write mailbox full!\n");
	}

	/* write the command */
	mmio_write(MAILBOX_WRITE,channel|value);

	return 0;
}

int mailbox_read(unsigned int channel) {

	unsigned int mail;

	printk("MAILBOX_READ: reading channel %x\n",
		channel);

	/* Channel must be 4-bits */
	if (channel>15) {
		printk("mailbox_read: channel too high\n",
			channel);
		return -1;
	}

	/* Wait until mailbox has something there */

	while((mmio_read(MAILBOX_STATUS) & MAIL_EMPTY) ) {
		printk("mailbox_read: mail_empty\n");
	}

	mail=mmio_read(MAILBOX_READ);

	/* Got mail from the wrong channel!*/
	/* FIXME: Should we try again? */

	if ((mail&0xf)!=channel) {
		printk("mailbox_read: read from wrong channel %x %x\n",
			mail&0xf,channel);
		return -1;
	}

	/* Return top 28 bits */
	return mail&0xfffffff0;
}
