# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#		startAngle deltaAngle thickness)  
#		startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#
Element[ "" "MAX98306-BREAKOUT" "" "" 0 0 0mil 0mil 0 150 ""]
(
	# Main Outline
	ElementLine[   0.00mil    0.00mil   950.00mil    0.00mil 10.00mil]
	ElementLine[ 950.00mil    0.00mil   950.00mil 1100.00mil 10.00mil]
	ElementLine[ 950.00mil 1100.00mil     0.00mil 1100.00mil 10.00mil]
	ElementLine[   0.00mil 1100.00mil     0.00mil    0.00mil 10.00mil]

	# Output Outline
	ElementLine[ 950.00mil  550.00mil   700.00mil  550.00mil 10.00mil]
	ElementLine[ 700.00mil  550.00mil   700.00mil  300.00mil 10.00mil]
	ElementLine[ 700.00mil  300.00mil   950.00mil  300.00mil 10.00mil]
	ElementLine[ 950.00mil  300.00mil   950.00mil  800.00mil 10.00mil]
	ElementLine[ 950.00mil  800.00mil   700.00mil  800.00mil 10.00mil]
	ElementLine[ 700.00mil  800.00mil   700.00mil  550.00mil 10.00mil]

	# Mounting Holes
	#          x       y  thick  clear   mask  drill  name num flags
	Pin [ 855mil   95mil 100mil  20mil 100mil  100mil  ""   ""  "pin,hole"]
	Pin [ 855mil 1005mil 100mil  20mil 100mil  100mil  ""   ""  "pin,hole"]

	# Header
	#          x       y  thick  clear   mask  drill  name num flags
	Pin [ 100mil  150mil  65mil  20mil  70mil  38mil   ""  "9" "pin"]
	Pin [ 100mil  250mil  65mil  20mil  70mil  38mil   ""  "8" "pin"]
	Pin [ 100mil  350mil  65mil  20mil  70mil  38mil   ""  "7" "pin"]
	Pin [ 100mil  450mil  65mil  20mil  70mil  38mil   ""  "6" "pin"]
	Pin [ 100mil  550mil  65mil  20mil  70mil  38mil   ""  "5" "pin"]
	Pin [ 100mil  650mil  65mil  20mil  70mil  38mil   ""  "4" "pin"]
	Pin [ 100mil  750mil  65mil  20mil  70mil  38mil   ""  "3" "pin"]
	Pin [ 100mil  850mil  65mil  20mil  70mil  38mil   ""  "2" "pin"]
	Pin [ 100mil  950mil  65mil  20mil  70mil  38mil   ""  "1" "pin,square"]




)

