all:	ltp3786e.sym vmwledbus.sym lta1000g.sym

ltp3786e.sym:	ltp3786e.symdef
	./djboxsym < ltp3786e.symdef > ltp3786e.sym
	
vmwledbus.sym:	vmwledbus.symdef
	./djboxsym < vmwledbus.symdef > vmwledbus.sym	

lta1000g.sym:	lta1000g.symdef
	./djboxsym < lta1000g.symdef > lta1000g.sym	


clean:	
	rm -f *~ *.sym

