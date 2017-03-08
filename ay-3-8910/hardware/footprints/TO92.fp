	Element(0x00 "Transistor" "" "TO92" 60 70 0 100 0x00)
(
# The JEDEC drawing shows a pin diameter of 16-21 mils
#
#
#         _______
# TO92:  | 1 2 3 |   <-- bottom view
#         \_____/
#       
# The pin to pin spacing is 100 mils.
	Pin(250 200 72 42 "1" 0x101)
	Pin(150 200 72 42 "2" 0x01)
	Pin(50 200 72 42 "3" 0x01)
	ElementArc(150 200 100 100 315 270 10)
	ElementLine( 80 130 220 130 10)
	Mark(50 200)
)
