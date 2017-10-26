#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include <sched.h>
#include <sys/mman.h>

#include "stats.h"
#include "display.h"

/* From */
/* http://www.airspayce.com/mikem/bcm2835/ */
int display_enable_realtime(void) {

	// previously was just doing this
	// setpriority(PRIO_PROCESS, 0, -20);

	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &sp);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	return 0;
}




