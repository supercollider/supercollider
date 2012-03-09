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

import gedit
import gtk

from WindowHelper import WindowHelper
import Settings
from ConfigurationDialog import ConfigurationDialog

class ScedPlugin(gedit.Plugin):
    def __init__(self):
        gedit.Plugin.__init__(self)
        #self.settings = Settings()
        self.__instances = {}
        self.__settings = None

    def activate(self, window):
        self.__instances[window] = WindowHelper(self, window)

    def deactivate(self, window):
        self.__instances[window].deactivate()
        del self.__instances[window]

    def update_ui(self, window):
        self.__instances[window].update_ui()

    def create_configure_dialog(self):
        dialog = ConfigurationDialog(self)
        return dialog

    def settings(self):
        if self.__settings is None:
            self.__settings = Settings.load()
        return self.__settings

