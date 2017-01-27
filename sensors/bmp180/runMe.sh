#!/bin/bash

while :
do
	./getCalib.sh
	python calcTemp.py
	sleep 1
done