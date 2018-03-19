include ../Makefile.include

YMLIB_INCLUDE = -I../ymlib/
NOTES_INC = -I../noteslib/
LZ4_LIB = -llz4

all:	dump_xm xm_to_text \
	dump_ym5 text_to_ed text_to_ym ym_to_ym5 \
	ym5_to_raw ym5_to_mockingboard ym5_to_krg ym5_to_krw

../ymlib/ym_lib.o:	
	cd ../ymlib && make

###

xm_lib.o:	xm_lib.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_lib.c

dump_xm.o:	dump_xm.c xm_lib.h
	$(CC) $(CFLAGS) -c dump_xm.c

dump_xm:	dump_xm.o xm_lib.o
	$(CC) $(LFLAGS) -o dump_xm dump_xm.o xm_lib.o


xm_to_text.o:	xm_to_text.c xm_lib.h
	$(CC) $(CFLAGS) -c xm_to_text.c

xm_to_text:	xm_to_text.o xm_lib.o
	$(CC) $(LFLAGS) -o xm_to_text xm_to_text.o xm_lib.o



###

ym_lib.o:	ym_lib.c ym_lib.h
	$(CC) $(CFLAGS) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c ym_lib.c




######


dump_ym5:	dump_ym5.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o dump_ym5 dump_ym5.o ../ymlib/ym_lib.o

dump_ym5.o:	dump_ym5.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) $(LIBLHASA_INCLUDEDIR) $(USE_LIBLHASA) -c dump_ym5.c


###

ym5_to_mockingboard:	ym5_to_mockingboard.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o ym5_to_mockingboard ym5_to_mockingboard.o ../ymlib/ym_lib.o

ym5_to_mockingboard.o:	ym5_to_mockingboard.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_mockingboard.c

###

ym5_to_raw:	ym5_to_raw.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) $(LIBLHASA_LIB) -o ym5_to_raw ym5_to_raw.o ../ymlib/ym_lib.o

ym5_to_raw.o:	ym5_to_raw.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_raw.c

###

ym5_to_krg:	ym5_to_krg.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB) -o ym5_to_krg ym5_to_krg.o ../ymlib/ym_lib.o

ym5_to_krg.o:	ym5_to_krg.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_krg.c

###

ym5_to_krw:	ym5_to_krw.o ../ymlib/ym_lib.o
	$(CC) $(LFLAGS) $(LZ4_LIB) $(LIBLHASA_LIB) -o ym5_to_krw ym5_to_krw.o ../ymlib/ym_lib.o

ym5_to_krw.o:	ym5_to_krw.c
	$(CC) $(CFLAGS) $(YMLIB_INCLUDE) -c ym5_to_krw.c



###

text_to_ym:	text_to_ym.o ../noteslib/notes.o
	$(CC) $(LFLAGS) -o text_to_ym text_to_ym.o ../noteslib/notes.o

text_to_ym.o:	text_to_ym.c ../noteslib/notes.h
	$(CC) $(CFLAGS) $(NOTES_INC) -c text_to_ym.c

####

text_to_ed:	text_to_ed.o ../noteslib/notes.o
	$(CC) $(LFLAGS) -o text_to_ed text_to_ed.o ../noteslib/notes.o

text_to_ed.o:	text_to_ed.c ../noteslib/notes.h
	$(CC) $(CFLAGS) $(NOTES_INC) -c text_to_ed.c


###

ym_to_ym5:	ym_to_ym5.o
	$(CC) $(LFLAGS) -o ym_to_ym5 ym_to_ym5.o

ym_to_ym5.o:	ym_to_ym5.c
	$(CC) $(CFLAGS) -c ym_to_ym5.c

###

clean:	
	rm -f *~ *.o text_to_ym ym_to_ym5 text_to_ed \
		dump_ym5 dump_xm xm_to_text ym5_to_mockingboard \
		ym5_to_raw ym5_to_krg ym5_to_krw
