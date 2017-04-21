# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

# SPDT slide switch OS102011MA1QN1

Element[ "" "Switch" "" "" 1000 1000 10000 5000 0 150 ""]
(

	#         x      y thick clear mask  drill name number flags
	Pin[ -4.1mm  0.0mm 1.5mm 0.5mm 1.5mm 1.5mm "" "" "pin,hole"]
	Pin[  4.1mm  0.0mm 1.5mm 0.5mm 1.5mm 1.5mm "" "" "pin,hole"]

	Pin[ -2.0mm  0.0mm 1.4mm 0.5mm 1.5mm 0.8mm "1" "1" "pin"]
	Pin[  0.0mm  0.0mm 1.4mm 0.5mm 1.5mm 0.8mm "2" "2" "pin"]
	Pin[  2.0mm  0.0mm 1.4mm 0.5mm 1.5mm 0.8mm "3" "3" "pin"]

	ElementLine[ -4.3mm  -2.2mm   4.3mm  -2.2mm 1000]
	ElementLine[  4.3mm  -2.2mm   4.3mm   2.2mm 1000]
	ElementLine[  4.3mm   2.2mm  -4.3mm   2.2mm 1000]
	ElementLine[ -4.3mm   2.2mm  -4.3mm  -2.2mm 1000]

	ElementLine[ -2mm  2.2mm   -2mm  1.8mm 1000]
	ElementLine[ -2mm  1.8mm    2mm  1.8mm 1000]
	ElementLine[  2mm  1.8mm    2mm  2.2mm 1000]
	ElementLine[  2mm  2.2mm    0mm  2.2mm 1000]
	ElementLine[  0mm  2.2mm    0mm  1.8mm 1000]

)
