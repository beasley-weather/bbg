echo BB-ADC:00A0 > /sys/devices/platform/bone_capemgr/slots
watch -n 0.5 cat /sys/bus/iio/devices/iio\:device0/in_voltage0_raw
