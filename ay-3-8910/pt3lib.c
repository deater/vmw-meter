/* Code based on z80 assembly code:
 Vortex Tracker II v1.0 PT3 player for ZX Spectrum
 ROM version (specially for Axor)
 (c)2004,2007 S.V.Bulba <vorobey@mail.khstu.ru>
 http://bulba.untergrund.net (http://bulba.at.kz)
*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* DEFC is Define Constant */

/* AY-3-8910 Register definitions */

#define	TonA	0
#define TonB	2
#define	TonC	4
#define	Noise	6
#define	Mixer	7
#define	AmplA	8
#define	AmplB	9
#define	AmplC	10
#define	Env	11
#define	EnvTp	13


/*ChannelsVars STRUCT	CHP */
/* reset group */
#define CHP_PsInOr	0
#define CHP_PsInSm	1
#define CHP_CrAmSl	2
#define CHP_CrNsSl	3
#define CHP_CrEnSl	4
#define CHP_TSlCnt	5
#define CHP_CrTnSl	6
#define CHP_TnAcc	8
#define CHP_COnOff	10
/* reset group */

#define CHP_OnOffD	11

/* IX for PTDECOD here (+12) */
#define CHP_OffOnD	12
#define CHP_OrnPtr	13
#define CHP_SamPtr	15
#define CHP_NNtSkp	17
#define CHP_Note	18
#define CHP_SlToNt	19
#define CHP_Env_En	20
#define CHP_Flags	21

/* Enabled - 0,SimpleGliss - 2 */
#define CHP_TnSlDl	22
#define CHP_TSlStp	23
#define CHP_TnDelt	25
#define CHP_NtSkCn	27
#define CHP_Volume	28
/* 	ENDS */
#define CHP 		29

/*
;vars from here can be stripped
;you can move VARS to any other address

VARS:

;vars in code and other self-modified code moved here
;(for ROM and RAM separation)
*/

#define SETUP	0x100 //static unsigned char SETUP=0;

/*SETUP:
	DEFB 0 ;set bit0 to 1, if you want to play without looping
	     ;bit7 is set each time, when loop point is passed
*/
static short CrPsPtr=0;	// CrPsPtr: DEFW 0
/*
AddToEn:
	DEFB 0
AdInPtA:
	DEFW 0
AdInPtB:
	DEFW 0
AdInPtC:
	DEFW 0
Env_Del:
	DEFB 0
*/
static short MODADDR;	// MODADDR:DEFW 0
/*
ESldAdd:
	DEFW 0
*/
static unsigned char Delay;	//Delay: DEFB 0
/*
PDSP_:
CSP_:
PSP_:
	DEFW 0
*/
unsigned short SamPtrs=0;	// SamPtrs: DEFW 0
unsigned short OrnPtrs=0;	// OrnPtrs: DEFW 0
unsigned short PatsPtr=0;	// PatsPtr: DEFW 0
unsigned short LPosPtr=0;	// LPosPtr: DEFW 0
/*
L3:
M2:
PrSlide:
	DEFW 0
PrNote:
	DEFB 0
Version:
	DEFB 0
;end of moved vars and self-modified code

VAR0START: ;START of INITZERO area

ChanA:
	DEFS CHP
ChanB:
	DEFS CHP
ChanC:
	DEFS CHP

;GlobalVars
DelyCnt:
	DEFB 0
CurESld:
	DEFW 0
CurEDel:
	DEFB 0
Ns_Base_AddToNs:
Ns_Base:
	DEFB 0
AddToNs:
	DEFB 0

AYREGS:

VT_:
	DEFS 256 ;CreatedVolumeTableAddress

DEFC EnvBase = VT_+14

DEFC T1_ = VT_+16 ;Tone tables data depacked here

DEFC T_OLD_1 = T1_
DEFC T_OLD_2 = T_OLD_1+24
DEFC T_OLD_3 = T_OLD_2+24
DEFC T_OLD_0 = T_OLD_3+2
DEFC T_NEW_0 = T_OLD_0
DEFC T_NEW_1 = T_OLD_1
DEFC T_NEW_2 = T_NEW_0+24
DEFC T_NEW_3 = T_OLD_3

NT_:
	DEFS 192 ;CreatedNoteTableAddress

;local var
DEFC Ampl = AYREGS+AmplC

DEFC VAR0END = VT_+16 ;INIT zeroes from VARS to VAR0END-1

DEFC VARSEND = ASMPC

DEFC MDLADDR = ASMPC

*/


static unsigned char ram[65536];
static short hl=0,de=0,ix=0;
unsigned char a=0;
static unsigned short sp=0xdff0;

