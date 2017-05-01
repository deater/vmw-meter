/* This is a two-track format to play on the simple 1-bit speaker */
/* Interface available on the Apple II */

/* https://sourceforge.net/p/ed2midi/code/ */
/* From a spreadsheet from ed2midi */
/*	Octave	1	2	3	4	5
	A	255	128	64	32	16
	A#	240	120	60	30	15
	B	228	114	57	28	14
	C	216	108	54	27	13
	C#	204	102	51	25	12
	D	192	 96	48	24	12
	D#	180	 90	45	22	11
	E	172	 86	43	21	10
	F	160	 80	40	20	10
	F#	152	 76	38	19	9
	G	144	 72	36	18	9
	G#	136	 68	34	17	8
*/

/* ed file format */
/* from about.md */
/* First byte 0:	Voice */
/*		byte1 = voice1 instrument */
/*		byte2 = voice2 instrument */
/*		Varies, bigger than 8 seem to make no difference */

/* Otherwise,	byte0 = duration (20=quarter, 40=half) */
/* 		byte1 = voice1 note */
/*		byte2 = voice2 note */

