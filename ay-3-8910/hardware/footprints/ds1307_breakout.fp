# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#		startAngle deltaAngle thickness)  
#		startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#
Element[ "" "DS1307-BREAKOUT" "" "" 0 0 0mil 0mil 0 150 ""]
(
	# Main Outline
	ElementLine[   0.00mm    0.00mm   30.00mm    0.00mm 10.00mil]
	ElementLine[  30.00mm    0.00mm   30.00mm   24.00mm 10.00mil]
	ElementLine[  30.00mm   24.00mm    0.00mm   24.00mm 10.00mil]
	ElementLine[   0.00mm   24.00mm    0.00mm    0.00mm 10.00mil]

	# Mounting Holes
	#          x       y  thick  clear   mask  drill  name num flags
	Pin [   27mm   2.5mm    3mm    1mm    3mm    3mm  ""   ""  "pin,hole"]
	Pin [   2.5mm 21.5mm    3mm    1mm    3mm    3mm  ""   ""  "pin,hole"]

	# Header
	#       x       y  thick  clear   mask  drill  name num flags
	Pin [ 2mm   80mil  65mil  20mil  70mil  38mil   ""  "5" "pin"]
	Pin [ 2mm  180mil  65mil  20mil  70mil  38mil   ""  "4" "pin"]
	Pin [ 2mm  280mil  65mil  20mil  70mil  38mil   ""  "3" "pin"]
	Pin [ 2mm  380mil  65mil  20mil  70mil  38mil   ""  "2" "pin"]
	Pin [ 2mm  480mil  65mil  20mil  70mil  38mil   ""  "1" "pin,square"]

)