extern void pt3_init(void);

int play_pt3(char *filename) {

	int fd,result;

	printf("Trying to play %s\n",filename);

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s: %s\n",filename,strerror(errno));

		return -1;
	}
	result=read(fd,&ram[0x1000],16384);

	printf("Read %d bytes into RAM at $1000\n",result);

	close(fd);

	hl=0x1000;

	pt3_init();


	return 0;
}




/*

;Entry and other points
;START initialization
;START+3 initialization with module address in HL
;START+5 play one quark
;START+8 mute
;START+10 setup and status flags
;START+11 pointer to current position value in PT3 module;
;After INIT (START+11) points to Postion0-1 (optimization)

PUBLIC VT_START
VT_START:

START:
	LD HL,MDLADDR
	JR INIT
	JP PLAY
	JR MUTE

;Identifier
	DEFM "=VTII PT3 Player r.7ROM="

CHECKLP:
	LD HL,SETUP
	SET 7,(HL)
	BIT 0,(HL)
	RET Z
	POP HL
	LD HL,DelyCnt
	INC (HL)
	LD HL,ChanA+CHP_NtSkCn
	INC (HL)
MUTE:
	XOR A
	LD H,A
	LD L,A
	LD (AYREGS+AmplA),A
	LD (AYREGS+AmplB),HL
	JP ROUT_A0
*/

void push(short value) {

	sp-=2;
	ram[sp]=value&0xff;
	ram[sp+1]=(value>>8)&0xff;
}

short pop(void) {
	short temp;
	temp=ram[sp] | (ram[sp+1]<<8);
	return temp;
}

void ld_E(unsigned short addr) {
	unsigned char temp_e;

	temp_e=ram[addr];

	de&=0xff00;
	de|=temp_e;
}

void ld_L(unsigned short addr) {
	unsigned char temp_l;

	temp_l=ram[addr];

	hl&=0xff00;
	hl|=temp_l;
}

void ld_H(unsigned short addr) {
	unsigned char temp_h;

	temp_h=ram[addr];

	hl&=0x00ff;
	hl|=temp_h<<8;
}

