#!/bin/bash

# Purpose: get calib and UT data from I2C sensor and put into two file (calib.txt and UT.txt) 
#          so that temp.py can do math on them to get a temperature based on the datasheet:
#          pg 15 https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

# start the I2C config for beaglebone green
export SLOTS=/sys/devices/platform/bone_capemgr/slots
echo BB-I2C2:0A00 > $SLOTS
cat $SLOTS

echo "Check posible i2c interfaces"
i2cdetect -l

echo "Check that the start command bits are 0x80 0x00. This proves we are connected and reset."
i2cdump -y -r 0xf6-0xf7 2 0x77 b

# Get calibration parameters
# A
AC1_MSB=`i2cget -y 2 0x77 0xaa b`
AC1_LSB=`i2cget -y 2 0x77 0xab b`

AC2_MSB=`i2cget -y 2 0x77 0xac b`
AC2_LSB=`i2cget -y 2 0x77 0xad b`

AC3_MSB=`i2cget -y 2 0x77 0xae b`
AC3_LSB=`i2cget -y 2 0x77 0xaf b`

AC4_MSB=`i2cget -y 2 0x77 0xb0 b`
AC4_LSB=`i2cget -y 2 0x77 0xb1 b`

AC5_MSB=`i2cget -y 2 0x77 0xb2 b`
AC5_LSB=`i2cget -y 2 0x77 0xb3 b`

AC6_MSB=`i2cget -y 2 0x77 0xb4 b`
AC6_LSB=`i2cget -y 2 0x77 0xb5 b`

# B
B1_MSB=`i2cget -y 2 0x77 0xb6 b`
B1_LSB=`i2cget -y 2 0x77 0xb7 b`

B2_MSB=`i2cget -y 2 0x77 0xb8 b`
B2_LSB=`i2cget -y 2 0x77 0xb9 b`

# M
MB_MSB=`i2cget -y 2 0x77 0xba b`
MB_LSB=`i2cget -y 2 0x77 0xbb b`

MC_MSB=`i2cget -y 2 0x77 0xbc b`
MC_LSB=`i2cget -y 2 0x77 0xbd b`

MD_MSB=`i2cget -y 2 0x77 0xbe b`
MD_LSB=`i2cget -y 2 0x77 0xbf b`

F="calib.txt"
echo $AC1_MSB > $F   # OVERWRITE
echo $AC1_LSB >> $F
echo $AC2_MSB >> $F
echo $AC2_LSB >> $F
echo $AC3_MSB >> $F
echo $AC3_LSB >> $F
echo $AC4_MSB >> $F
echo $AC4_LSB >> $F
echo $AC5_MSB >> $F
echo $AC5_LSB >> $F
echo $AC6_MSB >> $F
echo $AC6_LSB >> $F
echo $B1_MSB >> $F
echo $B1_LSB >> $F
echo $B2_MSB >> $F
echo $B2_LSB >> $F
echo $MB_MSB >> $F
echo $MB_LSB >> $F
echo $MC_MSB >> $F
echo $MC_LSB >> $F
echo $MD_MSB >> $F
echo $MD_LSB >> $F

i2cset -y 2 0x77 0xf4 0x2e
sleep 0.01 # only need to wait 4.5ms
UT_MSB=`i2cget -y 2 0x77 0xf6 b`
UT_LSB=`i2cget -y 2 0x77 0xf7 b`

F2="UT.txt"
echo $UT_MSB > $F2   # OVERWRITE
echo $UT_LSB >> $F2
