all:	slider_board.zip

slider_board.zip:	\
	slider_board.GTL \
	slider_board.GBL \
	slider_board.GTS \
	slider_board.GBS \
	slider_board.GTO \
	slider_board.GBO \
	slider_board.GKO \
	slider_board.XLN
	zip slider_board.zip \
		slider_board.GTL \
	        slider_board.GBL \
		slider_board.GTS \
		slider_board.GBS \
		slider_board.GTO \
		slider_board.GBO \
		slider_board.GKO \
		slider_board.XLN


slider_board.GTL:	slider_board.top.gbr
	cp slider_board.top.gbr slider_board.GTL

slider_board.GBL:	slider_board.bottom.gbr
	cp slider_board.bottom.gbr slider_board.GBL

slider_board.GTS:	slider_board.topmask.gbr
	cp slider_board.topmask.gbr slider_board.GTS

slider_board.GBS:	slider_board.bottommask.gbr
	cp slider_board.bottommask.gbr slider_board.GBS

slider_board.GTO:	slider_board.topsilk.gbr
	cp slider_board.topsilk.gbr slider_board.GTO

slider_board.GBO:	slider_board.bottomsilk.gbr
	cp slider_board.bottomsilk.gbr slider_board.GBO

slider_board.GKO:	slider_board.outline.gbr
	cp slider_board.outline.gbr slider_board.GKO

slider_board.XLN:	slider_board.combined.cnc
	cp slider_board.combined.cnc slider_board.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

