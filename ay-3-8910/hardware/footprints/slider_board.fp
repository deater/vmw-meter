# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

# VMW Slider Board Mk1

Element[ "" "SlideBoard" "" "" 1000 1000 10000 5000 0 150 ""]
(

	#         x          y thick     clear mask      drill   name number flags
	Pin[  0.0mm  -1040.0mil 128.50mil 0.5mm 128.50mil 128.50mil "" "" "pin,hole"]
	Pin[  0.0mm   1040.0mil 128.50mil 0.5mm 128.50mil 128.50mil "" "" "pin,hole"]

	Pin[ -50.00mil 800.00mil  60.00mil 30.00mil 66.00mil 38.00mil "1" "1" "pin,square"]
	Pin[  50.00mil 800.00mil  60.00mil 30.00mil 66.00mil 38.00mil "2" "2" "pin"]

	Pin[ -50.00mil -800.00mil  60.00mil 30.00mil 66.00mil 38.00mil "3" "3" "pin,square"]
	Pin[  50.00mil -800.00mil  60.00mil 30.00mil 66.00mil 38.00mil "4" "4" "pin"]


#	Pin[ -17.5mm  -1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "1" "1" "pin"]
#	Pin[ -17.5mm   1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "2" "2" "pin"]
#	Pin[  17.5mm   1.875mm 2.0mm 0.5mm 2.1mm 1.5mm "3" "3" "pin"]


	# PCB
	ElementLine[  -6.4mm  -1170mil  -8.0mm  -1170mil 10.00mil]
	ElementLine[  -8.0mm  -1170mil  -8.0mm   1170mil 10.00mil]
	ElementLine[  -8.0mm   1170mil  -6.4mm   1170mil 10.00mil]
	ElementLine[  -6.4mm   1170mil  -6.4mm  -1170mil 10.00mil]

	# Top V
	ElementLine[  -6.4mm  -1170mil  -6.4mm   -910mil 10.00mil]
	ElementLine[  -6.4mm   -910mil  -4.83mm  -910mil 10.00mil]
	ElementLine[  -4.83mm  -910mil  -4.83mm -1170mil 10.00mil]
	ElementLine[  -4.83mm -1170mil  -6.4mm  -1170mil 10.00mil]
	
	# Bottom V
	ElementLine[  -6.4mm   1170mil  -6.4mm    910mil 10.00mil]
	ElementLine[  -6.4mm    910mil  -4.83mm   910mil 10.00mil]
	ElementLine[  -4.83mm   910mil  -4.83mm  1170mil 10.00mil]
	ElementLine[  -4.83mm  1170mil  -6.4mm   1170mil 10.00mil]

	# Top Rect
	ElementLine[  -6.4mm -1170mil  -6.4mm  -910mil 10.00mil]
	ElementLine[  -6.4mm  -910mil   3.1mm  -910mil 10.00mil]
	ElementLine[   3.1mm  -910mil   3.1mm -1170mil 10.00mil]
	ElementLine[   3.1mm -1170mil  -6.4mm -1170mil 10.00mil]

	# Bottom Rect
	ElementLine[  -6.4mm  1170mil  -6.4mm   910mil 10.00mil]
	ElementLine[  -6.4mm   910mil   3.1mm   910mil 10.00mil]
	ElementLine[   3.1mm   910mil   3.1mm  1170mil 10.00mil]
	ElementLine[   3.1mm  1170mil  -6.4mm  1170mil 10.00mil]


	# Slider outline
	ElementLine[  -8.0mm  -22.5mm -15.0mm  -22.5mm 10.00mil]
	ElementLine[ -15.0mm  -22.5mm -15.0mm   22.5mm 10.00mil]
	ElementLine[ -15.0mm   22.5mm  -8.0mm   22.5mm 10.00mil]
	ElementLine[  -8.0mm   22.5mm  -8.0mm  -22.5mm 10.00mil]

)
