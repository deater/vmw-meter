# Element( flags description blank blank mark_x mark_y text_x text_y 
#		text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#		startAngle deltaAngle thickness)  
#		startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#
Element[ "" "RasPi-2" "" "" 1000 1000 90000 1000 3 150 ""]
(
	# Main Outline
	ElementLine[     0      0 334645      0 1000]
	ElementLine[334645      0 334645 220472 1000]
	ElementLine[334645 220472      0 220472 1000]
	ElementLine[     0 220472      0      0 1000]

	# Power in
	ElementLine[277165      0 277165  19685 1000]
	ElementLine[277165  19685 308661  19685 1000]
	ElementLine[308661  19685 308661      0 1000]

	# Lower Left Hole
	ElementArc[ 92520 206693 4921 4921 0 360 1000]

	# Upper Left Hole
	ElementArc[ 92520  13780 4921 4921 0 360 1000]

	# Lower Right Hole
	ElementArc[320866 206693 4921 4921 0 360 1000]

	# Upper Right Hole
	ElementArc[320866  13780 4921 4921 0 360 1000]
)
