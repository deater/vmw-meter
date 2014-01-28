all:	time_circuit_build.pdf

time_circuit_build.pdf:	time_circuit_build.dvi
	dvipdf time_circuit_build.dvi

time_circuit_build.dvi:	time_circuit_build.tex
	latex time_circuit_build.tex
	latex time_circuit_build.tex

clean:	
	rm -f *.dvi *.pdf *~ *.aux *.log


