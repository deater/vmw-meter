# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[ "" "Connector 1x4" "" "" 1000 1000 10000 5000 0 150 ""]
(
	Pin[10000 45000 6000 2000 7000 3800 "1" "1" "square,pin"]
	Pin[20000 45000 6000 2000 7000 3800 "2" "2" "pin"]
	Pin[30000 45000 6000 2000 7000 3800 "3" "3" "pin"]
	Pin[40000 45000 6000 2000 7000 3800 "4" "4" "pin"]

	ElementLine[    0     0 50000     0 1000]
	ElementLine[50000     0 50000 45000 1000]
	ElementLine[50000 45000     0 45000 1000]
	ElementLine[    0 45000     0     0 1000]

)
