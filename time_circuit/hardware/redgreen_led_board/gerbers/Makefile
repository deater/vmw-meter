all:	redgreen_ledboard.zip


redgreen_ledboard.zip:	\
	redgreen_ledboard.GTL \
	redgreen_ledboard.GBL \
	redgreen_ledboard.GTS \
	redgreen_ledboard.GBS \
	redgreen_ledboard.GTO \
	redgreen_ledboard.GKO \
	redgreen_ledboard.XLN
	zip redgreen_ledboard.zip \
		redgreen_ledboard.GTL \
	        redgreen_ledboard.GBL \
		redgreen_ledboard.GTS \
		redgreen_ledboard.GBS \
		redgreen_ledboard.GTO \
		redgreen_ledboard.GKO \
		redgreen_ledboard.XLN


redgreen_ledboard.GTL:	redgreen_ledboard.possible3.top.gbr
	cp redgreen_ledboard.possible3.top.gbr redgreen_ledboard.GTL

redgreen_ledboard.GBL:	redgreen_ledboard.possible3.bottom.gbr
	cp redgreen_ledboard.possible3.bottom.gbr redgreen_ledboard.GBL

redgreen_ledboard.GTS:	redgreen_ledboard.possible3.topmask.gbr
	cp redgreen_ledboard.possible3.topmask.gbr redgreen_ledboard.GTS

redgreen_ledboard.GBS:	redgreen_ledboard.possible3.bottommask.gbr
	cp redgreen_ledboard.possible3.bottommask.gbr redgreen_ledboard.GBS

redgreen_ledboard.GTO:	redgreen_ledboard.possible3.topsilk.gbr
	cp redgreen_ledboard.possible3.topsilk.gbr redgreen_ledboard.GTO

redgreen_ledboard.GBO:	redgreen_ledboard.possible3.bottomsilk.gbr
	cp redgreen_ledboard.possible3.bottomsilk.gbr redgreen_ledboard.GBO

redgreen_ledboard.GKO:	redgreen_ledboard.possible3.outline.gbr
	cp redgreen_ledboard.possible3.outline.gbr redgreen_ledboard.GKO

redgreen_ledboard.XLN:	redgreen_ledboard.possible3.combined.cnc
	cp redgreen_ledboard.possible3.combined.cnc redgreen_ledboard.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN

