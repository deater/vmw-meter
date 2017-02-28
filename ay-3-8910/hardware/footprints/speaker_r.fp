# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#

Element[ "" "Right Speaker" "" "" 0 0 25000 5000 0 150 ""]
(

# 3 Corner Pins
#		X		Y	Thick	Clear	mask	drill
	Pin[   125.00mil   125.00mil 125.00mil 10.00mil 125.00mil 125.00mil "" "" 0x9]
	Pin[   125.00mil  2625.50mil 125.00mil 10.00mil 125.00mil 125.00mil "" "" 0x9]
	Pin[  1062.50mil  2625.50mil 125.00mil 10.00mil 125.00mil 125.00mil "" "" 0x9]

# Outline
	ElementLine[     0.00mil    0.00mil   300.00mil     0.00mil 10.00mil]
	ElementLine[   300.00mil    0.00mil   300.00mil   300.00mil 10.00mil]
	ElementLine[   300.00mil  300.00mil     0.00mil   300.00mil 10.00mil]

	ElementLine[     0.00mil    0.00mil     0.00mil  2750.00mil 10.00mil]
	ElementLine[   300.00mil 2750.00mil   300.00mil  2450.00mil 10.00mil]
	ElementLine[   300.00mil 2450.00mil     0.00mil  2450.00mil 10.00mil]


	ElementLine[     0.00mil 2750.00mil  1187.50mil  2750.00mil 10.00mil]
	ElementLine[  1187.50mil 2750.00mil  1187.50mil  2450.00mil 10.00mil]
	ElementLine[  1187.00mil 2450.00mil   887.00mil  2450.00mil 10.00mil]
	ElementLine[   887.00mil 2450.00mil   887.00mil  2750.00mil 10.00mil]




)
