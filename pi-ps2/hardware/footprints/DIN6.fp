# Element[ sflags "desc" "name" "value" mark_x mark_y text_x text_y 
#     text_direction text_scale text_sflags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#

Element["" "" "" "" 1000 1000   0   4000 0 100 ""]
(

	Pin[      0  18500  11000  3000 11000 9000 "" "" ""]
	Pin[ -26600  21700  11000  3000 11000 9000 "" "" ""]
	Pin[  26600  21700  11000  3000 11000 9000 "" "" ""]

# Pin numbers as seen from top

	Pin[  -5100  33500   5500  2000 5500 3500 "" "1" "pin"]
	Pin[ -13200  33500   5500  2000 5500 3500 "" "3" "pin"]
	Pin[ -13200  43300   5500  2000 5500 3500 "" "5" "pin"]

	Pin[   5100  33500   5500  2000 5500 3500 "" "2" "pin"]
	Pin[  13200  33500   5500  2000 5500 3500 "" "4" "pin"]
	Pin[  13200  43300   5500  2000 5500 3500 "" "6" "pin"]

	ElementLine[-27550     0  27550     0 1000]
	ElementLine[-27550     0 -27550 50500 1000]
	ElementLine[ 27550     0  27550 50500 1000]
	ElementLine[-27550 50500  27550 50500 1000]
)
