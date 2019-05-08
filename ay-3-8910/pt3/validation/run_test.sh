#!/bin/sh

# EA
../pt3_to_ym5 ea.pt3 ea.ym > /dev/null
../../conversion_tools/ym_to_ym5 ea > /dev/null
cmp ea.ym5 ea_good.ym5
