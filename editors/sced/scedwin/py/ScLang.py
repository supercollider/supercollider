# -*- coding: utf-8 -*-
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


import os
import subprocess
import time # FIXME: time is a workaround
import gtk
import gedit
import gobject

def warn_config_deferred(message, plugin):
    message = message + "\n\nWould you like to open the configuration dialog now?"
    dialog = gtk.MessageDialog(
        parent = gedit.app_get_default().get_active_window(),
        flags = gtk.DIALOG_MODAL,
        type = gtk.MESSAGE_WARNING,
        buttons = gtk.BUTTONS_YES_NO,
        message_format = message)
    dialog.set_title("SuperCollider")
    #dialog.format_secondary_text("")
    response = dialog.run()
    dialog.destroy()
    if response == gtk.RESPONSE_YES:
        dialog = plugin.create_configure_dialog()
        dialog.run()
        dialog.destroy()
    return False

def warn_config(message, plugin):
    gobject.idle_add( warn_config_deferred, message, plugin )

class ScLang:
    def __init__(self, plugin):
        self.__plugin = plugin
        self.__sclang = None

    def start (self):
        if self.running():
            return False

        sets = self.__plugin.settings()

        if sets.advanced is True:
            cmd = sets.sclang_cmd
            if cmd is None or cmd == "":
                warn_config (
                    "A command to start the interpreter appears not to be given. Please set a command in the Sced configuration dialog, and restart the SuperCollider mode.",
                    self.__plugin
                )
                return False
            wdir = sets.sclang_work_dir

        else:
            wdir = sets.sc_dir
            if wdir is None or wdir == "":
                warn_config (
                    "No SuperCollider folder appears to be given. Please set a folder in the Sced configuration dialog, and restart the SuperCollider mode.",
                    self.__plugin
                )
                return False
            cmd = os.path.join(wdir, "sclang.exe")

        if wdir is None or wdir == "":
            wdir = os.path.dirname(cmd)

        print "Starting SuperCollider:"
        print "-- command = " + cmd
        print "-- working dir = " + wdir

        # On Windows, Popen pops up a new console window when gedit
        # is not started itself from a console.

        # One way to avoid that is using the SW_HIDE flag of STARTUPINFO.
        # However, this also hides the first window created in SC language.

        # A working alternative is to specify CREATE_NO_WINDOW in
        # creationflags, but python does not expose this constant,
        # so we have to use its literal value

        #opt = subprocess.STARTUPINFO();
        #opt.dwFlags = subprocess.STARTF_USESHOWWINDOW;
        #opt.wShowWindow = subprocess.SW_HIDE;

        try:
            self.__sclang = subprocess.Popen (
                [cmd, "-i", "sced"],
                cwd = wdir,
                bufsize=0,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                #startupinfo = opt,
                creationflags = 0x08000000 # == CREATE_NO_WINDOW
            )
        except:
            warn_config (
                "The interpreter could not be started. Try adjusting the configuration, and restart the SuperCollider mode.",
                self.__plugin
            )
            return False

        self.stdout = self.__sclang.stdout
        self.stdin = self.__sclang.stdin

        return True

    def stop(self):
        if self.running():
            self.stdin.close()
            self.__sclang.wait()
            self.__sclang = None

    def running(self):
        return (self.__sclang is not None) and (self.__sclang.poll() is None)

    def evaluate(self, code, silent=False):
        self.stdin.write(code)
        if silent:
            self.stdin.write("\x1b")
        else:
            self.stdin.write("\x0c")
        self.stdin.flush()

    def toggle_recording(self, record):
        if record:
            self.evaluate("s.prepareForRecord;", silent=True)
            time.sleep(0.1) # give server some time to prepare
            self.evaluate("s.record;", silent=True)
        else:
            self.evaluate("s.stopRecording;", silent=True)

    def stop_sound(self):
        self.evaluate("thisProcess.stop;", silent=True)

