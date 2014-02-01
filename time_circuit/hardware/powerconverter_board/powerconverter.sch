v 20130925 2
C 40000 40000 0 0 0 title-B.sym
C 54200 50000 1 0 0 led-2.sym
{
T 55000 50300 5 10 1 1 0 0 1
refdes=D1
T 54300 50600 5 10 0 0 0 0 1
device=LED
T 54300 50600 5 10 0 0 0 0 1
footprint=T1.75_LED
}
C 53300 50000 1 0 0 resistor-1.sym
{
T 53600 50400 5 10 0 0 0 0 1
device=RESISTOR
T 53500 50300 5 10 1 1 0 0 1
refdes=R1
T 53500 50300 5 10 1 1 0 0 1
footprint=0.125W_Carbon_Resistor
}
C 40700 41600 1 0 0 pwrjack-1.sym
{
T 40800 42100 5 10 0 0 0 0 1
device=PWRJACK
T 40700 42100 5 10 1 1 0 0 1
refdes=CONN1
T 40700 41600 5 10 0 0 0 0 1
footprint=powerjack
}
C 53200 48400 1 0 0 i2c_connector.sym
{
T 58500 49200 5 10 0 0 0 0 1
footprint=JUMPER4
T 58500 49600 5 10 0 0 0 0 1
device=i2c connector
T 56000 48100 5 10 1 1 0 3 1
refdes=ISQUAREDC1
}
C 53200 47100 1 0 0 i2c_connector.sym
{
T 58500 47900 5 10 0 0 0 0 1
footprint=JUMPER4
T 58500 48300 5 10 0 0 0 0 1
device=i2c connector
T 56000 46800 5 10 1 1 0 3 1
refdes=ISQUAREDC2
}
C 42800 40800 1 0 0 i2c_connector.sym
{
T 48100 41600 5 10 0 0 0 0 1
footprint=JUMPER4
T 48100 42000 5 10 0 0 0 0 1
device=i2c connector
T 45600 40500 5 10 1 1 0 3 1
refdes=ISQUAREDC3
}
C 40800 44300 1 0 0 header26-1.sym
{
T 41400 49700 5 10 1 1 0 0 1
refdes=J1
T 42400 49000 5 10 0 0 0 0 1
footprint=HEADER26_SHROUD
}
C 45700 47900 1 0 0 header3-1.sym
{
T 46700 48550 5 10 0 0 0 0 1
device=HEADER3
T 46100 49200 5 10 1 1 0 0 1
refdes=J2
T 45700 47900 5 10 0 0 0 0 1
footprint=JUMPER3
}
C 48500 43800 1 0 0 levelshifter.sym
{
T 53600 44900 5 10 0 0 0 0 1
footprint=DIP-12-400
T 53600 45300 5 10 0 0 0 0 1
device=levelshift
T 49400 44800 5 10 1 1 0 0 1
refdes=IC1
}
C 43800 44800 1 0 0 usba.sym
{
T 49100 45600 5 10 0 0 0 0 1
footprint=USBA
T 49100 46000 5 10 0 0 0 0 1
device=usb-a connector
T 46600 44500 5 10 1 1 0 3 1
refdes=USB1
}
