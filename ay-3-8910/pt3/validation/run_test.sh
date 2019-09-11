#!/bin/sh

# HOWTO set one of these up
# Run Ay_Emul, convert to YM6
# manually convert using ../dump
#	../../conversion_tools/dump_ym5 -d RI.ym > RI.ay
# manually convert the output of pt3_to_ym5
#	../pt3_to_ym5 RI.pt3 ri.ym
#	../../conversion_tools/dump_ym5 -d ri.ym5 > RI.ay2
#	diff
# if they match, make it the "correct" output
#	mv ri.ym5 ri_good.ym5


echo

# EA
echo "Testing EA"
../pt3_to_ym5 ea.pt3 ea.ym > /dev/null
../../conversion_tools/ym_to_ym5 ea > /dev/null
cmp ea.ym5 ea_good.ym5

# AR_IT
echo "Testing AR_IT"
../pt3_to_ym5 ar_it.pt3 ar_it.ym > /dev/null
../../conversion_tools/ym_to_ym5 ar_it > /dev/null
cmp ar_it.ym5 ar_it_good.ym5

# MMCM_OS
echo "Testing MMCM_OS"
../pt3_to_ym5 mmcm_os.pt3 mmcm_os.ym > /dev/null
../../conversion_tools/ym_to_ym5 mmcm_os > /dev/null
cmp mmcm_os.ym5 mmcm_os_good.ym5

# MMCM_SR
echo "Testing MMCM_SR"
../pt3_to_ym5 mmcm_sr.pt3 mmcm_sr.ym > /dev/null
../../conversion_tools/ym_to_ym5 mmcm_sr > /dev/null
cmp mmcm_sr.ym5 mmcm_sr_good.ym5

# DASCON_MB
echo "Testing DASCON_MB"
../pt3_to_ym5 dascon_mb.pt3 dascon_mb.ym > /dev/null
../../conversion_tools/ym_to_ym5 dascon_mb > /dev/null
cmp dascon_mb.ym5 dascon_mb_good.ym5

# NOTE: the DOTD are off very slightly on some notes
#       for now hoping it's a version difference on Vortex Tracker 3.7 files
#       and using AYemul29b17 to convert but AYemul30b11 source

# DYA_DOTD_PT1
echo "Testing DYA_DOTD_PT1"
../pt3_to_ym5 dya_dotd_pt1.pt3 dya_dotd_pt1.ym > /dev/null
../../conversion_tools/ym_to_ym5 dya_dotd_pt1 > /dev/null
cmp dya_dotd_pt1.ym5 dya_dotd_pt1_good.ym5

# DYA_DOTD_PT2
# This does wacky stuf like set up slides with 0 length
# and set sample offset to FF
#echo "Testing DYA_DOTD_PT2"
#../pt3_to_ym5 dya_dotd_pt2.pt3 dya_dotd_pt2.ym > /dev/null
#../../conversion_tools/ym_to_ym5 dya_dotd_pt2 > /dev/null
#cmp dya_dotd_pt2.ym5 dya_dotd_pt2_good.ym5

# VC_IFR
echo "Testing VC_IFR"
../pt3_to_ym5 vc_ifr.pt3 vc_ifr.ym > /dev/null
../../conversion_tools/ym_to_ym5 vc_ifr > /dev/null
cmp vc_ifr.ym5 vc_ifr_good.ym5


#BA.PT3  -- 6 channel
#echo "Testing BA.PT3"
#../pt3_to_ym5 BA.PT3 ba.ym > /dev/null
#../../conversion_tools/ym_to_ym5 ba > /dev/null
#cmp ba.ym5 BA_good.ym5


#CR.PT3
echo "Testing CR.PT3"
../pt3_to_ym5 CR.PT3 cr.ym > /dev/null
../../conversion_tools/ym_to_ym5 cr > /dev/null
cmp cr.ym5 CR_good.ym5


#DF.PT3

#BH.PT3
echo "Testing BH.PT3"
../pt3_to_ym5 BH.PT3 bh.ym > /dev/null
../../conversion_tools/ym_to_ym5 bh > /dev/null
cmp bh.ym5 bh_good.ym5

#CH.PT3
echo "Testing CH.PT3"
../pt3_to_ym5 CH.PT3 ch.ym > /dev/null
../../conversion_tools/ym_to_ym5 ch > /dev/null
cmp ch.ym5 ch_good.ym5

#DY.PT3
echo "Testing DY.PT3"
../pt3_to_ym5 DY.PT3 dy.ym > /dev/null
../../conversion_tools/ym_to_ym5 dy > /dev/null
cmp dy.ym5 dy_good.ym5

#F4.PT3
#FC.PT3
#HI.PT3
#I2.PT3
#IT.PT3
#MB.PT3
#OS.PT3

#RI.PT3
echo "Testing RI.PT3"
../pt3_to_ym5 RI.PT3 ri.ym > /dev/null
../../conversion_tools/ym_to_ym5 ri > /dev/null
cmp ri.ym5 RI_good.ym5

#SD.PT3
#SR.PT3

