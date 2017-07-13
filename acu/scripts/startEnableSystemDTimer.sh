#!/bin/bash

echo -e "\nAbout to move necessary files to their respective locations on the BeagleBone..."
sudo cp -v pushWeatherDatabaseToServerSCP.sh /usr/local/bin/pushWeatherDatabaseToServerSCP.sh
sudo cp -v pushWeatherDatabaseToServer.timer /etc/systemd/system/pushWeatherDatabaseToServer.timer
sudo cp -v pushWeatherDatabaseToServer.service /etc/systemd/system/pushWeatherDatabaseToServer.service

# enable the timer, from now on the server should see 
echo -e "\nAbout to enable systemd timer that will run the pushWeatherDatabaseToServerSCP.sh script..."
sudo systemctl status pushWeatherDatabaseToServer.timer # good for human-readable output
sudo systemctl enable pushWeatherDatabaseToServer.timer
systemctl daemon-reload

echo -e "\nShowing script status..."
sudo systemctl status pushWeatherDatabaseToServer.timer # good for human-readable output

echo -e "\nDone enabling the service. Wait one minute for the first database push!"
