# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "diode" "" "" 1.1465mm 11.3065mm 56.00mil -138.00mil 0 100 ""]
(
	Pin[0.0000    0.00mil 60.00mil 30.00mil 60.00mil 28.50mil "" "1" ""]
	Pin[0.0000 -300.00mil 60.00mil 30.00mil 60.00mil 28.50mil "" "2" ""]

	ElementLine [  0.00mil  -50.00mil   0.00mil  -90.00mil 10.00mil]
	ElementLine [ 40.00mil  -90.00mil -40.00mil  -90.00mil 10.00mil]
	ElementLine [-40.00mil  -90.00mil -40.00mil -210.00mil 10.00mil]
	ElementLine [-40.00mil -210.00mil  40.00mil -210.00mil 10.00mil]
	ElementLine [ 40.00mil -210.00mil  40.00mil  -90.00mil 10.00mil]
	ElementLine [  0.00mil -210.00mil   0.00mil -250.00mil 10.00mil]
# Stripe
	ElementLine [ 40.00mil  -110.00mil -40.00mil  -110.00mil 10.00mil]


	)
