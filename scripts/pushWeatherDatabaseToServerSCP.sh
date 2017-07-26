#!/bin/bash

USER=badger
BBG_LOG_DIR=/home/$USER/beasley-weather-station/bbg/log
BBG_DB_DIR=/home/$USER/beasley-weather-station/bbg/db
BBG_LOG_FILE_NAME=bbg.log
BBG_LOG=$BBG_LOG_DIR/$BBG_LOG_FILE_NAME
DATABASE_FILE_TO_PUSH=newPush.db
DATABASE_PUSH_FILE_LOCATION_ON_SERVER=/home/$USER/beasley-weather-station/server/db
BEASLEY_SERVER_DOMAIN_NAME=beasley-weather-station-server.com

echo `date` ": Begin push weather database to server." >> $BBG_LOG

scp -v $BBG_DB_DIR/$DATABASE_FILE_TO_PUSH $USER@$BEASLEY_SERVER_DOMAIN_NAME:$DATABASE_PUSH_FILE_LOCATION_ON_SERVER >> $BBG_LOG

echo `date` ": End push weather database to server." >> $BBG_LOG