all:    vmw_chiptune_build.pdf

vmw_chiptune_build.pdf:	vmw_chiptune_build.tex
	pdflatex vmw_chiptune_build.tex
	bibtex vmw_chiptune_build
	pdflatex vmw_chiptune_build.tex
	pdflatex vmw_chiptune_build.tex

clean:  
	rm -f *.dvi *.pdf *~ *.aux *.log

