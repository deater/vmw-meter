# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element[0x0 "USBA" "" "" 0 0   0   0 3 100 0x0]
(

	Pin[ -13777 -10670 6000  2000 7000 3800 "" "1" 0x1]
	Pin[ -3937  -10670 6000  2000 7000 3800 "" "2" 0x1]
	Pin[ 3937  -10670 6000  2000 7000 3800 "" "3" 0x1]
	Pin[ 13777 -10670 6000  2000 7000 3800 "" "4" 0x1]

	Pin[ -25850  0 10050  2000 10000 9050 "" "5" 0x1]
	Pin[ 25850  0 10050  2000 10000 9050 "" "6" 0x1]

	ElementLine[-32500 -21340 -32500 40550 1000]
	ElementLine[-32500 40550 32500 40550 1000]
	ElementLine[32500 40550 32500 -21340 1000]
	ElementLine[32500 -21340 -32500 -21340 1000]
)
