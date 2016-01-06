# Element( flags description blank blank mark_x mark_y text_x text_y text_direction text_scale text_flags
# Pin( x y thickness clearance mask drillhole name number flags
# ElementLine( x1 y1 x2 y2 thickness)
# ElementArc( centre_x centre_y radius_x radius_y startAngle deltaAngle thickness)  startAngle in degs, 0=East, +ve deltaAngle anti-clockwise
# Measurements are taken from the datasheet
#
Element[0x0 "VMW-LOGO-small" "" "" 0 0 -15000 -5000 0 100 0x0]
(
   ElementLine[-15000 -5000 -10000 5000 1000]
   ElementLine[-10000 5000 10000 5000 1000]
   ElementLine[10000 5000 15000 -5000 1000]
   ElementLine[15000 -5000 -15000 -5000 1000]

   ElementLine[-10000 5000 -5000 -5000 1000]
   ElementLine[-5000 -5000 0 5000 1000]
   ElementLine[0 5000 5000 -5000 1000]
   ElementLine[5000 -5000 10000 5000 1000]
)
