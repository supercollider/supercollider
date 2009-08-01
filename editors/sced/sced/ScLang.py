# -*- coding: utf-8 -*-

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

