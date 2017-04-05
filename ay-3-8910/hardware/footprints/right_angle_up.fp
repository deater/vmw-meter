# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

# Keystone-621 Right Andble Bracket 4-40 hole

Element[ "" "Bracket" "" "" 1000 1000 10000 5000 0 150 ""]
(

	#         x      y     thick clear mask      drill     name number flags
	Pin[  0.0mm  0.0mm 128.50mil 0.5mm 128.50mil 128.50mil "" "" "pin,hole"]

	ElementLine[ -3.2mm  -3.5mm   3.2mm  -3.5mm 1000]
	ElementLine[  3.2mm  -3.5mm   3.2mm   5.2mm 1000]
	ElementLine[  3.2mm   5.2mm  -3.2mm   5.2mm 1000]
	ElementLine[ -3.2mm   5.2mm  -3.2mm  -3.5mm 1000]

)
