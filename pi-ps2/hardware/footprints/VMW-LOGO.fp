# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[0x0 "VMW-LOGO" "" "" 0 0 -30000 -10000 0 100 0x0]
(
   ElementLine[-30000 -10000 -20000 10000 1000]
   ElementLine[-20000 10000 20000 10000 1000]
   ElementLine[20000 10000 30000 -10000 1000]
   ElementLine[30000 -10000 -30000 -10000 1000]

   ElementLine[-20000 10000 -10000 -10000 1000]
   ElementLine[-10000 -10000 0 10000 1000]
   ElementLine[0 10000 10000 -10000 1000]
   ElementLine[10000 -10000 20000 10000 1000]
)
