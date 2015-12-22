# Element[ sflags "desc" "name" "value" mark_x mark_y text_x text_y 
#     text_direction text_scale text_sflags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "A" "USB" "0" 1000 1000   0   5000 0 100 ""]
(

	Pin[ -13750 0 5000  2000 5000 3600 "" "1" "pin"]
	Pin[  -3950 0 5000  2000 5000 3600 "" "2" "pin"]
	Pin[   3950 0 5000  2000 5000 3600 "" "3" "pin"]
	Pin[  13750 0 5000  2000 5000 3600 "" "4" "pin"]

	Pin[ -25850  10700 10500  2000 10500 9100 "" "" "pin"]
	Pin[  25850  10700 10500  2000 10500 9100 "" "" "pin"]

	ElementLine[-28550 -3300  28550 -3300 1000]
	ElementLine[-28550 -3300 -28550  51200 1000]
	ElementLine[ 28550 -3300  28550  51200 1000]
	ElementLine[-28550  51200  28550  51200 1000]
)
