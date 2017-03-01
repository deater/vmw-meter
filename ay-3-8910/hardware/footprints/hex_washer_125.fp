# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "hex_washer_125" "" "" 0 0 0 0 0 100 ""]
(
#               x y thickness clearance mask      drill	    name number flags
	Pin[	0 0 150.00mil 15.00mil  160.00mil 126.00mil ""   "1"     "pin,hole"]
	ElementLine [-12500 0 -6250 10820 200]
	ElementLine [-6250 10820 6250 10820 200]
	ElementLine [6250 10820 12500 0 200]
	ElementLine [12500 0 6250 -10820 200]
	ElementLine [6250 -10820 -6250 -10820 200]
	ElementLine [-6250 -10820 -12500 0 200]
	ElementArc [0 0 6500 6500 0 360 200]
	ElementArc [0 0 12500 12500 0 360 200]

)
