#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "serial.h"
#include "framebuffer_console.h"

int console_write(const void *buf, size_t count) {

	int result;

	/* Write to framebuffer */
        result=framebuffer_console_write(buf, count);

	/* We only support UART for now */
	result=uart_write(buf, count);

	return result;
}

int console_read(void *buf, size_t count) {

        int i;
        char *buffer=buf;

        /* We only support UART for now */

        for(i=0;i<count;i++) {
                buffer[i]=uart_getc_noblock();
                if (buffer[i]<=0) break;
        }

        return i;

}

