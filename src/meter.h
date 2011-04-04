void init_meter();
void reset_display();
void emulate_i2c(char *buffer, int length);


#define CONTROL_STATIC    0x01
#define CONTROL_UNBLANK13 0x02
#define CONTROL_UNBLANK24 0x04
#define CONTROL_OUTTEST   0x08
#define CONTROL_PLUS3MA   0x10
#define CONTROL_PLUS6MA   0x20
#define CONTROL_PLUS12MA  0x40

#define SEGMENT_A  0x8000
#define SEGMENT_B  0x4000
#define SEGMENT_C  0x2000
#define SEGMENT_D  0x1000
#define SEGMENT_E  0x0800
#define SEGMENT_F  0x0400
#define SEGMENT_G  0x0200
#define SEGMENT_H  0x0100
#define SEGMENT_J  0x0080
#define SEGMENT_K  0x0040
#define SEGMENT_L  0x0020
#define SEGMENT_M  0x0010
#define SEGMENT_N  0x0008
#define SEGMENT_P  0x0004
#define SEGMENT_DP 0x0002
#define SEGMENT_EX 0x0001

