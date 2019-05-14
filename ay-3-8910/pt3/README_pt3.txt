How to decode a Vortex Tracker II "PT3" File


Background:
~~~~~~~~~~~
	Vortex Tracker II ( https://bulba.untergrund.net/vortex_e.htm )
	is a music tracker (tool for writing music) that targets systems
	with the AY-3-8910 sound chip.  The music made is most popular
	on various ZX Spectrum (z80) and Atari ST systems.

	I wanted to make a decoder for a 6502-based Apple II with a
	AY-3-8910 based Mockingboard sound card.  The challenge was
	all the low-level Vortex Tracker documentation is in Russian,
	and the source code available is either uncommented z80 assembly
	language or Russian-commented Pascal source code.

	This document is based at least partly on the writeup here:
		http://karoshi.auic.es/index.php?topic=397.msg4641#msg4641
	as well as the AY_emul source code, and lastly just from reverse
	engineering things while trying to get my code to match the output
	of AY_emul exactly.


The PT3 Format
~~~~~~~~~~~~~~


* File Header

	Note: 16-bit values are little-endian

	Offset      Size       Description    Contents
	$00 - $0C : 13 bytes : Magic        : "ProTracker 3."
	$0D       :  1 byte  : Version      : '5' for Vortex Tracker II
	$0E - $1D : 16 bytes : String       : " compilation of "
	$1E - $3E : 32 bytes : Name         : Name of the module
	$3E - $41 :  4 bytes : String       : " by "
	$42 - $62 : 32 bytes : Author       : Author of the module.
	$63       :  1 byte  : Frequency table (from 1 to 4)
	$64       :  1 byte  : Speed/Delay
	$65       :  1 byte  : Number of patterns+1  (Max Patterns)
	$66       :  1 byte  : LPosPtr      : Pattern Loop Pointer
	$67 - $68 :  2 bytes : PatsPtrs     : Pointers to the patterns
	$69 - $A8 : 64 bytes : SamPtrs[32]  : Pointers to the samples
	$A9 - $C8 : 32 bytes : OrnPtrs[16]  : Pointers to the ornaments
	$C9 - ??? :          : Patterns[]   : $FF terminated, More on this below

	Version: is at offset 13, which is just the ASCII value of
		the version number from the magic.  Usually you
		subtract off '0' to get the proper value.

* Pattern list

	The pattern list starts at $C9 and is terminated by $FF

	The pattern is multiplied by 3.

	A sample pattern list might look like
		$03, $06, $09, $FF
	which corresponds to playing in order patterns
		1, 2, 3


* Samples

	The pt3 file allows for 32 samples.  These samples contain
	values that are applied to the music notes as they are playing.

	The sample pointer will point to this 

	// Sample format

	// XX YYYYY Z  = X= 10=VOLDOWN 11=VOLUP, Y=NOISE, Z= 0=ENV, 1=NO ENVELO$
        // XX YY ZZZZ  = X= FREQ SLIDE YY=NOISE SLIDE ZZ=VOLUME
        // XXXXXXXX = LOW BYTE FREQ SLIDE
        // YYYYYYYY = HIGH BYTE FREQ SLIDE
        //
        // 80 8f 00 00
        // 1000 0000 -- VOLDOWN
        // 10 00 1111 --FREQ_SLIDE, VOLUME

* Ornaments

	The PT3 file format has 16 ornaments, which are patterns applied
	to the note.  This can be used for ?? effects.

	The 16-bit address of an ornament X can be found by getting the
		16-bit little-endian address at offset
		$A9+(X*2) in the header.

	Byte 0:	 LOOP VALUE -- ornament offset to return to once hit the end
	Byte 1:  LENGTH     -- number of ornament values
	Byte 2+: VALUES     -- list of byte values applied to the notes


* Pattern data

// Actual Pattern Data, Stream of bytes, null terminated
//      $00           -- nul teminate
//      $01-$0f       -- effects
// OnDisk  InTracker
//   $01     $01: Tone Down
//                First byte indicates the delay used to add the new frequency.
//                Next 2 bytes will indicate the frequency to add. Example:
//                $02,$23,$00 will add $23 to the final frequency in that raster
//                and another $23 every 2 rasters.
//   $01     $02: Tone Up
//                It's the same as above but the value is rested to $FFFF. Example:
//                $01,$DD,$FF will rest $23 to the final frequency in every raster.
//   $02     $03: Tone portamento
//   $03     $04: Sample Offset
//		  Starts playing sample from a particular position/line definition.
//                Byte indicates from which position.
//   $04     $05: Ornament offset
//		  Starts playing ornament from a particular position/line definition.
//                Byte indicates from which position.
//   $05     $06: Vibrato
//		  Periodic sound off/on in that channel
//                Two bytes are used here. The first one tells how many rasters will be
//                played and the second how many will be not. Example. $03,$04 means
//                that that channel will be played 3 rasters, 4 not played, 3 played, ...
//   $08     $09: Envelope frequency decreasing.
//   $08     $0A: Envelope frequency increasing.
//                Diffrence from previous is it has a negative value?
//   $09     $0B: Set playing speed (new Delay).
//                The byte after the note will tell the new delay.
//
//
//
//
//      $10-$1f       -- envelope.   (1-E, F=envelope off)
//	$20-$3f       -- set noise
//      $40-$4f       -- set ornament 0-F
//		Ornament 0 can't be set directly, instead $40
//		is reported as envelope off (F)?
//      $50-$ad       -- play note, see below
//	$b0           -- env=$f, ornament=saved ornament
//	$b1, arg1     -- set skip value to arg1 (how long note plays)
//	$b2-$bf,arg1/2-- envelope?
//      $c0-$cf       -- set volume, value-$c0.  $c0 means sound off
//	$d0           -- do nothing?
//      $d1-$ef       -- set sample, value-$d0.
//	$f0-$ff, arg1 -- Initialize?
//               Envelope=15, Ornament=low byte, Sample=arg1/2
-
// if you reach a note, a 0xd0, a 0xc0 then done this note.
-
// f0 10 cf b1 40 74 00
-
-
// 50-AF = notes
//      0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
//50: C-1 C#1 D-1 D#1 E-1 F-1 F#1 G-1 G#1 A-1 A#1 B-1 C-2 C#2 D-2 D#2
//60: E-2 F-2 F#2 G-2 G#2 A-2 A#2 B-2 C-3 C#3 D-3 D#3 E-3 F-3 F#3 G-3
//70: G#3 A-3 A#3 B-3 C-4 C#4 D-4 D#4 E-4 F-2 F#4 G-4 G#4 A-4 A#4 B-4
//80: C-5 C#5 D-5 D#5 E-5 F-5 F#5 G-5 G#5 A-5 A#5 B-5 C-6 C#6 D-6 D#6
//90: E-6 F-6 F#6 G-6 G#6 A-6 A#6 B-6 C-7 C#7 D-7 D#7 E-7 F-7 F#7 G-7
//a0: G#7 A-7 A#7 B-7 C-8 C#8 D-8 D#8 E-8 F-8 F#8 G-8 G#8 A-8 A#8 B-8


* Frequency Tables

	You can select between various frequency tables.
	I'm not sure why?

* Amplitude Tables
	There are multiple 256-byte amplitude/volume lookup tables
	These seem to have changed with different versions of Vortex Tracker
	so to be complete you need to contain them all and use the proper
	one at run time
