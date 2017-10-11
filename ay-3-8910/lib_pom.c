/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software posted to USENET.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



static short led_art[16][8]={
        {	/* 0 New */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        0x0000,         /*                */
        },
        {	/* 1 Waxing Crescent */
        0x00c0,         /*        **      */
        0x0060,         /*         **     */
        0x0030,         /*          **    */
        0x0030,         /*          **    */
        0x0030,         /*          **    */
        0x0030,         /*          **    */
        0x0060,         /*         **     */
        0x00c0,         /*        **      */
        },
        {	/* 2 First Quarter */
        0x00c0,         /*        **      */
        0x00e0,         /*        ***     */
        0x00f0,         /*        ****    */
        0x00f0,         /*        ****    */
        0x00f0,         /*        ****    */
        0x00f0,         /*        ****    */
        0x00e0,         /*        ***     */
        0x00c0,         /*        **      */
        },
        {	/* 3 Waxing Gibbous */
        0x00c0,         /*        **      */
        0x01e0,         /*       ****     */
        0x03f0,         /*      ******    */
        0x03f0,         /*      ******    */
        0x03f0,         /*      ******    */
        0x03f0,         /*      ******    */
        0x01e0,         /*       ****     */
        0x00c0,         /*        **      */
        },
        {	/* 4 Full */
        0x03c0,         /*      ****      */
        0x07e0,         /*     ******     */
        0x0ff0,         /*    ********    */
        0x0ff0,         /*    ********    */
        0x0ff0,         /*    ********    */
        0x0ff0,         /*    ********    */
        0x07e0,         /*     ******     */
        0x03c0,         /*      ****      */
        },
        {	/* 5 Waning Gibbous */
        0x0300,         /*      **        */
        0x0780,         /*     ****       */
        0x0fc0,         /*    ******      */
        0x0fc0,         /*    ******      */
        0x0fc0,         /*    ******      */
        0x0fc0,         /*    ******      */
        0x0780,         /*     ****       */
        0x0300,         /*      **        */
        },
        {	/* 6 Third Quarter */
        0x0300,         /*      **        */
        0x0700,         /*     ***        */
        0x0f00,         /*    ****        */
        0x0f00,         /*    ****        */
        0x0f00,         /*    ****        */
        0x0f00,         /*    ****        */
        0x0700,         /*     ***        */
        0x0300,         /*      **        */
        },
        {	/* 7 Waning Crescent */
        0x0300,         /*      **        */
        0x0600,         /*     **         */
        0x0c00,         /*    **          */
        0x0c00,         /*    **          */
        0x0c00,         /*    **          */
        0x0c00,         /*    **          */
        0x0600,         /*     **         */
        0x0300,         /*      **        */
        },
};


/*
 * Phase of the Moon.  Calculates the current phase of the moon.
 * Based on routines from `Practical Astronomy with Your Calculator',
 * by Duffett-Smith.  Comments give the section from the book that
 * particular piece of code was adapted from.
 *
 * -- Keith E. Brandt  VIII 1984
 *
 * Updated to the Third Edition of Duffett-Smith's book, Paul Janzen, IX 1998
 *
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "stats.h"
#include "display.h"

#include "lib_lib.h"

#define	PI	  3.14159265358979323846


/*
 * The EPOCH in the third edition of the book is 1990 Jan 0.0 TDT.
 * In this program, we do not bother to correct for the differences
 * between UTC (as shown by the UNIX clock) and TDT.  (TDT = TAI + 32.184s;
 * TAI-UTC = 32s in Jan 1999.)
 */
#define EPOCH_MINUS_1970	(20 * 365 + 5 - 1) /* 20 years, 5 leaps, back 1 day to Jan 0 */
#define	EPSILONg  279.403303	/* solar ecliptic long at EPOCH */
#define	RHOg	  282.768422	/* solar ecliptic long of perigee at EPOCH */
#define	ECCEN	  0.016713	/* solar orbit eccentricity */
#define	lzero	  318.351648	/* lunar mean long at EPOCH */
#define	Pzero	  36.340410	/* lunar mean long of perigee at EPOCH */
#define	Nzero	  318.510107	/* lunar mean long of node at EPOCH */

/* dtor -- convert degrees to radians */
static double dtor(double deg) {
	return(deg * PI / 180);
}



/* adj360 -- adjust value so 0 <= deg <= 360 */
static void adj360(double *deg) {

	for (;;)
		if (*deg < 0)
			*deg += 360;
		else if (*deg > 360)
			*deg -= 360;
		else
			break;
}


