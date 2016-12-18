/* Test using the 74HC595 shift reg with Raspberry Pi */

/* The outputs Q0-Q7 are hooked to LEDs through 470 Ohm resistors */
/* DS connects to GPIO17 */
/* SHCP/SRCLK connects to GPIO27 */
/* STCP/RCLK connects to GPIO22 */
/* |OE is tied to ground */
/* |MR |SRCLR is tied to +5V */

/* UGH!  Upstream kernels start GPIO at 970 on the Pi for some reason */
/* I'm developing this on upstream 4.8-rc3 for some reason */
#define KERNEL 970

#include <stdio.h>
#include <unistd.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int i,odd=0;

	printf("74HC595 LED Test test\n");

	/* Enable GPIO17 */
	gpio_enable(17+KERNEL);
	gpio_set_write(17+KERNEL);

	/* Enable GPIO27 */
	gpio_enable(27+KERNEL);
	gpio_set_write(27+KERNEL);

	/* Enable GPIO22 */
	gpio_enable(22+KERNEL);
	gpio_set_write(22+KERNEL);

	while(1) {

		for(i=0;i<8;i++) {

			/* Set clock low */
			gpio_write(27+KERNEL,0);

			/* Set bit to output */
			if (odd) {
				gpio_write(17+KERNEL,i&1);
			}
			else {
				gpio_write(17+KERNEL,!(i&1));
			}

			/* Set clock high */
			gpio_write(27+KERNEL,1);
		}

		/* Write to output buffer */

		gpio_write(22+KERNEL,1);
		gpio_write(22+KERNEL,0);

		sleep(1);
		odd=!odd;

	}

	return 0;
}
