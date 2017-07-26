# This file goes in the BIN_ROOT/user directory
# BIN_ROOT could be /usr/share/weewx or /home/weewx depending on your install method
# See http://www.weewx.com/docs/usersguide.htm#Where_to_find_things

# WIP

import weewx
from weewx.engine import StdService

class AddData(StdService):

    def __init__(self, engine, config_dict):
      # Initialize my superclass first:
      super(AddData, self).__init__(engine, config_dict)
      # Bind to any new archive record events:
      self.bind(weewx.NEW_ARCHIVE_RECORD, self.new_archive_packet)

    def new_archive_packet(self, event):
        event.record['username'] = 'acurite'
        event.record['latitude'] = 43.2573141
        event.record['latitude'] = -79.8634597

#
#    Copyright (c) 2009-2015 Tom Keffer <tkeffer@gmail.com>
#
#    See the file LICENSE.txt for your full rights.
#
"""The wview schema, which is also used by weewx."""

# =============================================================================
# This list contains the schema for the Beasley weather station, based on 
# the wview schema used by weewx.
# =============================================================================

beasley_schema = [('dateTime',             'INTEGER NOT NULL UNIQUE PRIMARY KEY'),
                  ('usUnits',              'INTEGER NOT NULL'),
                  ('interval',             'INTEGER NOT NULL'),
                  ('username',             'TEXT NOT NULL'),
                  ('latitude',             'REAL'),
                  ('longitude',            'REAL'),
                  ('barometer',            'REAL'),
                  ('pressure',             'REAL'),
                  ('altimeter',            'REAL'),
                  ('inTemp',               'REAL'),
                  ('outTemp',              'REAL'),
                  ('inHumidity',           'REAL'),
                  ('outHumidity',          'REAL'),
                  ('windSpeed',            'REAL'),
                  ('windDir',              'REAL'),
                  ('rainRate',             'REAL'),
                  ('rain',                 'REAL'),
                  ('dewpoint',             'REAL'),
                  ('windchill',            'REAL'),
                  ('heatindex',            'REAL')]