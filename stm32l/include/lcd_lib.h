#define SEGA	0x8000
#define SEGB	0x4000
#define SEGC	0x2000
#define SEGD	0x1000
#define SEGE	0x0800
#define SEGF	0x0400
#define SEGG	0x0200
#define SEGH	0x0100
#define SEGJ	0x0080
#define SEGK	0x0040
#define SEGM	0x0020
#define SEGN	0x0010
#define SEGP	0x0008
#define SEGQ	0x0004
#define SEGCOLON	0x0002
#define SEGBAR		0x0002
#define SEGDP		0x0001

void lcd_clock_init(void);
void lcd_pin_init(void);
void lcd_config(void);
void lcd_display(unsigned int *buffer);
void lcd_convert(char *string, unsigned int *buffer);
