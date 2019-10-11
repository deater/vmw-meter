#if 0
static inline uint32_t syscall3(int arg0, int arg1, int arg2, int which) {

        uint32_t result;

        asm volatile (  "mov r0, %[arg0]\n"
                        "mov r1, %[arg1]\n"
                        "mov r2, %[arg2]\n"
                        "mov r7, %[which]\n"
                        "swi 0\n"
                        "mov %[result], r0\n"
                :       [result] "=r" (result)
                :       [arg0] "r" (arg0),
                        [arg1] "r" (arg1),
                        [arg2] "r" (arg2),
                        [which] "r" (which)
                :       "r0", "r1", "r2", "r7" );

        return result;

}
#endif

static inline uint32_t syscall3(long sysnum, long a, long b, long c) {

	register long _r0 asm("r0")=(long)(a);
	register long _r1 asm("r1")=(long)(b);
	register long _r2 asm("r2")=(long)(c);
	register long _r7 asm("r7")=(long)(sysnum);

	asm volatile(
		"swi #0\n"
		: "=r"(_r0)				/* Outputs */
		:  "r"(_r0), "r"(_r1), "r"(_r2),	/* Inputs */
		   "r"(_r7)
		: "memory");				/* Clobbers */

	if(_r0 >=(unsigned long) -4095) {
		/* FIXME: Update errno */
		_r0=(unsigned long) -1;
	}
	return (long) _r0;

}

static inline uint32_t syscall1(long sysnum, long a) {

	register long _r0 asm("r0")=(long)(a);
	register long _r7 asm("r7")=(long)(sysnum);

	asm volatile(
		"swi #0\n"
		: "=r"(_r0)				/* Outputs */
		:  "r"(_r0),	/* Inputs */
		   "r"(_r7)
		: "memory");				/* Clobbers */

	if(_r0 >=(unsigned long) -4095) {
		/* FIXME: Update errno */
		_r0=(unsigned long) -1;
	}
	return (long) _r0;

}


#define STDIN	0
#define STDOUT	1
#define STDERR	2

/* Same as Linux for compatibility */
//#define SYSCALL_EXIT		1
//#define SYSCALL_FORK		2
#define SYSCALL_READ		3
#define SYSCALL_WRITE		4
//#define SYSCALL_OPEN		5
//#define SYSCALL_CLOSE		6
//#define SYSCALL_EXECVE	11
#define SYSCALL_TIME		13
#define SYSCALL_GETPID		20
#define SYSCALL_IOCTL		54
#define SYSCALL_REBOOT		88
#define SYSCALL_NANOSLEEP	162

/* VMW syscalls */
#define SYSCALL_BLINK		8192
#define SYSCALL_SETFONT		8193
#define SYSCALL_GRADIENT	8194
#define SYSCALL_TB1		8195
#define SYSCALL_RUN		8196
#define SYSCALL_STOP		8197
