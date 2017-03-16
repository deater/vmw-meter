/* I did this by hand                                    */
/* then touched up some of the lousy looking ones        */
/*   based on ideas from the MAX6954 chip docs           */
/*   as well as from                                     */
/* http://www.robotroom.com/MultiSegmentLEDDisplay3.html */
/* This is probably still sub-optimal.  I want each char */
/*   to be distinct, so that when looking at something   */
/*   like a code listing you can tell the difference     */
/*   between C and [, as well as 1 : l ! |               */

/* Lower case looks pretty horrible, updated with some */
/* ideas found online: */
/* http://hwm3.gyao.ne.jp/shiroi-niwatori/nishiki-teki.htm */
/* Still don't like a and i, and p and q are problematic too */
/* Semicolon remains a problem, as well as period */



short ascii_lookup[256]={

  /* 0x00 */ 0,
  /* 0x01 */ 0,
  /* 0x02 */ 0,
  /* 0x03 */ 0,
  /* 0x04 */ 0,
  /* 0x05 */ 0,
  /* 0x06 */ 0,
  /* 0x07 */ 0,
  /* 0x08 */ 0,
  /* 0x09 */ 0,
  /* 0x0a */ 0,
  /* 0x0b */ 0,
  /* 0x0c */ 0,
  /* 0x0d */ 0,
  /* 0x0e */ 0,
  /* 0x0f */ 0,
  /* 0x10 */ 0,
  /* 0x11 */ 0,
  /* 0x12 */ 0,
  /* 0x13 */ 0,
  /* 0x14 */ 0,
  /* 0x15 */ 0,
  /* 0x16 */ 0,
  /* 0x17 */ 0,
  /* 0x18 */ 0,
  /* 0x19 */ 0,
  /* 0x1a */ 0,
  /* 0x1b */ 0,
  /* 0x1c */ 0,
  /* 0x1d */ 0,
  /* 0x1e */ 0,
  /* 0x1f */ 0,
  /* 0x20 ' ' */ 0,
  /* 0x21 '!' */ 0x9124,    /* 1001 0001 0010 0100 adhlp      */
  /* 0x22 '"' */ 0x4200,    /* 0100 0010 0000 000x bg         */
  /* 0x23 '#' */ 0x62a8,    /* 0110 0010 1010 100x bcgjln     */
  /* 0x24 '$' */ 0xb6a8,    /* 1011 0110 1010 100x acdfgjln   */
  /* 0x25 '%' */ 0x2510,    /* 0010 0101 0001 000x cfhm       */
  /* 0x26 '&' */ 0x994c,    /* 1001 1001 0100 110x adehknp    */
  /* 0x27 ''' */ 0x0100,    /* 0000 0001 0000 000x h          */
  /* 0x28 '(' */ 0x0140,    /* 0000 0001 0100 000x hk         */
                            /* abcd efgh jklm np.X            */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x29 ')' */ 0x0014,    /* 0000 0000 0001 010x mp */
  /* 0x2a '*' */ 0x03fc,    /* 0000 0011 1111 110x ghjklmnp */
  /* 0x2b '+' */ 0x02a8,    /* 0000 0010 1010 100x gjln */
  /* 0x2c ',' */ 0x0010,    /* 0000 0000 0001 000x m */
  /* 0x2d '-' */ 0x0088,    /* 0000 0000 1000 100x jn */
  /* 0x2e '.' */ 0x0020,    /* 0000 0000 0010 000x l */
  /* 0x2f '/' */ 0x0110,    /* 0000 0001 0001 000x hm */
  /* 0x30 '0' */ 0xfd10,    /* 1111 1101 0001 000x abcdefhm */
  /* 0x31 '1' */ 0x6100,    /* 0110 0001 0000 000x bch */
  /* 0x32 '2' */ 0xd888,    /* 1101 1000 1000 100x abdejn */
  /* 0x33 '3' */ 0xf088,    /* 1111 0000 1000 100x abcdjn */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x34 '4' */ 0x6488,    /* 0110 0100 1000 100x bcfjn */
  /* 0x35 '5' */ 0x9448,    /* 1001 0100 0100 100x adfkn */
  /* 0x36 '6' */ 0xbc88,    /* 1011 1100 1000 100x acdefjn */
  /* 0x37 '7' */ 0x81a8,    /* 1000 0001 1010 100x ahjln */
  /* 0x38 '8' */ 0xfc88,    /* 1111 1100 1000 100x abcdefjn */
  /* 0x39 '9' */ 0xe488,    /* 1110 0100 1000 100x abcfjn */
  /* 0x3a ':' */ 0x4020,    /* 0100 0000 0010 000x bl */
  /* 0x3b ';' */ 0x0210,    /* 0000 0010 0001 000x gm */
  /* 0x3c '<' */ 0x1110,    /* 0001 0001 0001 000x dhm */
  /* 0x3d '=' */ 0x1088,    /* 0001 0000 1000 100x djn */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x3e '>' */ 0x1044,    /* 0001 0000 0100 010x dkp      */
  /* 0x3f '?' */ 0xc4a0,    /* 1100 0100 1010 000x abfjl    */
  /* 0x40 '@' */ 0xde80,    /* 1101 1110 1000 000x abdefgj  */
  /* 0x41 'A' */ 0xec88,    /* 1110 1100 1000 100x abcefjn  */
  /* 0x42 'B' */ 0x9d48,    /* 1001 1101 0100 100x adefhkn  */
  /* 0x43 'C' */ 0x9c00,    /* 1001 1100 0000 000x adef     */
  /* 0x44 'D' */ 0xf220,    /* 1111 0010 0010 000x abcdgl   */
  /* 0x45 'E' */ 0x9c88,    /* 1001 1100 1000 100x adefjn   */
  /* 0x46 'F' */ 0x8c88,    /* 1000 1100 1000 100x aefjn    */
  /* 0x47 'G' */ 0xbc80,    /* 1011 1100 1000 000x acdefj   */
  /* 0x48 'H' */ 0x6c88,    /* 0110 1100 1000 100x bcefnj   */
  /* 0x49 'I' */ 0x9220,    /* 1001 0010 0010 000x adgl     */
  /* 0x4a 'J' */ 0x7800,    /* 0111 1000 0000 000x bcde     */
  /* 0x4b 'K' */ 0x0d48,    /* 0000 1101 0100 100x efhkn    */
  /* 0x4c 'L' */ 0x1c00,    /* 0001 1100 0000 000x def      */
  /* 0x4d 'M' */ 0x6d04,    /* 0110 1101 0000 010x bcefhp   */
  /* 0x4e 'N' */ 0x6c44,    /* 0110 1100 0100 010x bcefkp   */
  /* 0x4f 'O' */ 0xfc00,    /* 1111 1100 0000 000x abcdef   */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x50 'P' */ 0xcc88,    /* 1100 1100 1000 100x abefjn     */
  /* 0x51 'Q' */ 0xfc40,    /* 1111 1100 0100 000x abcdefk    */
  /* 0x52 'R' */ 0xccc8,    /* 1100 1100 1100 100x abefjkn    */
  /* 0x53 'S' */ 0xb488,    /* 1011 0100 1000 100x acdfjn     */
  /* 0x54 'T' */ 0x8220,    /* 1000 0010 0010 000x agl        */
  /* 0x55 'U' */ 0x7c00,    /* 0111 1100 0000 000x bcdef      */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x56 'V' */ 0x0d10,    /* 0000 1101 0001 000x efhm     */
  /* 0x57 'W' */ 0x6c50,    /* 0110 1100 0101 000x bcefkm   */
  /* 0x58 'X' */ 0x0154,    /* 0000 0001 0101 010x hkmp     */
  /* 0x59 'Y' */ 0x0124,    /* 0000 0001 0010 010x hlp      */
  /* 0x5a 'Z' */ 0x9110,    /* 1001 0001 0001 000x adhm     */
  /* 0x5b '[' */ 0x901c,    /* 1001 0000 0001 110x admnp    */
  /* 0x5c '\' */ 0x0044,    /* 0000 0000 0100 010x kp       */
  /* 0x5d ']' */ 0x91c0,    /* 1001 0001 1100 000x adhjk    */
  /* 0x5e '^' */ 0x4100,    /* 0100 0001 0000 000x bh       */
  /* 0x5f '_' */ 0x1000,    /* 0001 0000 0000 000x d        */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x60 '`' */ 0x0004,    /* 0000 0000 0000 010x p		*/
  /* 0x61 'a' */ 0x1828,    /* 0001 1000 0010 100x deln		*/
#if 0
  /* 0x61 'a' */ 0x3090,    /* 0011 0000 1001 000x cdjm		*/
#endif
  /* 0x62 'b' */ 0x3c88,    /* 0011 1100 1000 100x cdefjn	*/
  /* 0x63 'c' */ 0x1888,    /* 0001 1000 1000 100x dejn		*/
  /* 0x64 'd' */ 0x7888,    /* 0111 1000 1000 100x bcdejn	*/
  /* 0x65 'e' */ 0x1818,    /* 0001 1000 0001 100x demn		*/
  /* 0x66 'f' */ 0x01a8,    /* 0000 0001 1010 100x hjln		*/
  /* 0x67 'g' */ 0x7180,    /* 0111 0001 1000 000x bcdhj	*/
  /* 0x68 'h' */ 0x2c88,    /* 0010 1100 1000 100x cefjn	*/
  /* 0x69 'i' */ 0x1020,    /* 0001 0000 0010 000x dl		*/
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x6a 'j' */ 0x3000,    /* 0111 0000 0000 000x cd		*/
  /* 0x6b 'k' */ 0x0360,    /* 0000 0011 0110 000x ghkl		*/
  /* 0x6c 'l' */ 0x1220,    /* 0001 0010 0010 000x dgl		*/
  /* 0x6d 'm' */ 0x28a8,    /* 0010 1000 1010 100x cejln	*/
  /* 0x6e 'n' */ 0x0848,    /* 0000 1000 0100 100x ekn		*/
  /* 0x6f 'o' */ 0x3888,    /* 0011 1000 1000 100x cdejn	*/
  /* 0x70 'p' */ 0x8d08,    /* 1000 1101 0000 100x aefhn	*/
  /* 0x71 'q' */ 0xe084,    /* 1110 0000 1000 010x abcjp	*/
  /* 0x72 'r' */ 0x0888,    /* 0000 1000 1000 100x ejn		*/
  /* 0x73 's' */ 0x10c0,    /* 0001 0000 1100 000x djk		*/
  /* 0x74 't' */ 0x1c08,    /* 0001 1100 0000 100x defn		*/
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
  /* 0x75 'u' */ 0x3800,    /* 0011 1000 0000 000x cde      */
  /* 0x76 'v' */ 0x0810,    /* 0000 1000 0001 000x em       */
  /* 0x77 'w' */ 0x2850,    /* 0010 1000 0101 000x cekm     */
  /* 0x78 'x' */ 0x01dc,    /* 0000 0001 1101 110x hjkmnp   */
  /* 0x79 'y' */ 0x7280,    /* 0111 0010 1000 000x bcdgj    */
  /* 0x7a 'z' */ 0x1018,    /* 0001 0000 0001 100x dmn      */
  /* 0x7b '{' */ 0x01c0,    /* 0000 0001 1100 000x hjk      */
  /* 0x7c '|' */ 0x0c00,    /* 0000 1100 0000 000x ef       */
  /* 0x7d '}' */ 0x001c,    /* 0000 0000 0001 110x mnp      */
  /* 0x7e '~' */ 0x8000,    /* 1000 0000 0000 000x a        */
  /* 0x7f '' */  0xfffc,    /* 1111 1111 1111 110x          */
                            /* abcd efgh jklm np.X */
/*   A
    PGH
    ___
F  |\|/|  B
N   - -   J
E  |/|\|  C
    ---
    MLK
     D
*/
};




