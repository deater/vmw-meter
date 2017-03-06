# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#		startAngle deltaAngle thickness)  
#		startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#
Element[ "" "RasPi-2" "" "" 0 0 10mm 10mm 0 150 ""]
(
	# Main Outline
	ElementLine[   0.00mm    0.00mm   85.00mm    0.00mm 10.00mil]
	ElementLine[  85.00mm    0.00mm   85.00mm   56.00mm 10.00mil]
	ElementLine[  85.00mm   56.00mm    0.00mm   56.00mm 10.00mil]
	ElementLine[   0.00mm   56.00mm    0.00mm    0.00mm 10.00mil]

	# Power in
	ElementLine[   6.6mm    52.0mm    14.6mm    52.0mm 10.00mil]
	ElementLine[  14.6mm    52.0mm    14.6mm    57.0mm 10.00mil]
	ElementLine[  14.6mm    57.0mm     6.6mm    57.0mm 10.00mil]
	ElementLine[   6.6mm    57.0mm     6.6mm    52.0mm 10.00mil]

	# HDMI
	ElementLine[  24.5mm    46.0mm    39.5mm   46.0mm 10.00mil]
	ElementLine[  39.5mm    46.0mm    39.5mm   57.0mm 10.00mil]
	ElementLine[  39.5mm    57.0mm    24.5mm   57.0mm 10.00mil]
	ElementLine[  24.5mm    57.0mm    24.5mm   46.0mm 10.00mil]

	# USB1
	ElementLine[  70.0mm     2.5mm    87.0mm    2.5mm 10.00mil]
	ElementLine[  87.0mm     2.5mm    87.0mm   15.5mm 10.00mil]
	ElementLine[  87.0mm    15.5mm    70.0mm   15.5mm 10.00mil]
	ElementLine[  70.0mm    15.5mm    70.0mm    2.5mm 10.00mil]

	# USB2
	ElementLine[  70.0mm    20.5mm    87.0mm   20.5mm 10.00mil]
	ElementLine[  87.0mm    20.5mm    87.0mm   33.5mm 10.00mil]
	ElementLine[  87.0mm    33.5mm    70.0mm   33.5mm 10.00mil]
	ElementLine[  70.0mm    33.5mm    70.0mm   20.5mm 10.00mil]

	# Ethernet
	ElementLine[  66.0mm    37.75mm    87.0mm   37.75mm 10.00mil]
	ElementLine[  87.0mm    37.75mm    87.0mm   53.75mm 10.00mil]
	ElementLine[  87.0mm    53.75mm    66.0mm   53.75mm 10.00mil]
	ElementLine[  66.0mm    53.75mm    66.0mm   37.75mm 10.00mil]

	# Upper Left Hole
	#          x          y    thick  clear  mask   drill  name num flags
	Pin [  3.5mm      3.5mm   6.20mm  0.20mm 6.20mm 2.75mm ""   ""  "pin,hole"]

	# Upper Right Hole
	#          x          y    thick  clear  mask   drill  name num flags
	Pin [ 61.5mm      3.5mm   6.20mm  0.20mm 6.20mm 2.75mm ""   ""  "pin,hole"]

	# Lower Left Hole
	#          x          y    thick  clear  mask   drill  name num flags
	Pin [  3.5mm     52.5mm   6.20mm 0.20mm 6.20mm 2.75mm ""   ""  "pin,hole"]

	# Lower Right Hole
	#          x          y    thick  clear  mask   drill  name num flags
	Pin [ 61.5mm     52.5mm   6.20mm 0.20mm 6.20mm 2.75mm ""   ""  "pin,hole"]

	# Header

	#          x          y     thick     clear     mask    drill name num flags
	Pin[ 329.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "1" "pin,square"]
	Pin[ 329.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "2" "pin"]
	Pin[ 429.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "3" "pin"]
	Pin[ 429.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "4" "pin"]
	Pin[ 529.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "5" "pin"]
	Pin[ 529.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "6" "pin"]
	Pin[ 629.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "7" "pin"]
	Pin[ 629.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "8" "pin"]
	Pin[ 729.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil ""  "9" "pin"]
	Pin[ 729.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "10" "pin"]
	Pin[ 829.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "11" "pin"]
	Pin[ 829.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "12" "pin"]
	Pin[ 929.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "13" "pin"]
	Pin[ 929.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "14" "pin"]
	Pin[1029.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "15" "pin"]
	Pin[1029.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "16" "pin"]
	Pin[1129.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "17" "pin"]
	Pin[1129.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "18" "pin"]
	Pin[1229.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "19" "pin"]
	Pin[1229.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "20" "pin"]

	Pin[1329.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "21" "pin"]
	Pin[1329.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "22" "pin"]
	Pin[1429.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "23" "pin"]
	Pin[1429.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "24" "pin"]
	Pin[1529.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "25" "pin"]
	Pin[1529.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "26" "pin"]
	Pin[1629.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "27" "pin"]
	Pin[1629.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "28" "pin"]
	Pin[1729.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "29" "pin"]
	Pin[1729.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "30" "pin"]
	Pin[1829.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "31" "pin"]
	Pin[1829.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "32" "pin"]
	Pin[1929.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "33" "pin"]
	Pin[1929.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "34" "pin"]
	Pin[2029.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "35" "pin"]
	Pin[2029.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "36" "pin"]
	Pin[2129.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "37" "pin"]
	Pin[2129.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "38" "pin"]
	Pin[2229.53mil 187.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "39" "pin"]
	Pin[2229.53mil  87.80mil 60.00mil  20.00mil 70.00mil 38.00mil "" "40" "pin"]

	ElementLine[ 279.53mil   37.80mil  2279.53mil  37.80mil 10.00mil]
	ElementLine[2279.53mil   37.80mil  2279.53mil 237.80mil 10.00mil]
	ElementLine[2279.53mil  237.80mil   279.53mil 237.80mil 10.00mil]
	ElementLine[ 279.53mil  237.80mil   279.53mil  37.80mil 10.00mil]
)

