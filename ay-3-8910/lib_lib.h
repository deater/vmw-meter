extern int display_type;

void quiet_and_exit(int sig);
int sleep_unless_keypress(int seconds);

int lib_clock(int alarm_hour, int alarm_minute);
int lib_pom(void);
int lib_temperature(void);