void pt3_init(void) {
	// INIT:
	// ;HL - AddressOfModule

	MODADDR=hl;		// LD (MODADDR),HL
	push(hl);		// PUSH HL
	de=100;			// LD DE,100
	hl=hl+de;		// ADD HL,DE
	a=ram[hl];		// LD A,(HL)
	Delay=a;		// LD (Delay),A
	push(hl);		// PUSH HL
	ix=pop();		// POP IX
	hl=hl+de;		// ADD HL,DE
	CrPsPtr=hl;		// LD (CrPsPtr),HL
	ld_E(ix+102-100);	// LD E,(IX+102-100)
	hl=hl+de;		// ADD HL,DE
	hl++;			// INC HL
	LPosPtr=hl;		// LD (LPosPtr),HL
	de=pop();		// POP DE
	ld_L(ix+103-100);	// LD L,(IX+103-100)
	ld_H(ix+104-100);	// LD H,(IX+104-100)
	hl=hl+de;		// ADD HL,DE
	PatsPtr=hl;		// LD (PatsPtr),HL
	hl=169;			// LD HL,169
	hl=hl+de;		// ADD HL,DE
	OrnPtrs=hl;		// LD (OrnPtrs),HL
	hl=105;			// LD HL,105
	hl=hl+de;		// ADD HL,DE
	SamPtrs=hl;		// LD (SamPtrs),HL
	hl=SETUP;		// LD HL,SETUP
	ram[hl]&=0x7f;		// RES 7,(HL)

	printf("\tSamPtrs=%04x\n",SamPtrs);
	printf("\tPatsPtr=%04x\n",PatsPtr);
	printf("\tDelay=%02x\n",Delay);

/*
;note table data depacker
	LD DE,T_PACK
	LD BC,T1_+(2*49)-1
TP_0:
	LD A,(DE)
	INC DE
	CP 15*2
	JR NC,TP_1
	LD H,A
	LD A,(DE)
	LD L,A
	INC DE
	JR TP_2
TP_1:
	PUSH DE
	LD D,0
	LD E,A
	ADD HL,DE
	ADD HL,DE
	POP DE
TP_2:
	LD A,H
	LD (BC),A
	DEC BC
	LD A,L
	LD (BC),A
	DEC BC
	SUB $F8*2
	JR NZ,TP_0

	LD HL,VAR0START
	LD (HL),A
	LD DE,VAR0START+1
	LD BC,VAR0END-VAR0START-1
	LDIR
	INC A
	LD (DelyCnt),A
	LD HL,$F001 ;H - CHP_Volume, L - CHP_NtSkCn
	LD (ChanA+CHP_NtSkCn),HL
	LD (ChanB+CHP_NtSkCn),HL
	LD (ChanC+CHP_NtSkCn),HL

	LD HL,EMPTYSAMORN
	LD (AdInPtA),HL ;ptr to zero
	LD (ChanA+CHP_OrnPtr),HL ;ornament 0 is "0,1,0"
	LD (ChanB+CHP_OrnPtr),HL ;in all versions from
	LD (ChanC+CHP_OrnPtr),HL ;3.xx to 3.6x and VTII

	LD (ChanA+CHP_SamPtr),HL ;S1 There is no default
	LD (ChanB+CHP_SamPtr),HL ;S2 sample in PT3, so, you
	LD (ChanC+CHP_SamPtr),HL ;S3 can comment S1,2,3; see
				    ;also EMPTYSAMORN comment

	LD A,(IX+13-100) ;EXTRACT VERSION NUMBER
	SUB $30
	JR C,L20
	CP 10
	JR C,L21
L20:
	LD A,6
L21:
	LD (Version),A
	PUSH AF
	CP 4
	LD A,(IX+99-100) ;TONE TABLE NUMBER
	RLA
	AND 7

;NoteTableCreator (c) Ivan Roshin
;A - NoteTableNumber*2+VersionForNoteTable
;(xx1b - 3.xx..3.4r, xx0b - 3.4x..3.6x..VTII1.0)

	LD HL,NT_DATA
	PUSH DE
	LD D,B
	ADD A,A
	LD E,A
	ADD HL,DE
	LD E,(HL)
	INC HL
	SRL E
	SBC A,A
	AND $A7 ;$00 (NOP) or $A7 (AND A)
	LD (L3),A
	LD A,201 ;RET temporary
	LD (L3+1),A ;temporary
	EX DE,HL
	POP BC ;BC=T1_
	ADD HL,BC

	LD A,(DE)

	ADD A,T_
	LD C,A
	ADC A,T_/256

	SUB C
	LD B,A
	PUSH BC
	LD DE,NT_
	PUSH DE

	LD B,12
L1:
	PUSH BC
	LD C,(HL)
	INC HL
	PUSH HL
	LD B,(HL)

	PUSH DE
	EX DE,HL
	LD DE,23
	LD IXH,8

L2:
	SRL B
	RR C
	CALL L3 ;temporary
;L3:
;	DB $19	;AND A or NOP
	LD A,C
	ADC A,D	;=ADC 0
	LD (HL),A
	INC HL
	LD A,B
	ADC A,D
	LD (HL),A
	ADD HL,DE
	DEC IXH
	JR NZ,L2

	POP DE
	INC DE
	INC DE
	POP HL
	INC HL
	POP BC
	DJNZ L1

	POP HL
	POP DE

	LD A,E
	CP TCOLD_1
	JR NZ,CORR_1
	LD A,$FD
	LD (NT_+$2E),A

CORR_1:
	LD A,(DE)
	AND A
	JR Z,TC_EXIT
	RRA
	PUSH AF
	ADD A,A
	LD C,A
	ADD HL,BC
	POP AF
	JR NC,CORR_2
	DEC (HL)
	DEC (HL)
CORR_2:
	INC (HL)
	AND A
	SBC HL,BC
	INC DE
	JR CORR_1

TC_EXIT:

	POP AF

;VolTableCreator (c) Ivan Roshin
;A - VersionForVolumeTable (0..4 - 3.xx..3.4x;
			   ;5.. - 3.5x..3.6x..VTII1.0)

	CP 5
	LD HL,$11
	LD D,H
	LD E,H
	LD A,$17
	JR NC,M1
	DEC L
	LD E,L
	XOR A
M1:
	LD (M2),A

	LD IX,VT_+16
	LD C,$10

INITV2:
	PUSH HL

	ADD HL,DE
	EX DE,HL
	SBC HL,HL

INITV1:
	LD A,L
;M2:
;	DB $7D
	CALL M2 ;temporary
	LD A,H
	ADC A,0
	LD (IX),A
	INC IX
	ADD HL,DE
	INC C
	LD A,C
	AND 15
	JR NZ,INITV1

	POP HL
	LD A,E
	CP $77
	JR NZ,M3
	INC E
M3:
	LD A,C
	AND A
	JR NZ,INITV2

	JP ROUT_A0
*/
}

