all:	flux_cap.zip

flux_cap.zip:	\
	flux_cap.GTL \
	flux_cap.GBL \
	flux_cap.GTS \
	flux_cap.GBS \
	flux_cap.GTO \
	flux_cap.GBO \
	flux_cap.GKO \
	flux_cap.XLN
	zip flux_cap.zip \
		flux_cap.GTL \
	        flux_cap.GBL \
		flux_cap.GTS \
		flux_cap.GBS \
		flux_cap.GTO \
		flux_cap.GBO \
		flux_cap.GKO \
		flux_cap.XLN


flux_cap.GTL:	flux_cap.top.gbr
	cp flux_cap.top.gbr flux_cap.GTL

flux_cap.GBL:	flux_cap.bottom.gbr
	cp flux_cap.bottom.gbr flux_cap.GBL

flux_cap.GTS:	flux_cap.topmask.gbr
	cp flux_cap.topmask.gbr flux_cap.GTS

flux_cap.GBS:	flux_cap.bottommask.gbr
	cp flux_cap.bottommask.gbr flux_cap.GBS

flux_cap.GTO:	flux_cap.topsilk.gbr
	cp flux_cap.topsilk.gbr flux_cap.GTO

flux_cap.GBO:	flux_cap.bottomsilk.gbr
	cp flux_cap.bottomsilk.gbr flux_cap.GBO

flux_cap.GKO:	flux_cap.outline.gbr
	cp flux_cap.outline.gbr flux_cap.GKO

flux_cap.XLN:	flux_cap.combined.cnc
	cp flux_cap.combined.cnc flux_cap.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

