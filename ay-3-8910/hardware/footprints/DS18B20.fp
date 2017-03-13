# Element( flags description blank blank mark_x mark_y text_x text_y 
#               text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y 
#               startAngle deltaAngle thickness)  
#               startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
#

Element["" "DS18B20" "" "" 1.2994mm 1.9256mm 10.00mil -130.00mil 0 100 ""]
(
	Pin[200.00mil 0.0000 72.00mil 30.00mil 78.00mil 42.00mil "1" "1" "square,edge2"]
	Pin[100.00mil 0.0000 72.00mil 30.00mil 78.00mil 42.00mil "2" "2" "edge2"]
	Pin[0.0000 0.0000 72.00mil 30.00mil 78.00mil 42.00mil "3" "3" "edge2"]
	ElementLine [30.00mil -70.00mil 170.00mil -70.00mil 10.00mil]
	ElementArc [100.00mil 0.0000 100.00mil 100.00mil 315 270 10.00mil]
)
