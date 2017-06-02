all:    vmw_chiptune_build.pdf

vmw_chiptune_build.pdf:	vmw_chiptune_build.dvi
	dvipdf vmw_chiptune_build.dvi

vmw_chiptune_build.dvi:	vmw_chiptune_build.tex
	latex vmw_chiptune_build.tex
	latex vmw_chiptune_build.tex

clean:  
	rm -f *.dvi *.pdf *~ *.aux *.log

