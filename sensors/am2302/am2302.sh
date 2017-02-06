#!/bin/bash

# Purpose: read temp and relative humidity from a sensor found at akizukidenshi.com/download/ds/aosong/AM2302.pdf 
#          by making the beaglebone greens input

# make UART4_RXD=P9_13=$PINS 30 a GPIO0_23
# must have Derek Molloys git: install using "git clone https://github.com/derekmolloy/exploringBB.git"
cd /root/projects/exploringBB/chp06/overlay/
cp EBB-GPIO-Example-00A0.dtbo /lib/firmware/
cd -
cd /lib/firmware/
echo EBB-GPIO-Example > $SLOTS
cat $SLOTS

# verify GPIO0_23 is in mode 0x37 = fast, input, pullup, enabled, mode 7 (gpio mode)
echo "Verify that $PINS 29 is in mode 0x37"
cat $PINS | grep "pin 29"

# connect a wire from DGND on BBB to P9_13, pull it in and out as this executable runs
cd -
make

# todo: buffer the signals from the AM2302 sensor using a program like devMem and decode temp/humidity
