# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[ "" "14 SEG LED Matrix" "" "" 1000 1000 25000 5000 0 150 ""]
(

# Connectors
	Pin[ 80000 5000 6000 2000 7000 3800 "+" "1" "pin"]
	Pin[ 90000 5000 6000 2000 7000 3800 "+" "2" "pin"]
	Pin[100000 5000 6000 2000 7000 3800 "-" "3" "pin"]
	Pin[110000 5000 6000 2000 7000 3800 "D" "4" "pin"]
	Pin[120000 5000 6000 2000 7000 3800 "C" "5" "square,pin"]

# 4 Corner Pins
	Pin[  7300   7300 8500 0 8500 8500 "" "" 0x9]
	Pin[  7300 105200 8500 0 8500 8500 "" "" 0x9]
	Pin[192700   7300 8500 0 8500 8500 "" "" 0x9]
	Pin[192700 105200 8500 0 8500 8500 "" "" 0x9]

# Outline
	ElementLine[     0      0 200000      0 1000]
	ElementLine[200000      0 200000 112500 1000]
	ElementLine[200000 112500      0 112500 1000]
	ElementLine[     0 112500      0      0 1000]

# Outline of 14-segs

# Display 1
	ElementLine[     0  14600 100000  14600 1000]
	ElementLine[100000  14600 100000  97900 1000]
	ElementLine[100000  97900      0  97900 1000]
	ElementLine[     0  97900      0  14600 1000]
# Display 1a
	ElementLine[  9500  29300  41000  29300 1000]
	ElementLine[ 41000  29300  41000  83300 1000]
	ElementLine[ 41000  83300   9500  83300 1000]
	ElementLine[  9500  83300   9500  29300 1000]
	ElementLine[ 25250  29300  25250  83300 1000]
	ElementLine[  9500  56300  41000  56300 1000]
	ElementLine[  9500  29300  41000  83300 1000]
	ElementLine[ 41000  29300   9500  83300 1000]
	ElementLine[ 44000  83300  46000  83300 1000]
	ElementLine[ 46000  83300  46000  81300 1000]
	ElementLine[ 46000  81300  44000  81300 1000]
	ElementLine[ 44000  81300  44000  83300 1000]
# Display 1b
	ElementLine[ 59500  29300  91000  29300 1000]
	ElementLine[ 91000  29300  91000  83300 1000]
	ElementLine[ 91000  83300  59500  83300 1000]
	ElementLine[ 59500  83300  59500  29300 1000]
	ElementLine[ 75250  29300  75250  83300 1000]
	ElementLine[ 59500  56300  91000  56300 1000]
	ElementLine[ 59500  29300  91000  83300 1000]
	ElementLine[ 91000  29300  59500  83300 1000]
	ElementLine[ 94000  83300  96000  83300 1000]
	ElementLine[ 96000  83300  96000  81300 1000]
	ElementLine[ 96000  81300  94000  81300 1000]
	ElementLine[ 94000  81300  94000  83300 1000]

# Display 2
	ElementLine[100000  14600 200000  14600 1000]
	ElementLine[200000  14600 200000  97900 1000]
	ElementLine[200000  97900 100000  97900 1000]
	ElementLine[100000  97900 100000  14600 1000]
# Display 2a
	ElementLine[109500  29300  141000  29300 1000]
	ElementLine[141000  29300  141000  83300 1000]
	ElementLine[141000  83300  109500  83300 1000]
	ElementLine[109500  83300  109500  29300 1000]
	ElementLine[125250  29300  125250  83300 1000]
	ElementLine[109500  56300  141000  56300 1000]
	ElementLine[109500  29300  141000  83300 1000]
	ElementLine[141000  29300  109500  83300 1000]
	ElementLine[144000  83300  146000  83300 1000]
	ElementLine[146000  83300  146000  81300 1000]
	ElementLine[146000  81300  144000  81300 1000]
	ElementLine[144000  81300  144000  83300 1000]
# Display 2b
	ElementLine[159500  29300  191000  29300 1000]
	ElementLine[191000  29300  191000  83300 1000]
	ElementLine[191000  83300  159500  83300 1000]
	ElementLine[159500  83300  159500  29300 1000]
	ElementLine[175250  29300  175250  83300 1000]
	ElementLine[159500  56300  191000  56300 1000]
	ElementLine[159500  29300  191000  83300 1000]
	ElementLine[191000  29300  159500  83300 1000]
	ElementLine[194000  83300  196000  83300 1000]
	ElementLine[196000  83300  196000  81300 1000]
	ElementLine[196000  81300  194000  81300 1000]
	ElementLine[194000  81300  194000  83300 1000]
)
