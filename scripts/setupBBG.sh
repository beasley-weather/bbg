#!/bin/bash

# this scripts turns any BeagleBoneGreen running _________
# connected into the internet into the 
# "Beasley Weather Station Embedded Device" which pulls database from
# an Acurite weather station and pushes to a database

BBG_USER=badger
SERVER_USER=badger
BBG_SCRIPTS_DIR=/home/$USER/beasley-weather-station/bbg/scripts
BBG_LOG_DIR=/home/$USER/beasley-weather-station/bbg/log
BBG_DB_DIR=/home/$USER/beasley-weather-station/bbg/db
BBG_LOG_FILE_NAME=bbg.log
BBG_LOG=$BBG_LOG_DIR/$BBG_LOG_FILE_NAME

# exit at the first failure
set -e

echo -e "\n"

while true; do
    read -p "Do you wish to setup BeagleBone as Beasley Weather Station? [y/n]" yn
    case $yn in
        [Yy]* )
			echo "Continuing with BB setup..." 
			break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

cd $BBG_SCRIPTS_DIR

echo -e "\nInstall utilities..."
wget http://ftp.debian.org/debian/pool/main/i/iwatch/iwatch_0.2.2-2_all.deb
sudo dpkg -i iwatch_0.2.2-2_all.deb

echo -e "\nInstall weewx dependencies..."
sudo apt-get update
sudo apt-get install python-configobj python-cheetah python-serial python-usb

# get weewx from the internet and install it
echo -e "\nGoing to download and install weewx, but only if I don't find it in your downloads directory..."
if [ ! -f ~/Downloads/weewx_3.6.2-1_all.deb ];
	then
	wget http://weewx.com/downloads/released_versions/weewx_3.6.2-1_all.deb
	mv -v weewx_3.6.2-1_all.deb ~/Downloads/weewx_3.6.2-1_all.deb 
	sudo dpkg -i ~/Downloads/weewx_3.6.2-1_all.deb 
fi

# Configure and run weewx
# http://weewx.com/docs/usersguide.htm#Running_as_a_daemon 
# "If you use a packaged install from a DEB or RPM distribution, this is done automatically. You can ignore this section."
# Uncomment the following:
#cp util/init.d/weewx.debian /etc/init.d/weewx
#chmod +x /etc/init.d/weewx
#update-rc.d weewx defaults 98

# if the beaglebone green log file is not created, create it now
echo -e "\nChecking to see if bbg log file exists..."
if [[ ! -f $BBG_LOG ]]; then
	echo -e "\nCreate bbg log file [" $BBG_LOG "]"
	mkdir -p $BBG_LOG_DIR
	touch $BBG_LOG
else
	echo "...log file exists."
fi

# start process that will push database file to the server every hour
./startEnableSystemDTimer.sh


