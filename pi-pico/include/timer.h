typedef uint64_t absolute_time_t;

#define TIMER_BASE 0x40054000
#define NUM_TIMERS 4

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;

typedef struct {
    io_wo_32 timehw;
    io_wo_32 timelw;
    io_ro_32 timehr;
    io_ro_32 timelr;
    io_rw_32 alarm[NUM_TIMERS];
    io_rw_32 armed;
    io_ro_32 timerawh;
    io_ro_32 timerawl;
    io_rw_32 dbgpause;
    io_rw_32 pause;
    io_rw_32 intr;
    io_rw_32 inte;
    io_rw_32 intf;
    io_ro_32 ints;
} timer_hw_t;

#define timer_hw ((timer_hw_t *const)TIMER_BASE)

void sleep_ms(uint32_t ms);
