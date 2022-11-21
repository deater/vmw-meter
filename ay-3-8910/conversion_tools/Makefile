include ../Makefile.include

YMLIB_INCLUDE = -I../ymlib/
NOTES_INC = -I../noteslib/
LZ4_LIB = -llz4

all:	dump_xm xm_to_text \
	dump_ym5 \
	dump_ed text_to_ed \
	raw_to_ym text_to_ym ym_to_ym5 raw_interleave \
	ym5_to_raw ym5_to_mockingboard ym5_to_krg ym5_to_krw ym5_to_kr4 \
	ym5_to_tfv ym5_to_mega ym5_to_kf apple_dump_ym5

../ymlib/ym_lib.o:	
	cd ../ymlib && make

###

xm_lib.o:	xm_lib.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_lib.c

dump_xm.o:	dump_xm.c xm_lib.h
	$(CC) $(CFLAGS) -c dump_xm.c

dump_xm:	dump_xm.o xm_lib.o
	$(CC) -o dump_xm dump_xm.o xm_lib.o $(LFLAGS)


xm_to_text.o:	xm_to_text.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_to_text.c

xm_to_text:	xm_to_text.o xm_lib.o
	$(CC) -o xm_to_text xm_to_text.o xm_lib.o $(LFLAGS)



###

ym_lib.o:	ym_lib.c ym_lib.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_lib.c




######


dump_ym5:	dump_ym5.o ../ymlib/ym_lib.o
	$(CC) -o dump_ym5 dump_ym5.o ../ymlib/ym_lib.o $(LFLAGS) $(LIBLHASA_LIB) 

dump_ym5.o:	dump_ym5.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c dump_ym5.c

######


apple_dump_ym5:	apple_dump_ym5.o ../ymlib/ym_lib.o
	$(CC) -o apple_dump_ym5 apple_dump_ym5.o ../ymlib/ym_lib.o $(LFLAGS) $(LIBLHASA_LIB) 

apple_dump_ym5.o:	apple_dump_ym5.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c apple_dump_ym5.c



###

ym5_to_mockingboard:	ym5_to_mockingboard.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_mockingboard ym5_to_mockingboard.o ../ymlib/ym_lib.o $(LFLAGS) $(LIBLHASA_LIB) 

ym5_to_mockingboard.o:	ym5_to_mockingboard.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_mockingboard.c

###

ym5_to_raw:	ym5_to_raw.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_raw ym5_to_raw.o ../ymlib/ym_lib.o $(LFLAGS) $(LIBLHASA_LIB)

ym5_to_raw.o:	ym5_to_raw.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_raw.c

###

ym5_to_krg:	ym5_to_krg.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_krg ym5_to_krg.o ../ymlib/ym_lib.o $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB)

ym5_to_krg.o:	ym5_to_krg.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_krg.c

###

ym5_to_krw:	ym5_to_krw.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_krw ym5_to_krw.o ../ymlib/ym_lib.o $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB)

ym5_to_krw.o:	ym5_to_krw.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_krw.c

###

ym5_to_kr4:	ym5_to_kr4.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_kr4 ym5_to_kr4.o ../ymlib/ym_lib.o $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB) 

ym5_to_kr4.o:	ym5_to_kr4.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_kr4.c

###

ym5_to_tfv:	ym5_to_tfv.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_tfv ym5_to_tfv.o ../ymlib/ym_lib.o $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB)

ym5_to_tfv.o:	ym5_to_tfv.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_tfv.c

###

ym5_to_mega:	ym5_to_mega.o ../ymlib/ym_lib.o
	$(CC) -o ym5_to_mega ym5_to_mega.o ../ymlib/ym_lib.o $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB)

ym5_to_mega.o:	ym5_to_mega.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_mega.c

###

ym5_to_kf:	ym5_to_kf.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) -o ym5_to_kf ym5_to_kf.o ../ymlib/ym_lib.o $(LZ4_LIB) $(LIBLHASA_LIB)

ym5_to_kf.o:	ym5_to_kf.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_kf.c



###

text_to_ym:	text_to_ym.o ../noteslib/notes.o
	$(CC) -o text_to_ym text_to_ym.o ../noteslib/notes.o $(LFLAGS)

text_to_ym.o:	text_to_ym.c ../noteslib/notes.h
	$(CC) $(CFLAGS) $(NOTES_INC) -c text_to_ym.c

###

raw_to_ym:	raw_to_ym.o
	$(CC) -o raw_to_ym raw_to_ym.o $(LFLAGS)

raw_to_ym.o:	raw_to_ym.c
	$(CC) $(CFLAGS) -c raw_to_ym.c

####

text_to_ed:	text_to_ed.o ../noteslib/notes.o
	$(CC) -o text_to_ed text_to_ed.o ../noteslib/notes.o $(LFLAGS)

text_to_ed.o:	text_to_ed.c ../noteslib/notes.h
	$(CC) $(CFLAGS) $(NOTES_INC) -c text_to_ed.c


dump_ed:	dump_ed.o
	$(CC) -o dump_ed dump_ed.o $(LFLAGS)

dump_ed.o:	dump_ed.c
	$(CC) $(CFLAGS) -c dump_ed.c

###

ym_to_ym5:	ym_to_ym5.o
	$(CC) -o ym_to_ym5 ym_to_ym5.o $(LFLAGS)

ym_to_ym5.o:	ym_to_ym5.c
	$(CC) $(CFLAGS) -c ym_to_ym5.c


###

raw_interleave:	raw_interleave.o
	$(CC) -o raw_interleave raw_interleave.o $(LFLAGS)

raw_interleave.o:	raw_interleave.c
	$(CC) $(CFLAGS) -c raw_interleave.c


###

clean:	
	rm -f *~ *.o text_to_ym ym_to_ym5 text_to_ed \
		dump_ed dump_ym5 dump_xm xm_to_text ym5_to_mockingboard \
		ym5_to_raw ym5_to_krg ym5_to_krw raw_to_ym ym5_to_kr4 \
		ym5_to_tfv ym5_to_kf raw_interleave
