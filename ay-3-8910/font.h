//1 1 11 11 1   1
//1 1 1 1 1 1 1 1
// 1  1   1 11 11

//111 1 1 111 111 111 1 1 11  111
//1   111  1  111  1  1 1 1 1 11
//111 1 1 111 1    1  111 1 1 111

unsigned char fbs_font[128][8]={
{ /* 0x00 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x01 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x02 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x03 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x04 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x05 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x06 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x07 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x08 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x09 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0a */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0b */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0c */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0d */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0e */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x0f */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x10 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x11 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x12 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x13 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x14 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x15 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x16 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x17 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x18 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x19 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x20 ' ' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x21 '!' */
0x0d,	// 11
0x0d,	// 11
0x0d,	// 11
0x0d,	// 11
0x0d,	// 11
0x0,	//
0x0d,	// 11
0x0,	//
},
{ /* 0x22 '"' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x23 '#' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x24 '$' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x25 '%'*/
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x26 '&' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x27 ''' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x28 '('*/
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x29 ')'*/
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2a '*" */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2b '+' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2c ',' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2d '-' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2e '.' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x2f '/' */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x30 0 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x31 1 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x32 2 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x33 3 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x34 4 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x35 5 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x36 6 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x37 7 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x38 8 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x39 9 */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3a : */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3b ; */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3c < */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3d = */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3e > */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x3f ? */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x40 @ */
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
0x0,
},
{ /* 0x41 A */
0x0e,	//  111
0x11,	// 1   1
0x11,	// 1   1
0x1f,	// 11111
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x0,
},
{ /* 0x42 B */
0x1e,	// 1111
0x11,	// 1   1
0x11,	// 1   1
0x1e,	// 1111
0x11,	// 1   1
0x11,	// 1   1
0x1e,	// 1111
0x00,
},
{ /* 0x43 C */
0x0f,	//  1111
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x0f,	//  1111
},
{ /* 0x44 D */
0x1c,	// 111
0x12,	// 1  1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x12,	// 1  1
0x1c,	// 111
0x00,
},
{ /* 0x45 E */
0x1f,	// 11111
0x10,	// 1
0x10,	// 1
0x1e,	// 1111
0x10,	// 1
0x10,	// 1
0x1f,	// 11111
},
{ /* 0x46 F */
0x1f,	// 11111
0x10,	// 1
0x10,	// 1
0x1e,	// 1111
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x00,
},
{ /* 0x47 G */
0x0e,	//  111
0x11,	// 1   1
0x10,	// 1
0x13,	// 1  11
0x11,	// 1   1
0x11,	// 1   1
0x0e,	//  111
},
{ /* 0x48 H */
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x1f,	// 11111
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
},
{ /* 0x49 I */
0x1f,	// 11111
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x1f,	// 11111
0x00,
},
{ /* 0x4a J */
0x1f,	// 11111
0x02,	//    1
0x02,	//    1
0x02,	//    1
0x02,	//    1
0x12,	// 1  1
0x0c,	//  11
0x00,
},
{ /* 0x4b K */
0x11,	// 1   1
0x12,	// 1  1
0x14,	// 1 1
0x18,	// 11
0x14,	// 1 1
0x12,	// 1  1
0x11,	// 1   1
0x00,
},
{ /* 0x4c L */
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x1f,	// 11111
0x00,
},
{ /* 0x4d M */
0x11,	// 1   1
0x1b,	// 11 11
0x15,	// 1 1 1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x00,
},
{ /* 0x4e N */
0x11,	// 1   1
0x1b,	// 11  1
0x15,	// 1 1 1
0x13,	// 1  11
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x00,
},
{ /* 0x4f O */
0x0e,	//  111
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x0e,	//  111
0x00,
},
{ /* 0x50 P */
0x1e,	// 1111
0x11,	// 1   1
0x11,	// 1   1
0x1e,	// 1111
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x00,
},
{ /* 0x51 Q */
0x0e,	//  111
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x15,	// 1 1 1
0x0e,	//  111
0x01,   //     1
},
{ /* 0x52 R */
0x1e,	// 1111
0x11,	// 1   1
0x11,	// 1   1
0x1e,	// 1111
0x14,	// 1 1
0x12,	// 1  1
0x11,	// 1   1
0x00,
},
{ /* 0x53 S */
0x0f,	//  1111
0x10,	// 1
0x10,	// 1
0x0e,	//  111
0x01,	//     1
0x01,	//     1
0x1e,	// 1111
0x00,
},
{ /* 0x54 T */
0x1f,	// 11111
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x04,	//   1
0x00,
},
{ /* 0x55 U */
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x0e,	//  111
0x00,
},
{ /* 0x56 V */
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x0a,	//  1 1
0x0a,	//  1 1
0x0e,	//  111
0x00,
},
{ /* 0x57 W */
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x11,	// 1   1
0x15,	// 1 1 1
0x1b,	// 11 11
0x11,	// 1   1
0x00,
},
{ /* 0x58 X */
0x11,	// 1   1
0x11,	// 1   1
0x0a,	//  1 1
0x08,	//   1
0x0a,	//  1 1
0x11,	// 1   1
0x11,	// 1   1
0x00,
},
{ /* 0x59 Y */
0x11,	// 1   1
0x11,	// 1   1
0x0a,	//  1 1
0x04,	//   1
0x04,	//   1
0x14,	//   1
0x14,	//   1
0x00,
},
{ /* 0x5a Z */
0x1f,	// 11111
0x01,	//     1
0x02,	//    1
0x04,	//   1
0x08,	//  1
0x10,	// 1
0x1f,	// 11111
0x00,
},
{ /* 0x5b [ */
0x1f,	// 11111
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x10,	// 1
0x1f,	// 11111
0x00,
},
{ /* 0x5c \ */
0x10,	// 1
0x10,	// 1
0x08,	//  1
0x04,	//   1
0x02,	//    1
0x01,	//     1
0x01,	//     1
0x00,
},
{ /* 0x5d ] */
0x1f,	// 11111
0x01,	//     1
0x01,	//     1
0x01,	//     1
0x01,	//     1
0x01,	//     1
0x1f,	// 11111
0x00,
},
{ /* 0x5e ^ */
0x04,	//   1
0x0a,	//  1 1
0x11,	// 1   1
0x00,	//
0x00,	//
0x00,	//
0x00,	//
0x00,
},
{ /* 0x5f _ */
0x00,	//
0x00,	//
0x00,	//
0x00,	//
0x00,	//
0x00,	//
0x1f,	// 11111
0x00,
},
};
