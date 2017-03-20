/* Test using the 74HC595 shift reg with Raspberry Pi */

/* The outputs Q0-Q7 are hooked to LEDs through 470 Ohm resistors */
/* DS connects to GPIO17 */
/* SHCP/SRCLK connects to GPIO27 */
/* STCP/RCLK connects to GPIO22 */
/* |OE is tied to ground */
/* |MR |SRCLR is tied to +5V */

/* UGH!  Upstream kernels start GPIO at 970 on the Pi for some reason */
/* I'm developing this on upstream 4.8-rc3 for some reason */
//#define KERNEL 970

#define KERNEL 0

#if 1
#define GPIO_MOSI	10
#define GPIO_CLK	11
#define GPIO_CE0	8
#else
#define GPIO_MOSI	17
#define GPIO_CLK	27
#define GPIO_CE0	22
#endif


#include <stdio.h>
#include <unistd.h>

#include "gpio_lib.h"

int main(int argc, char **argv) {

	int i,odd=0;

	printf("74HC595 LED Test test\n");

	/* Enable GPIO17 */
	gpio_enable(GPIO_MOSI+KERNEL);
	gpio_set_write(GPIO_MOSI+KERNEL);

	/* Enable GPIO27 */
	gpio_enable(GPIO_CLK+KERNEL);
	gpio_set_write(GPIO_CLK+KERNEL);

	/* Enable GPIO22 */
	gpio_enable(GPIO_CE0+KERNEL);
	gpio_set_write(GPIO_CE0+KERNEL);

	while(1) {

		for(i=0;i<8;i++) {

			/* Set clock low */
			gpio_write(GPIO_CLK+KERNEL,0);

			/* Set bit to output */
			if (odd) {
				gpio_write(GPIO_MOSI+KERNEL,i&1);
			}
			else {
				gpio_write(GPIO_MOSI+KERNEL,!(i&1));
			}

			/* Set clock high */
			gpio_write(GPIO_CLK+KERNEL,1);
		}

		/* Write to output buffer */

		gpio_write(GPIO_CE0+KERNEL,1);
		gpio_write(GPIO_CE0+KERNEL,0);

		sleep(1);
		odd=!odd;

	}

	return 0;
}
