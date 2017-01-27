#!/bin/python

# Purpose: read calib.txt and UT.txt from conTemp.sh and do math on them to get a temperature based on the datasheet:
#          pg 15 https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

import binascii

def main():
	print "hi"
	f = open('calib.txt', 'r')
	f2 = open('UT.txt', 'r')

	# initialize
	AC1 = 0x00 # each of these has 2 lines in the file calib.txt
	AC2 = 0x00
	AC3 = 0x00
	AC4 = 0x00
	AC5 = 0x00  # count 9 and 10
	AC6 = 0x00  # count 11 and 12
	B1 = 0x00
	B2 = 0x00
	MB = 0x00
	MC = 0x00   # count 19 and 20
	MD = 0x00   # count 21 and 22

	UT = 0x00   # count 1 and 2

	# calib
	count = 0
	for line in f:
		
		count = count + 1
		#print line,

		if count == 9:
			AC5 = int(line, 16) << 8
		if count == 10:
			AC5 = AC5 + int(line, 16)

		if count == 11:
			AC6 = int(line, 16) << 8
		if count == 12:
			AC6 = AC6 + int(line, 16)

		if count == 19:
			MC = int(line, 16) << 8
		if count == 20:
			MC = MC + int(line, 16)

		if count == 21:
			MD = int(line, 16) << 8
		if count == 22:
			MD = MD + int(line, 16)

	# temp
	count = 0
	for line in f2:
		
		count = count + 1

		if count == 1:
			print line,
			UT = int(line, 16) << 8
		if count == 2:
			print line,
			UT = UT + int(line, 16)

	if MC > 0x7FFF:
		MC = MC - 0x10000

	print "AC5 " + str(AC5)
	print "AC6 " + str(AC6) 
	print "MC " + str(MC)
	print "MD " + str(MD)
	print "UT " + str(UT)

	# turn UT into t
	x1 = ((UT - AC6) * AC5) / pow(2, 15)
	x2 = (MC * pow(2, 11)) / (x1 + MD)
	b5 = x1 + x2
	t = (b5 + 8) / pow(2, 4)

	print ""
	print "X1"
	print x1

	print ""
	print "X2"
	print x2

	print ""
	print "B5"
	print b5

	print ""
	print "T"
	print t

	print ""
	print "Temperature is [" + str(t/10.0) + "] degrees Celsius"

main()