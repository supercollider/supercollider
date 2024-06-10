#!/usr/bin/env python
from hidtools.device.base_gamepad import AsusGamepad

import signal
import sys
import time
import os

from pythonosc.dispatcher import Dispatcher
from pythonosc.osc_server import BlockingOSCUDPServer
from pythonosc.udp_client import SimpleUDPClient

class TestGamepad():
    def __init__(self):
        print("hidapi test tool")
        self.device = AsusGamepad()
        self.device.create_kernel_device()
        self.client = SimpleUDPClient("127.0.0.1", 57120)

    def quit(self):
        print("quitting uhid service")
        self.device.destroy()
        exit(0)

    def pid(self):
        self.client.send_message("/test_hidraw_pid", os.getpid())

    def device_path(self, osc_path):
        try:
            print(osc_path)
            pathname = self.device.hidraw_nodes[0]
            print(pathname)
            self.client.send_message("/device_created", pathname)
            return pathname
        except:
            return None

def default_handler(path, *args):
    print(f"DEFAULT {path}: {args}")

def main():
    device = TestGamepad()
    print("main up")
    signal.signal(signal.SIGTERM, device.quit)

    dispatcher = Dispatcher()
    dispatcher.map("/quit", device.quit)
    dispatcher.map("/test_hidraw_device_path", device.device_path)
    dispatcher.set_default_handler(default_handler)

    server = BlockingOSCUDPServer(("localhost", 57420), dispatcher)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        device.quit()


if __name__ == "__main__":
    main()
