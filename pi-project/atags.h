struct atag_info_t {
        uint32_t revision;
        uint32_t ramsize;
	uint32_t hardware_type;
        uint32_t framebuffer_x;
        uint32_t framebuffer_y;
};


void atags_dump(uint32_t *atags);
void atags_detect(uint32_t *atags, struct atag_info_t *info);

#define ATAG_NONE	0x00000000	// 2	Empty tag used to end list
#define ATAG_CORE	0x54410001	// 5 (2 if empty)	First tag used to start list
#define ATAG_MEM	0x54410002	// 4	Describes a physical area of memory
#define ATAG_VIDEOTEXT	0x54410003	// 5	Describes a VGA text display
#define ATAG_RAMDISK	0x54410004	// 5	Describes how the ramdisk will be used in kernel
#define ATAG_INITRD2	0x54420005	// 4	Describes where the compressed ramdisk image is placed in memory
#define ATAG_SERIAL	0x54410006	// 4	64 bit board serial number
#define ATAG_REVISION	0x54410007	// 3	32 bit board revision number
#define ATAG_VIDEOLFB	0x54410008	// 8	Initial values for vesafb-type framebuffers
#define ATAG_CMDLINE	0x54410009	// 2 + ((length_of_cmdline + 3) / 4)	Command line to pass to kernel 

