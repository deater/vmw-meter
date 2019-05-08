#!/bin/sh

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
echo "Testing DYA_DOTD_PT2"
../pt3_to_ym5 dya_dotd_pt2.pt3 dya_dotd_pt2.ym > /dev/null
../../conversion_tools/ym_to_ym5 dya_dotd_pt2 > /dev/null
cmp dya_dotd_pt2.ym5 dya_dotd_pt2_good.ym5

# VC_IFR
echo "Testing VC_IFR"
../pt3_to_ym5 vc_ifr.pt3 vc_ifr.ym > /dev/null
../../conversion_tools/ym_to_ym5 vc_ifr > /dev/null
cmp vc_ifr.ym5 vc_ifr_good.ym5

