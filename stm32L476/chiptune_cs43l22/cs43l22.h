#define CS43L22_REG_ID			0x01
#define CS43L22_REG_POWER_CTL1		0x02
#define CS43L22_REG_POWER_CTL2		0x04
#define CS43L22_REG_CLOCKING_CTL	0x05
#define	CS43L22_REG_INTERFACE_CTL1	0x06

void cs43l22_init(void);
void cs43l22_disable(void);
void cs43l22_play(void);
void cs43l22_set_volume(int volume);