short adafruit_lookup[256];

int translate_to_adafruit(void) {

	int i;

	for(i=0;i<256;i++) {

		adafruit_lookup[i]=0;

		if (ascii_lookup[i]&(1<<15)) adafruit_lookup[i]|=(1<< 8); // A
		if (ascii_lookup[i]&(1<<14)) adafruit_lookup[i]|=(1<< 9); // B
		if (ascii_lookup[i]&(1<<13)) adafruit_lookup[i]|=(1<<10); // C
		if (ascii_lookup[i]&(1<<12)) adafruit_lookup[i]|=(1<<11); // D
		if (ascii_lookup[i]&(1<<11)) adafruit_lookup[i]|=(1<<12); // E
		if (ascii_lookup[i]&(1<<10)) adafruit_lookup[i]|=(1<<13); // F
		if (ascii_lookup[i]&(1<< 9)) adafruit_lookup[i]|=(1<< 1); // G
		if (ascii_lookup[i]&(1<< 8)) adafruit_lookup[i]|=(1<< 2); // H
		if (ascii_lookup[i]&(1<< 7)) adafruit_lookup[i]|=(1<<15); // J
		if (ascii_lookup[i]&(1<< 6)) adafruit_lookup[i]|=(1<< 5); // K
		if (ascii_lookup[i]&(1<< 5)) adafruit_lookup[i]|=(1<< 4); // L
		if (ascii_lookup[i]&(1<< 4)) adafruit_lookup[i]|=(1<< 3); // M
		if (ascii_lookup[i]&(1<< 3)) adafruit_lookup[i]|=(1<<14); // N
		if (ascii_lookup[i]&(1<< 2)) adafruit_lookup[i]|=(1<< 0); // P
		if (ascii_lookup[i]&(1<< 1)) adafruit_lookup[i]|=(1<< 6); // .
	}

	return 0;
}
