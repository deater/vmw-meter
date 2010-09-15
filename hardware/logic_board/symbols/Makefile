all:	ltp3786e.sym vmwledbus.sym lta1000g.sym 74hc05.sym saa1064.sym

saa1064.sym:	saa1064.symdef
	./djboxsym < saa1064.symdef > saa1064.sym
	
74hc05.sym:	74hc05.symdef
	./djboxsym < 74hc05.symdef > 74hc05.sym

ltp3786e.sym:	ltp3786e.symdef
	./djboxsym < ltp3786e.symdef > ltp3786e.sym
	
vmwledbus.sym:	vmwledbus.symdef
	./djboxsym < vmwledbus.symdef > vmwledbus.sym	

lta1000g.sym:	lta1000g.symdef
	./djboxsym < lta1000g.symdef > lta1000g.sym	


clean:	
	rm -f *~

