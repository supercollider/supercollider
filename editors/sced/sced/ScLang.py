# -*- coding: utf-8 -*-
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


import os
import subprocess
import time # FIXME: time is a workaround

class ScLang:
    def __init__(self, plugin):
        self.__sclang = None
        self.__settings = plugin.settings

    def start (self):
        if self.running():
            return

        # FIXME: maybe we need a default value in Settings?
        folder = self.__settings.props.runtime_folder
        if folder is None:
            folder = os.getcwd()

        self.__sclang = subprocess.Popen(["sclang",
                "-i", "sced", "-d", folder],
                bufsize=0,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                close_fds=True)
        self.stdout = self.__sclang.stdout
        self.stdin = self.__sclang.stdin

    def stop(self):
        if self.running():
            self.stdin.close()
            self.__sclang.wait()
            self.__sclang = None

    def running(self):
        return (self.__sclang is not None) and (self.__sclang.poll() is None)

    # FIXME: use sclang.communicate()
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

