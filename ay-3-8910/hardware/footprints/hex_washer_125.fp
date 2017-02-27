# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "hex_washer_125" "" "" 20062 18801 -169000 -210000 0 100 ""]
(
	Pin[0 0 15000 1000 16000 12600 "" "1" 0x9]
	ElementLine [-12500 0 -6250 10820 100]
	ElementLine [-6250 10820 6250 10820 100]
	ElementLine [6250 10820 12500 0 100]
	ElementLine [12500 0 6250 -10820 100]
	ElementLine [6250 -10820 -6250 -10820 100]
	ElementLine [-6250 -10820 -12500 0 100]
	ElementArc [0 0 6500 6500 0 360 100]
	ElementArc [0 0 12500 12500 0 360 100]

	)
