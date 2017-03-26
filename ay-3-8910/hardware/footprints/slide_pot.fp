# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

# Slide Potentiometer RA3043F-20

Element[ "" "Slide" "" "" 1000 1000 10000 5000 0 150 ""]
(

	#          x         y thick clear mask  drill name number flags
	Pin[ -17.5mm  -1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "1" "1" "pin"]
	Pin[ -17.5mm   1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "2" "2" "pin"]
	Pin[  17.5mm   1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "3" "3" "pin"]

	ElementLine[ -22.5mm  -4.0mm   22.5mm  -4.0mm 1000]
	ElementLine[  22.5mm  -4.0mm   22.5mm   4.0mm 1000]
	ElementLine[  22.5mm   4.0mm  -22.5mm   4.0mm 1000]
	ElementLine[ -22.5mm   4.0mm  -22.5mm  -4.0mm 1000]

	ElementLine[ -15.0mm  -2.0mm   15.0mm  -2.0mm 1000]
	ElementLine[  15.0mm  -2.0mm   15.0mm   2.0mm 1000]
	ElementLine[  15.0mm   2.0mm  -15.0mm   2.0mm 1000]
	ElementLine[ -15.0mm   2.0mm  -15.0mm  -2.0mm 1000]



)