/* potm -- return phase of the moon */
static double potm(double days) {

	double N, Msol, Ec, LambdaSol, l, Mm, Ev, Ac, A3, Mmprime;
	double A4, lprime, V, ldprime, D, Nm;

	N = 360 * days / 365.242191;				/* sec 46 #3 */
	adj360(&N);
	Msol = N + EPSILONg - RHOg;				/* sec 46 #4 */
	adj360(&Msol);
	Ec = 360 / PI * ECCEN * sin(dtor(Msol));		/* sec 46 #5 */
	LambdaSol = N + Ec + EPSILONg;				/* sec 46 #6 */
	adj360(&LambdaSol);
	l = 13.1763966 * days + lzero;				/* sec 65 #4 */
	adj360(&l);
	Mm = l - (0.1114041 * days) - Pzero;			/* sec 65 #5 */
	adj360(&Mm);
	Nm = Nzero - (0.0529539 * days);			/* sec 65 #6 */
	adj360(&Nm);
	Ev = 1.2739 * sin(dtor(2*(l - LambdaSol) - Mm));	/* sec 65 #7 */
	Ac = 0.1858 * sin(dtor(Msol));				/* sec 65 #8 */
	A3 = 0.37 * sin(dtor(Msol));
	Mmprime = Mm + Ev - Ac - A3;				/* sec 65 #9 */
	Ec = 6.2886 * sin(dtor(Mmprime));			/* sec 65 #10 */
	A4 = 0.214 * sin(dtor(2 * Mmprime));			/* sec 65 #11 */
	lprime = l + Ev + Ec - Ac + A4;				/* sec 65 #12 */
	V = 0.6583 * sin(dtor(2 * (lprime - LambdaSol)));	/* sec 65 #13 */
	ldprime = lprime + V;					/* sec 65 #14 */
	D = ldprime - LambdaSol;				/* sec 67 #2 */
	return(50.0 * (1 - cos(dtor(D))));			/* sec 67 #3 */
}

int lib_pom(void) {

	time_t tmpt, now;
	double days, today, tomorrow;
	int which,waxing=0,percent;
	char phase_string[100];

	int scroll=0,scroll_dir=1;
	char display_string[13];

	int ch;

	if (time(&now) == (time_t)-1) {
		fprintf(stderr,"Error getting time\n");
	}

	tmpt = now;

	days = (tmpt - EPOCH_MINUS_1970 * 86400) / 86400.0;
	today = potm(days) + .5;

	percent=(int)today;

	if (percent == 100) {
		sprintf(phase_string,"Full");
		which=4;
	}
	else if (percent==0) {
		sprintf(phase_string,"New");
		which=0;
	}
	else {
		tomorrow = potm(days + 1);

		if (tomorrow>today) waxing=1;

		if (percent == 50) {
			if (waxing) {
				sprintf(phase_string,"First Quarter");
				which=2;
			}
			else {
				sprintf(phase_string,"Last Quarter");
				which=6;
			}
		}
		else {
			today -= 0.5;
			if (tomorrow>today) waxing=1;

			if (waxing) {
				if (today>50) {
					sprintf(phase_string,"Waxing Gibbous");
					which=3;
				}
				else {
					sprintf(phase_string,"Waxing Crescent");
					which=1;
				}
			}
			else {
				if (today>50) {
					sprintf(phase_string,"Waning Gibbous");
					which=5;
				}
				else {
					sprintf(phase_string,"Waning Crescent");
					which=7;
				}
			}

		}
	}

	display_8x16_led_art(display_type,led_art,which);

	while(1) {

		if (strlen(phase_string)>12) {
			snprintf(display_string,13,
				"%s",phase_string+scroll);
			display_14seg_string(display_type,display_string);

			scroll=scroll+scroll_dir;

			if (scroll<0) {
				scroll=0;
				scroll_dir=-scroll_dir;
				usleep(200000);
			}

			if (scroll>(strlen(phase_string)-12)) {
				scroll=strlen(phase_string)-12;
				scroll_dir=-scroll_dir;
				usleep(200000);
			}

//			printf("%d\n",scroll);

		}
		else {
			snprintf(display_string,13,"%s          ",phase_string);
			display_14seg_string(display_type,display_string);
		}

		ch=display_keypad_read(display_type);
		if ((ch==CMD_EXIT_PROGRAM) || (ch==CMD_CANCEL)) {
			break;
		}

//		printf("%s\n",phase_string);
		usleep(200000);
	}

	return 0;
}

