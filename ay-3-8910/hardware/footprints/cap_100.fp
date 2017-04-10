# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "capacitor_100" "" "" 1.0828mm 1.0828mm 245.00mil 45.00mil 0 100 ""]
(

#       Pin(     x      y thick    clear    mask     drill    name number flags
	Pin[0.0000 0.0000 55.00mil 30.00mil 61.00mil 30.00mil "" "1" "square"]
	Pin[100.00mil 0.0000 55.00mil 30.00mil 61.00mil 30.00mil "" "2" ""]

	ElementLine [0.0000   0.0000   40.00mil 0.0000 10.00mil]
	ElementLine [40.00mil -15.00mil  40.00mil 15.0mil 10.00mil]

	ElementLine [60.00mil   0.0000  100.00mil  0.0000 10.00mil]
	ElementLine [60.00mil -15.00mil  60.00mil 15.0mil 10.00mil]

	)
