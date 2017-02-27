# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[ "" "Switch" "" "" 1000 1000 10000 5000 0 150 ""]
(
	Pin[ -6.25mm -2.5mm 2.0mm 0.5mm 2.4mm 1.2mm "1" "1" "pin"]
	Pin[  6.25mm -2.5mm 2.0mm 0.5mm 2.4mm 1.2mm "2" "2" "pin"]
	Pin[ -6.25mm  2.5mm 2.0mm 0.5mm 2.4mm 1.2mm "3" "3" "pin"]
	Pin[  6.25mm  2.5mm 2.0mm 0.5mm 2.4mm 1.2mm "4" "4" "pin"]

	Pin[     0mm  -4.5mm 1.8mm 0mm 1.8mm 1.8mm "" "" 0x9]
	Pin[     0mm   4.5mm 1.8mm 0mm 1.8mm 1.8mm "" "" 0x9]

	ElementLine[ -6mm  -6mm   6mm  -6mm 1000]
	ElementLine[  6mm  -6mm   6mm   6mm 1000]
	ElementLine[  6mm   6mm  -6mm   6mm 1000]
	ElementLine[ -6mm   6mm  -6mm  -6mm 1000]

	ElementLine[ -5mm  2.5mm    5mm  2.5mm 1000]
	ElementLine[ -5mm -2.5mm    5mm -2.5mm 1000]

	ElementLine[  0mm -2.5mm    0mm -1.5mm 1000]
	ElementLine[  0mm  2.5mm    0mm  1.5mm 1000]

	ElementLine[  0mm  1.5mm    2mm  -1mm 1000]



)
