all:	pi-ps2.zip

pi-ps2.zip:	\
	pi-ps2.GTL \
	pi-ps2.GBL \
	pi-ps2.GTS \
	pi-ps2.GBS \
	pi-ps2.GTO \
	pi-ps2.GBO \
	pi-ps2.GKO \
	pi-ps2.XLN
	zip pi-ps2.zip \
		pi-ps2.GTL \
	        pi-ps2.GBL \
		pi-ps2.GTS \
		pi-ps2.GBS \
		pi-ps2.GTO \
		pi-ps2.GBO \
		pi-ps2.GKO \
		pi-ps2.XLN


pi-ps2.GTL:	pi-ps2.top.gbr
	cp pi-ps2.top.gbr pi-ps2.GTL

pi-ps2.GBL:	pi-ps2.bottom.gbr
	cp pi-ps2.bottom.gbr pi-ps2.GBL

pi-ps2.GTS:	pi-ps2.topmask.gbr
	cp pi-ps2.topmask.gbr pi-ps2.GTS

pi-ps2.GBS:	pi-ps2.bottommask.gbr
	cp pi-ps2.bottommask.gbr pi-ps2.GBS

pi-ps2.GTO:	pi-ps2.topsilk.gbr
	cp pi-ps2.topsilk.gbr pi-ps2.GTO

pi-ps2.GBO:	pi-ps2.bottomsilk.gbr
	cp pi-ps2.bottomsilk.gbr pi-ps2.GBO

pi-ps2.GKO:	pi-ps2.outline.gbr
	cp pi-ps2.outline.gbr pi-ps2.GKO

pi-ps2.XLN:	pi-ps2.plated-drill.cnc
	cp pi-ps2.plated-drill.cnc pi-ps2.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

