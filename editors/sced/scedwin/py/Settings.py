# sced (SuperCollider mode for gedit)
#
# Copyright 2012 Jakob Leben
# Copyright 2009 Artem Popov and other contributors (see AUTHORS)
#
#    sced is free software: 
#    you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

from os import path
import simplejson

def locate():
    base = path.expandvars("%APPDATA%")
    if base is not None:
        return path.join(base, "sced.config.json")
    else:
        return None

def load():
    sets = Settings()

    try:
        filename = locate()
        f = open(filename, "r")
    except:
        print "Could not open configuration file: " + str(filename)
        f = None

    data = {}
    if f is not None:
        try:
            data = simplejson.load(f)
        except:
            print "Configuration file not a valid JSON script!"
        f.close()

    sets.sc_dir = data.get("supercollider-dir")
    sets.advanced = data.get("advanced", False)
    lang_data = data.get("interpreter", {})
    sets.sclang_cmd = lang_data.get("command")
    sets.sclang_work_dir = lang_data.get("runtime-dir")

    print "Sced settings loaded."

    return sets

# map gconf options to gobject properties
class Settings(object):
    def __init__(self):
        self.sc_dir = None
        self.advanced = None
        self.sclang_cmd = None
        self.sclang_work_dir = None

    def save(self):
        try:
            path.remove(self.__filename)
        except:
            print "existing"

        f = open(locate(), "w")

        data = {
            "supercollider-dir": self.sc_dir,
            "advanced": self.advanced,
            "interpreter": {
                "command": self.sclang_cmd,
                "runtime-dir": self.sclang_work_dir
            }
        };

        simplejson.dump(data, f, indent="    ")

        f.close()

        print "Sced settings saved."

