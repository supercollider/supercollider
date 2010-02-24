# sced (SuperCollider mode for gedit)
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

import gobject
import gconf

# map gconf options to gobject properties
class Settings(gobject.GObject):
    __gproperties__ = {
        "runtime-folder": (gobject.TYPE_STRING,
            "runtime folder",
            "sclang runtime folder",
            None,
            gobject.PARAM_READWRITE),
    } # __gproperties__

    def __init__(self):
        gobject.GObject.__init__(self)

        self.base = "/apps/gedit-2/plugins/sced"
        self.client = gconf.client_get_default()
        self.client.add_dir(self.base, gconf.CLIENT_PRELOAD_NONE)

        self.client.notify_add(self.base + "/runtime_folder",
            self.__on_runtime_folder_changed)

    def do_get_property(self, property):
        if property.name == "runtime-folder":
            return self.client.get_string(self.base + "/runtime_folder")
        else:
            raise AttributeError("Unknown property %s" % property.name)

    def do_set_property(self, property, value):
        if property.name == "runtime-folder":
            self.freeze_notify()
            self.client.set_string(self.base + "/runtime_folder", value)
            self.thaw_notify()
        else:
            raise AttributeError("Unknown property %s" % property.name)

    def __on_runtime_folder_changed(self, client, cnxn_id, entry, user_data):
        self.notify("runtime-folder")
