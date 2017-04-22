all:	sound_ground_fill.zip

sound_ground_fill.zip:	\
	sound_ground_fill.GTL \
	sound_ground_fill.GBL \
	sound_ground_fill.GTS \
	sound_ground_fill.GBS \
	sound_ground_fill.GTO \
	sound_ground_fill.GBO \
	sound_ground_fill.GKO \
	sound_ground_fill.XLN
	zip sound_ground_fill.zip \
		sound_ground_fill.GTL \
	        sound_ground_fill.GBL \
		sound_ground_fill.GTS \
		sound_ground_fill.GBS \
		sound_ground_fill.GTO \
		sound_ground_fill.GBO \
		sound_ground_fill.GKO \
		sound_ground_fill.XLN


sound_ground_fill.GTL:	sound_ground_fill.top.gbr
	cp sound_ground_fill.top.gbr sound_ground_fill.GTL

sound_ground_fill.GBL:	sound_ground_fill.bottom.gbr
	cp sound_ground_fill.bottom.gbr sound_ground_fill.GBL

sound_ground_fill.GTS:	sound_ground_fill.topmask.gbr
	cp sound_ground_fill.topmask.gbr sound_ground_fill.GTS

sound_ground_fill.GBS:	sound_ground_fill.bottommask.gbr
	cp sound_ground_fill.bottommask.gbr sound_ground_fill.GBS

sound_ground_fill.GTO:	sound_ground_fill.topsilk.gbr
	cp sound_ground_fill.topsilk.gbr sound_ground_fill.GTO

sound_ground_fill.GBO:	sound_ground_fill.bottomsilk.gbr
	cp sound_ground_fill.bottomsilk.gbr sound_ground_fill.GBO

sound_ground_fill.GKO:	sound_ground_fill.outline.gbr
	cp sound_ground_fill.outline.gbr sound_ground_fill.GKO

sound_ground_fill.XLN:	sound_ground_fill.combined.cnc
	cp sound_ground_fill.combined.cnc sound_ground_fill.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

