#!/bin/bash

# this scripts turns any BeagleBoneGreen running _________
# connected into the internet into the 
# "Beasley Weather Station Embedded Device" which pulls database from
# an Acurite weather station and pushes to a database

BBG_SCRIPTS_DIR=/home/badger/beasley-weather-station/bbg/scripts
BBG_USER=badger
SERVER_USER=badger

# exit at the first failure
set -e

echo -e "\n"

while true; do
    read -p "Do you wish to setup BeagleBone as Beasley Weather Staion? [y/n]" yn
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
sudo apt-get install iwatch

echo -e "\nInstall weewx dependencies..."
sudo apt-get install python-configobj python-cheetah python-serial python-usb

# get weewx from the internet and install it
echo -e "\nGoing to download and install weewx, but only if I don't find it in your downloads directory..."
if [ ! -f ~/Downloads/weewx_3.6.2-1_all.deb ];
	then
	wget http://weewx.com/downloads/weewx_3.6.2-1_all.deb
	mv weewx_3.6.2-1_all.deb ~/Downloads/weewx_3.6.2-1_all.deb 
	sudo dpkg -i ~/Downloads/weewx_3.6.2-1_all.deb 
fi

# Configure and run weewx
# http://weewx.com/docs/usersguide.htm#Running_as_a_daemon 
# Uncomment the following:
#cp util/init.d/weewx.debian /etc/init.d/weewx
#chmod +x /etc/init.d/weewx
#update-rc.d weewx defaults 98

# start process that will push database file to the server every hour
./startEnableSystemDTimer.sh


