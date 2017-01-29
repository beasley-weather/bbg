#!/bin/bash

# Purpose: get current time from the Real-time clock breakout board with the DS3231SN rtc chip
#          pg 11 https://datasheets.maximintegrated.com/en/ds/DS3231.pdf

# start the I2C config for beaglebone green
export SLOTS=/sys/devices/platform/bone_capemgr/slots
echo BB-I2C2:0A00 > $SLOTS
#cat $SLOTS

#echo "Check posible i2c interfaces..."
#i2cdetect -l

#echo "Find attached I2C devices..."
#i2cdetect -y -r 2

# get time from each register
# NOTE: the return value is in hex, but it can be interpreted as decimal
SECOND=`i2cget -y 2 0x68 0x00 b`
MINUTE=`i2cget -y 2 0x68 0x01 b`
HOUR=`i2cget -y 2 0x68 0x02 b`
YEAR=`i2cget -y 2 0x68 0x06 b` # 0x17 = 2017

# remove "0x" from the front of all the i2c gets
SECOND=${SECOND:2}
MINUTE=${MINUTE:2}
HOUR=${HOUR:2}
YEAR=20${YEAR:2} # add 2000 to the i2cget value

# construct $DATE
DATE="H:$HOUR M:$MINUTE S:$SECOND Y:$YEAR"

# user compare real date to the date from the RTC
echo -e Real Date: `date`
echo -e RTC  Date: $DATE
