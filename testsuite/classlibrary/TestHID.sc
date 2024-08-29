HIDTest : UnitTest {
    classvar <device;
    classvar <addr;
        *setUpClass {
            var sync = CondVar.new;
            var cb = {|msg, time, addr, rcvPort|
                var devPath=msg[1].asString;
                devPath.postln;
                HID.findAvailable;
                device = HID.openPath(devPath)
            };
            Class.initClassTree(HID);
            Class.initClassTree(OSCFunc);
            super.initClass();

            addr=NetAddr("127.0.0.1", 57420);
            OSCFunc.new(cb, '/device_created');

            sync.wait({HID.running});
            addr.sendMsg('/test_hidraw_device_path');

        }
        *tearDownClass {
            addr.sendMsg('/quit');
        }
    test_Meow {
        HIDTest.device.path.postln;
        this.assertEquals(1,1);
    }
}