/*
;pattern decoder
PD_OrSm:
	LD (IX-12+CHP_Env_En),0
	CALL SETORN
	LD A,(BC)
	INC BC
	RRCA

PD_SAM:
	ADD A,A
PD_SAM_:
	LD E,A
	LD D,0
;DEFC SamPtrs = ASMPC+1
;	LD HL,$2121
	LD HL,(SamPtrs)
	ADD HL,DE
	LD E,(HL)
	INC HL
	LD D,(HL)
;DEFC MODADDR = ASMPC+1
;	LD HL,$2121
	LD HL,(MODADDR)
	ADD HL,DE
	LD (IX-12+CHP_SamPtr),L
	LD (IX-12+CHP_SamPtr+1),H
	JR PD_LOOP

PD_VOL:
	RLCA
	RLCA
	RLCA
	RLCA
	LD (IX-12+CHP_Volume),A
	JR PD_LP2
	
PD_EOff:
	LD (IX-12+CHP_Env_En),A
	LD (IX-12+CHP_PsInOr),A
	JR PD_LP2

PD_SorE:
	DEC A
	JR NZ,PD_ENV
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_NNtSkp),A
	JR PD_LP2

PD_ENV:
	CALL SETENV
	JR PD_LP2

PD_ORN:
	CALL SETORN
	JR PD_LOOP

PD_ESAM:
	LD (IX-12+CHP_Env_En),A
	LD (IX-12+CHP_PsInOr),A
	CALL NZ,SETENV
	LD A,(BC)
	INC BC
	JR PD_SAM_

PTDECOD:
	LD A,(IX-12+CHP_Note)
;	LD (PrNote+1),A
	LD (PrNote),A
	LD L,(IX-12+CHP_CrTnSl)
	LD H,(IX-12+CHP_CrTnSl+1)
;	LD (PrSlide+1),HL
	LD (PrSlide),HL

PD_LOOP:
	LD DE,$2010
PD_LP2:
	LD A,(BC)
	INC BC
	ADD A,E
	JR C,PD_OrSm
	ADD A,D
	JR Z,PD_FIN
	JR C,PD_SAM
	ADD A,E
	JR Z,PD_REL
	JR C,PD_VOL
	ADD A,E
	JR Z,PD_EOff
	JR C,PD_SorE
	ADD A,96
	JR C,PD_NOTE
	ADD A,E
	JR C,PD_ORN
	ADD A,D
	JR C,PD_NOIS
	ADD A,E
	JR C,PD_ESAM
	ADD A,A
	LD E,A
	LD HL,SPCCOMS+$FF20-$2000
	ADD HL,DE
	LD E,(HL)
	INC HL
	LD D,(HL)
	PUSH DE
	JR PD_LOOP

PD_NOIS:
	LD (Ns_Base),A
	JR PD_LP2

PD_REL:
	RES 0,(IX-12+CHP_Flags)
	JR PD_RES
	
PD_NOTE:
	LD (IX-12+CHP_Note),A
	SET 0,(IX-12+CHP_Flags)
	XOR A

PD_RES:
;	LD (PDSP_+1),SP
	LD (PDSP_),SP
	LD SP,IX
	LD H,A
	LD L,A
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
;PDSP_:
;	LD SP,$3131
	LD SP,(PDSP_)

PD_FIN:
	LD A,(IX-12+CHP_NNtSkp)
	LD (IX-12+CHP_NtSkCn),A
	RET

C_PORTM:
	RES 2,(IX-12+CHP_Flags)
	LD A,(BC)
	INC BC
;SKIP PRECALCULATED TONE DELTA (BECAUSE
;CANNOT BE RIGHT AFTER PT3 COMPILATION)
	INC BC
	INC BC
	LD (IX-12+CHP_TnSlDl),A
	LD (IX-12+CHP_TSlCnt),A
	LD DE,NT_
	LD A,(IX-12+CHP_Note)
	LD (IX-12+CHP_SlToNt),A
	ADD A,A
	LD L,A
	LD H,0
	ADD HL,DE
	LD A,(HL)
	INC HL
	LD H,(HL)
	LD L,A
	PUSH HL
;PrNote:
;	LD A,$3E
	LD A,(PrNote)
	LD (IX-12+CHP_Note),A
	ADD A,A
	LD L,A
	LD H,0
	ADD HL,DE
	LD E,(HL)
	INC HL
	LD D,(HL)
	POP HL
	SBC HL,DE
	LD (IX-12+CHP_TnDelt),L
	LD (IX-12+CHP_TnDelt+1),H
	LD E,(IX-12+CHP_CrTnSl)
	LD D,(IX-12+CHP_CrTnSl+1)
;DEFC Version = ASMPC+1
;	LD A,$3E
	LD A,(Version)
	CP 6
	JR C,OLDPRTM ;Old 3xxx for PT v3.5-
;PrSlide:
;	LD DE,$1111
	LD DE,(PrSlide)
	LD (IX-12+CHP_CrTnSl),E
	LD (IX-12+CHP_CrTnSl+1),D
OLDPRTM:
	LD A,(BC) ;SIGNED TONE STEP
	INC BC
	EX AF,AF'
	LD A,(BC)
	INC BC
	AND A
	JR Z,NOSIG
	EX DE,HL
NOSIG:
	SBC HL,DE
	JP P,SET_STP
	CPL
	EX AF,AF'
	NEG
	EX AF,AF'
SET_STP:
	LD (IX-12+CHP_TSlStp+1),A
	EX AF,AF'
	LD (IX-12+CHP_TSlStp),A
	LD (IX-12+CHP_COnOff),0
	RET

C_GLISS:
	SET 2,(IX-12+CHP_Flags)
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_TnSlDl),A
	AND A
	JR NZ,GL36
	LD A,(Version) ;AlCo PT3.7+
	CP 7
	SBC A,A
	INC A
GL36:
	LD (IX-12+CHP_TSlCnt),A
	LD A,(BC)
	INC BC
	EX AF,AF'
	LD A,(BC)
	INC BC
	JR SET_STP

C_SMPOS:
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_PsInSm),A
	RET

C_ORPOS:
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_PsInOr),A
	RET

C_VIBRT:
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_OnOffD),A
	LD (IX-12+CHP_COnOff),A
	LD A,(BC)
	INC BC
	LD (IX-12+CHP_OffOnD),A
	XOR A
	LD (IX-12+CHP_TSlCnt),A
	LD (IX-12+CHP_CrTnSl),A
	LD (IX-12+CHP_CrTnSl+1),A
	RET

C_ENGLS:
	LD A,(BC)
	INC BC
	LD (Env_Del),A
	LD (CurEDel),A
	LD A,(BC)
	INC BC
	LD L,A
	LD A,(BC)
	INC BC
	LD H,A
	LD (ESldAdd),HL
	RET

C_DELAY:
	LD A,(BC)
	INC BC
	LD (Delay),A
	RET
	
SETENV:
	LD (IX-12+CHP_Env_En),E
	LD (AYREGS+EnvTp),A
	LD A,(BC)
	INC BC
	LD H,A
	LD A,(BC)
	INC BC
	LD L,A
	LD (EnvBase),HL
	XOR A
	LD (IX-12+CHP_PsInOr),A
	LD (CurEDel),A
	LD H,A
	LD L,A
	LD (CurESld),HL
C_NOP:
	RET

SETORN:
	ADD A,A
	LD E,A
	LD D,0
	LD (IX-12+CHP_PsInOr),D
;DEFC OrnPtrs = ASMPC+1
;	LD HL,$2121
	LD HL,(OrnPtrs)
	ADD HL,DE
	LD E,(HL)
	INC HL
	LD D,(HL)
;DEFC MDADDR2 = ASMPC+1
;	LD HL,$2121
	LD HL,(MODADDR)
	ADD HL,DE
	LD (IX-12+CHP_OrnPtr),L
	LD (IX-12+CHP_OrnPtr+1),H
	RET

;ALL 16 ADDRESSES TO PROTECT FROM BROKEN PT3 MODULES
SPCCOMS:
	DEFW C_NOP
	DEFW C_GLISS
	DEFW C_PORTM
	DEFW C_SMPOS
	DEFW C_ORPOS
	DEFW C_VIBRT
	DEFW C_NOP
	DEFW C_NOP
	DEFW C_ENGLS
	DEFW C_DELAY
	DEFW C_NOP
	DEFW C_NOP
	DEFW C_NOP
	DEFW C_NOP
	DEFW C_NOP
	DEFW C_NOP

CHREGS:
	XOR A
	LD (Ampl),A
	BIT 0,(IX+CHP_Flags)
	PUSH HL
	JP Z,CH_EXIT
;	LD (CSP_+1),SP
	LD (CSP_),SP
	LD L,(IX+CHP_OrnPtr)
	LD H,(IX+CHP_OrnPtr+1)
	LD SP,HL
	POP DE
	LD H,A
	LD A,(IX+CHP_PsInOr)
	LD L,A
	ADD HL,SP
	INC A
	CP D
	JR C,CH_ORPS
	LD A,E
CH_ORPS:
	LD (IX+CHP_PsInOr),A
	LD A,(IX+CHP_Note)
	ADD A,(HL)
	JP P,CH_NTP
	XOR A
CH_NTP:
	CP 96
	JR C,CH_NOK
	LD A,95
CH_NOK:
	ADD A,A
	EX AF,AF'
	LD L,(IX+CHP_SamPtr)
	LD H,(IX+CHP_SamPtr+1)
	LD SP,HL
	POP DE
	LD H,0
	LD A,(IX+CHP_PsInSm)
	LD B,A
	ADD A,A
	ADD A,A
	LD L,A
	ADD HL,SP
	LD SP,HL
	LD A,B
	INC A
	CP D
	JR C,CH_SMPS
	LD A,E
CH_SMPS:
	LD (IX+CHP_PsInSm),A
	POP BC
	POP HL
	LD E,(IX+CHP_TnAcc)
	LD D,(IX+CHP_TnAcc+1)
	ADD HL,DE
	BIT 6,B
	JR Z,CH_NOAC
	LD (IX+CHP_TnAcc),L
	LD (IX+CHP_TnAcc+1),H
CH_NOAC:
	EX DE,HL
	EX AF,AF'
	LD L,A
	LD H,0
	LD SP,NT_
	ADD HL,SP
	LD SP,HL
	POP HL
	ADD HL,DE
	LD E,(IX+CHP_CrTnSl)
	LD D,(IX+CHP_CrTnSl+1)
	ADD HL,DE
;CSP_:
;	LD SP,$3131
	LD SP,(CSP_)
	EX (SP),HL
	XOR A
	OR (IX+CHP_TSlCnt)
	JR Z,CH_AMP
	DEC (IX+CHP_TSlCnt)
	JR NZ,CH_AMP
	LD A,(IX+CHP_TnSlDl)
	LD (IX+CHP_TSlCnt),A
	LD L,(IX+CHP_TSlStp)
	LD H,(IX+CHP_TSlStp+1)
	LD A,H
	ADD HL,DE
	LD (IX+CHP_CrTnSl),L
	LD (IX+CHP_CrTnSl+1),H
	BIT 2,(IX+CHP_Flags)
	JR NZ,CH_AMP
	LD E,(IX+CHP_TnDelt)
	LD D,(IX+CHP_TnDelt+1)
	AND A
	JR Z,CH_STPP
	EX DE,HL
CH_STPP:
	SBC HL,DE
	JP M,CH_AMP
	LD A,(IX+CHP_SlToNt)
	LD (IX+CHP_Note),A
	XOR A
	LD (IX+CHP_TSlCnt),A
	LD (IX+CHP_CrTnSl),A
	LD (IX+CHP_CrTnSl+1),A
CH_AMP:
	LD A,(IX+CHP_CrAmSl)
	BIT 7,C
	JR Z,CH_NOAM
	BIT 6,C
	JR Z,CH_AMIN
	CP 15
	JR Z,CH_NOAM
	INC A
	JR CH_SVAM
CH_AMIN:
	CP -15
	JR Z,CH_NOAM
	DEC A
CH_SVAM:
	LD (IX+CHP_CrAmSl),A
CH_NOAM:
	LD L,A
	LD A,B
	AND 15
	ADD A,L
	JP P,CH_APOS
	XOR A
CH_APOS:
	CP 16
	JR C,CH_VOL
	LD A,15
CH_VOL:
	OR (IX+CHP_Volume)
	LD L,A
	LD H,0
	LD DE,VT_
	ADD HL,DE
	LD A,(HL)
CH_ENV:
	BIT 0,C
	JR NZ,CH_NOEN
	OR (IX+CHP_Env_En)
CH_NOEN:
	LD (Ampl),A
	BIT 7,B
	LD A,C
	JR Z,NO_ENSL
	RLA
	RLA
	SRA A
	SRA A
	SRA A
	ADD A,(IX+CHP_CrEnSl) ;SEE COMMENT BELOW
	BIT 5,B
	JR Z,NO_ENAC
	LD (IX+CHP_CrEnSl),A
NO_ENAC:
	LD HL,AddToEn
	ADD A,(HL) ;BUG IN PT3 - NEED WORD HERE.
		   ;FIX IT IN NEXT VERSION?
	LD (HL),A
	JR CH_MIX
NO_ENSL:
	RRA
	ADD A,(IX+CHP_CrNsSl)
	LD (AddToNs),A
	BIT 5,B
	JR Z,CH_MIX
	LD (IX+CHP_CrNsSl),A
CH_MIX:
	LD A,B
	RRA
	AND $48
CH_EXIT:
	LD HL,AYREGS+Mixer
	OR (HL)
	RRCA
	LD (HL),A
	POP HL
	XOR A
	OR (IX+CHP_COnOff)
	RET Z
	DEC (IX+CHP_COnOff)
	RET NZ
	XOR (IX+CHP_Flags)
	LD (IX+CHP_Flags),A
	RRA
	LD A,(IX+CHP_OnOffD)
	JR C,CH_ONDL
	LD A,(IX+CHP_OffOnD)
CH_ONDL:
	LD (IX+CHP_COnOff),A
	RET

PLAY:
	XOR A
	LD (AddToEn),A
	LD (AYREGS+Mixer),A
	DEC A
	LD (AYREGS+EnvTp),A
	LD HL,DelyCnt
	DEC (HL)
	JP NZ,PL2
	LD HL,ChanA+CHP_NtSkCn
	DEC (HL)
	JR NZ,PL1B
;DEFC AdInPtA = ASMPC+1
;	LD BC,$0101
	LD BC,(AdInPtA)
	LD A,(BC)
	AND A
	JR NZ,PL1A
	LD D,A
	LD (Ns_Base),A
	LD HL,(CrPsPtr)
	INC HL
	LD A,(HL)
	INC A
	JR NZ,PLNLP
	CALL CHECKLP
;DEFC LPosPtr = ASMPC+1
;	LD HL,$2121
	LD HL,(LPosPtr)
	LD A,(HL)
	INC A
PLNLP:
	LD (CrPsPtr),HL
	DEC A
	ADD A,A
	LD E,A
	RL D
;DEFC PatsPtr = ASMPC+1
;	LD HL,$2121
	LD HL,(PatsPtr)
	ADD HL,DE
	LD DE,(MODADDR)
;	LD (PSP_+1),SP
	LD (PSP_),SP
	LD SP,HL
	POP HL
	ADD HL,DE
	LD B,H
	LD C,L
	POP HL
	ADD HL,DE
	LD (AdInPtB),HL
	POP HL
	ADD HL,DE
	LD (AdInPtC),HL
;PSP_:
;	LD SP,$3131
	LD SP,(PSP_)
PL1A:
	LD IX,ChanA+12
	CALL PTDECOD
	LD (AdInPtA),BC

PL1B:
	LD HL,ChanB+CHP_NtSkCn
	DEC (HL)
	JR NZ,PL1C
	LD IX,ChanB+12
;DEFC AdInPtB = ASMPC+1
;	LD BC,$0101
	LD BC,(AdInPtB)
	CALL PTDECOD
	LD (AdInPtB),BC

PL1C:
	LD HL,ChanC+CHP_NtSkCn
	DEC (HL)
	JR NZ,PL1D
	LD IX,ChanC+12
;DEFC AdInPtC = ASMPC+1
;	LD BC,$0101
	LD BC,(AdInPtC)
	CALL PTDECOD
	LD (AdInPtC),BC

;DEFC Delay = ASMPC+1
PL1D:
;	LD A,$3E
	LD A,(Delay)
	LD (DelyCnt),A

PL2:
	LD IX,ChanA
	LD HL,(AYREGS+TonA)
	CALL CHREGS
	LD (AYREGS+TonA),HL
	LD A,(Ampl)
	LD (AYREGS+AmplA),A
	LD IX,ChanB
	LD HL,(AYREGS+TonB)
	CALL CHREGS
	LD (AYREGS+TonB),HL
	LD A,(Ampl)
	LD (AYREGS+AmplB),A
	LD IX,ChanC
	LD HL,(AYREGS+TonC)
	CALL CHREGS
;	LD A,(Ampl) ;Ampl = AYREGS+AmplC
;	LD (AYREGS+AmplC),A
	LD (AYREGS+TonC),HL

	LD HL,(Ns_Base_AddToNs)
	LD A,H
	ADD A,L
	LD (AYREGS+Noise),A

;DEFC AddToEn = ASMPC+1
;	LD A,$3E
	LD A,(AddToEn)
	LD E,A
	ADD A,A
	SBC A,A
	LD D,A
	LD HL,(EnvBase)
	ADD HL,DE
	LD DE,(CurESld)
	ADD HL,DE
	LD (AYREGS+Env),HL

	XOR A
	LD HL,CurEDel
	OR (HL)
	JR Z,ROUT_A0
	DEC (HL)
	JR NZ,ROUT
;DEFC Env_Del = ASMPC+1
;	LD A,$3E
	LD A,(Env_Del)
	LD (HL),A
;DEFC ESldAdd = ASMPC+1
;	LD HL,$2121
	LD HL,(ESldAdd)
	ADD HL,DE
	LD (CurESld),HL

ROUT:
	XOR A
ROUT_A0:
	LD DE,$FFBF
	LD BC,$FFFD
	LD HL,AYREGS
LOUT:
	OUT (C),A
	LD B,E
	OUTI 
	LD B,D
	INC A
	CP 13
	JR NZ,LOUT
	OUT (C),A
	LD A,(HL)
	AND A
	RET M
	LD B,E
	OUT (C),A
	RET

NT_DATA:
	DEFB (T_NEW_0-T1_)*2
	DEFB TCNEW_0-T_
	DEFB (T_OLD_0-T1_)*2+1
	DEFB TCOLD_0-T_
	DEFB (T_NEW_1-T1_)*2+1
	DEFB TCNEW_1-T_
	DEFB (T_OLD_1-T1_)*2+1
	DEFB TCOLD_1-T_
	DEFB (T_NEW_2-T1_)*2
	DEFB TCNEW_2-T_
	DEFB (T_OLD_2-T1_)*2
	DEFB TCOLD_2-T_
	DEFB (T_NEW_3-T1_)*2
	DEFB TCNEW_3-T_
	DEFB (T_OLD_3-T1_)*2
	DEFB TCOLD_3-T_

T_:

TCOLD_0:
	DEFB $00+1,$04+1,$08+1,$0A+1,$0C+1,$0E+1,$12+1,$14+1
	DEFB $18+1,$24+1,$3C+1,0
TCOLD_1:
	DEFB $5C+1,0
TCOLD_2:
	DEFB $30+1,$36+1,$4C+1,$52+1,$5E+1,$70+1,$82,$8C,$9C
	DEFB $9E,$A0,$A6,$A8,$AA,$AC,$AE,$AE,0
TCNEW_3:
	DEFB $56+1
TCOLD_3:
	DEFB $1E+1,$22+1,$24+1,$28+1,$2C+1,$2E+1,$32+1,$BE+1,0
TCNEW_0:
	DEFB $1C+1,$20+1,$22+1,$26+1,$2A+1,$2C+1,$30+1,$54+1
	DEFB $BC+1,$BE+1,0
DEFC TCNEW_1 = TCOLD_1
TCNEW_2:
	DEFB $1A+1,$20+1,$24+1,$28+1,$2A+1,$3A+1,$4C+1,$5E+1
	DEFB $BA+1,$BC+1,$BE+1,0

DEFC EMPTYSAMORN = ASMPC-1
	DEFB 1,0,$90 ;delete $90 if you don't need default sample

;first 12 values of tone tables (packed)

T_PACK:
	DEFB $06EC*2/256,$06EC*2
	DEFB $0755-$06EC
	DEFB $07C5-$0755
	DEFB $083B-$07C5
	DEFB $08B8-$083B
	DEFB $093D-$08B8
	DEFB $09CA-$093D
	DEFB $0A5F-$09CA
	DEFB $0AFC-$0A5F
	DEFB $0BA4-$0AFC
	DEFB $0C55-$0BA4
	DEFB $0D10-$0C55
	DEFB $066D*2/256,$066D*2
	DEFB $06CF-$066D
	DEFB $0737-$06CF
	DEFB $07A4-$0737
	DEFB $0819-$07A4
	DEFB $0894-$0819
	DEFB $0917-$0894
	DEFB $09A1-$0917
	DEFB $0A33-$09A1
	DEFB $0ACF-$0A33
	DEFB $0B73-$0ACF
	DEFB $0C22-$0B73
	DEFB $0CDA-$0C22
	DEFB $0704*2/256,$0704*2
	DEFB $076E-$0704
	DEFB $07E0-$076E
	DEFB $0858-$07E0
	DEFB $08D6-$0858
	DEFB $095C-$08D6
	DEFB $09EC-$095C
	DEFB $0A82-$09EC
	DEFB $0B22-$0A82
	DEFB $0BCC-$0B22
	DEFB $0C80-$0BCC
	DEFB $0D3E-$0C80
	DEFB $07E0*2/256,$07E0*2
	DEFB $0858-$07E0
	DEFB $08E0-$0858
	DEFB $0960-$08E0
	DEFB $09F0-$0960
	DEFB $0A88-$09F0
	DEFB $0B28-$0A88
	DEFB $0BD8-$0B28
	DEFB $0C80-$0BD8
	DEFB $0D60-$0C80
	DEFB $0E10-$0D60
	DEFB $0EF8-$0E10

*/
