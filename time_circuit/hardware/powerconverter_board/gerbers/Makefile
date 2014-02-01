all:	powerconverter_routed.zip

powerconverter_routed.zip:	\
	powerconverter_routed.GTL \
	powerconverter_routed.GBL \
	powerconverter_routed.GTS \
	powerconverter_routed.GBS \
	powerconverter_routed.GTO \
	powerconverter_routed.GBO \
	powerconverter_routed.GKO \
	powerconverter_routed.XLN
	zip powerconverter_routed.zip \
		powerconverter_routed.GTL \
	        powerconverter_routed.GBL \
		powerconverter_routed.GTS \
		powerconverter_routed.GBS \
		powerconverter_routed.GTO \
		powerconverter_routed.GBO \
		powerconverter_routed.GKO \
		powerconverter_routed.XLN


powerconverter_routed.GTL:	powerconverter_routed.top.gbr
	cp powerconverter_routed.top.gbr powerconverter_routed.GTL

powerconverter_routed.GBL:	powerconverter_routed.bottom.gbr
	cp powerconverter_routed.bottom.gbr powerconverter_routed.GBL

powerconverter_routed.GTS:	powerconverter_routed.topmask.gbr
	cp powerconverter_routed.topmask.gbr powerconverter_routed.GTS

powerconverter_routed.GBS:	powerconverter_routed.bottommask.gbr
	cp powerconverter_routed.bottommask.gbr powerconverter_routed.GBS

powerconverter_routed.GTO:	powerconverter_routed.topsilk.gbr
	cp powerconverter_routed.topsilk.gbr powerconverter_routed.GTO

powerconverter_routed.GBO:	powerconverter_routed.bottomsilk.gbr
	cp powerconverter_routed.bottomsilk.gbr powerconverter_routed.GBO

powerconverter_routed.GKO:	powerconverter_routed.outline.gbr
	cp powerconverter_routed.outline.gbr powerconverter_routed.GKO

powerconverter_routed.XLN:	powerconverter_routed.combined.cnc
	cp powerconverter_routed.combined.cnc powerconverter_routed.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

