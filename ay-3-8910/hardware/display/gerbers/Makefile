all:	display_routed.zip

display_routed.zip:	\
	display_routed.GTL \
	display_routed.GBL \
	display_routed.GTS \
	display_routed.GBS \
	display_routed.GTO \
	display_routed.GBO \
	display_routed.GKO \
	display_routed.XLN
	zip display_routed.zip \
		display_routed.GTL \
	        display_routed.GBL \
		display_routed.GTS \
		display_routed.GBS \
		display_routed.GTO \
		display_routed.GBO \
		display_routed.GKO \
		display_routed.XLN


display_routed.GTL:	display_routed.top.gbr
	cp display_routed.top.gbr display_routed.GTL

display_routed.GBL:	display_routed.bottom.gbr
	cp display_routed.bottom.gbr display_routed.GBL

display_routed.GTS:	display_routed.topmask.gbr
	cp display_routed.topmask.gbr display_routed.GTS

display_routed.GBS:	display_routed.bottommask.gbr
	cp display_routed.bottommask.gbr display_routed.GBS

display_routed.GTO:	display_routed.topsilk.gbr
	cp display_routed.topsilk.gbr display_routed.GTO

display_routed.GBO:	display_routed.bottomsilk.gbr
	cp display_routed.bottomsilk.gbr display_routed.GBO

display_routed.GKO:	display_routed.outline.gbr
	cp display_routed.outline.gbr display_routed.GKO

display_routed.XLN:	display_routed.combined.cnc
	cp display_routed.combined.cnc display_routed.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

