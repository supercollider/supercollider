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

import gtk

# in order to get oblivion etc. colors, we may use:
#  client.get_string("/apps/gedit-2/preferences/editor/colors/scheme")
#  client = gconf.client_get_default()

class LogPanel(gtk.ScrolledWindow):
    def __init__(self):
        gtk.ScrolledWindow.__init__(self)

        self.props.shadow_type = gtk.SHADOW_IN
        self.props.hscrollbar_policy = gtk.POLICY_AUTOMATIC
        self.props.vscrollbar_policy = gtk.POLICY_AUTOMATIC

        self.buffer = gtk.TextBuffer()
        self.view = gtk.TextView(self.buffer)
        self.view.props.editable = False
        self.view.props.wrap_mode = gtk.WRAP_CHAR

        self.add(self.view)
        self.view.show()
