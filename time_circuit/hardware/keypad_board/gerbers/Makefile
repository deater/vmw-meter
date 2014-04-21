all:	keypad_board_routed.zip

keypad_board_routed.zip:	\
	keypad_board_routed.GTL \
	keypad_board_routed.GBL \
	keypad_board_routed.GTS \
	keypad_board_routed.GBS \
	keypad_board_routed.GTO \
	keypad_board_routed.GBO \
	keypad_board_routed.GKO \
	keypad_board_routed.XLN
	zip keypad_board_routed.zip \
		keypad_board_routed.GTL \
	        keypad_board_routed.GBL \
		keypad_board_routed.GTS \
		keypad_board_routed.GBS \
		keypad_board_routed.GTO \
		keypad_board_routed.GBO \
		keypad_board_routed.GKO \
		keypad_board_routed.XLN


keypad_board_routed.GTL:	keypad_board_routed.top.gbr
	cp keypad_board_routed.top.gbr keypad_board_routed.GTL

keypad_board_routed.GBL:	keypad_board_routed.bottom.gbr
	cp keypad_board_routed.bottom.gbr keypad_board_routed.GBL

keypad_board_routed.GTS:	keypad_board_routed.topmask.gbr
	cp keypad_board_routed.topmask.gbr keypad_board_routed.GTS

keypad_board_routed.GBS:	keypad_board_routed.bottommask.gbr
	cp keypad_board_routed.bottommask.gbr keypad_board_routed.GBS

keypad_board_routed.GTO:	keypad_board_routed.topsilk.gbr
	cp keypad_board_routed.topsilk.gbr keypad_board_routed.GTO

keypad_board_routed.GBO:	keypad_board_routed.bottomsilk.gbr
	cp keypad_board_routed.bottomsilk.gbr keypad_board_routed.GBO

keypad_board_routed.GKO:	keypad_board_routed.outline.gbr
	cp keypad_board_routed.outline.gbr keypad_board_routed.GKO

keypad_board_routed.XLN:	keypad_board_routed.combined.cnc
	cp keypad_board_routed.combined.cnc keypad_board_routed.XLN


clean:	
	rm -f *~ *.zip *.GTL *.GBL *.GTS *.GBS *.GTO *.GKO *.XLN *.GBO

