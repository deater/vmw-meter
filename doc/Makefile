all:	svmw_meter_build.pdf

svmw_meter_build.pdf:	svmw_meter_build.dvi
	dvipdf svmw_meter_build.dvi

svmw_meter_build.dvi:	svmw_meter_build.tex
	latex svmw_meter_build.tex

clean:	
	rm -f *.dvi *.pdf *!


