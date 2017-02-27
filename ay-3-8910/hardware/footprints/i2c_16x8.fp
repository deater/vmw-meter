# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[ "" "8x16 LED Matrix" "" "" 1000 1000 25000 5000 0 150 ""]
(

# Connectors
	Pin[ 47500 10000 6000 2000 7000 3800 "+" "1" "square,pin"]
	Pin[ 57500 10000 6000 2000 7000 3800 "-" "2" "pin"]
	Pin[ 67500 10000 6000 2000 7000 3800 "D" "3" "pin"]
	Pin[ 77500 10000 6000 2000 7000 3800 "C" "4" "pin"]

# 4 Corner Pins
	Pin[  7300  10300 8500 0 8500 8500 "" "" 0x9]
	Pin[117700  10300 8500 0 8500 8500 "" "" 0x9]
	Pin[  7300 280200 8500 0 8500 8500 "" "" 0x9]
	Pin[117700 280200 8500 0 8500 8500 "" "" 0x9]


# Outline
	ElementLine[     0      0 125000      0 1000]
	ElementLine[125000      0 125000 287500 1000]
	ElementLine[125000 287500      0 287500 1000]
	ElementLine[     0 287500      0      0 1000]

# Outline of 8x8

# Display 1
	ElementLine[     0  18750 125000  18750 1000]
	ElementLine[125000  18750 125000 143750 1000]
	ElementLine[125000 143750      0 143750 1000]
	ElementLine[     0 143750      0  18750 1000]
# Display 2
	ElementLine[     0 143750 125000 143750 1000]
	ElementLine[125000 143750 125000 268750 1000]
	ElementLine[125000 268750      0 268750 1000]
	ElementLine[     0 268750      0 143750 1000]

)
