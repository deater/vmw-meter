# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#		startAngle deltaAngle thickness)  
#		startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#
Element[ "" "jack" "" "" 0 0 -5mm -5mm 0 150 ""]
(
	# Main Outline
	ElementLine[   2.50mm   -6.00mm   -8.50mm   -6.00mm 10.00mil]
	ElementLine[  -8.50mm   -6.00mm   -8.50mm   -5.00mm 10.00mil]
	ElementLine[  -8.50mm   -5.00mm   -9.50mm   -5.00mm 10.00mil]
	ElementLine[  -9.50mm   -5.00mm   -9.50mm   -2.00mm 10.00mil]
	ElementLine[  -9.50mm   -2.00mm   -8.50mm   -2.00mm 10.00mil]
	ElementLine[  -8.50mm   -2.00mm   -8.50mm    2.00mm 10.00mil]
	ElementLine[  -8.50mm    2.00mm   -9.50mm    2.00mm 10.00mil]
	ElementLine[  -9.50mm    2.00mm   -9.50mm    5.00mm 10.00mil]
	ElementLine[  -9.50mm    5.00mm   -8.50mm    5.00mm 10.00mil]
	ElementLine[  -8.50mm    5.00mm   -8.50mm    6.00mm 10.00mil]
	ElementLine[  -8.50mm    6.00mm    2.50mm    6.00mm 10.00mil]
	ElementLine[   2.50mm    6.00mm    2.50mm   -6.00mm 10.00mil]

	# Jack Outline
	ElementLine[  -9.50mm   -2.00mm  -11.50mm   -2.00mm 10.00mil]
	ElementLine[ -11.50mm   -2.00mm  -11.50mm    2.00mm 10.00mil]
	ElementLine[ -11.50mm    2.00mm   -9.50mm    2.00mm 10.00mil]


	# Positioning Holes
	#           x        y   thick  clear  mask   drill  name num flags
	Pin [   0.0mm    0.0mm   1.2mm  0.60mm 1.2mm  1.2mm  ""   ""  "pin,hole"]
	Pin [  -7.5mm   -5.0mm   1.2mm  0.60mm 1.2mm  1.2mm  ""   ""  "pin,hole"]
	Pin [  -5.0mm   -5.0mm   1.2mm  0.60mm 1.2mm  1.2mm  ""   ""  "pin,hole"]
	Pin [  -7.5mm    5.0mm   1.2mm  0.60mm 1.2mm  1.2mm  ""   ""  "pin,hole"]
	Pin [  -5.0mm    5.0mm   1.2mm  0.60mm 1.2mm  1.2mm  ""   ""  "pin,hole"]

	# Useful Holes
	#           x        y   thick  clear  mask   drill  name num flags
	Pin [  -5.0mm    0.0mm   2.1mm  0.60mm 2.2mm  1.5mm  ""   "1"  "pin"]
	Pin [   0.0mm    5.0mm   2.1mm  0.60mm 2.2mm  1.5mm  ""   "2"  "pin"]
	Pin [   0.0mm   -5.0mm   2.1mm  0.60mm 2.2mm  1.5mm  ""   "3"  "pin"]
	Pin [   0.0mm    2.5mm   2.1mm  0.60mm 2.2mm  1.5mm  ""   "4"  "pin"]
	Pin [   0.0mm   -2.5mm   2.1mm  0.60mm 2.2mm  1.5mm  ""   "5"  "pin"]

)

