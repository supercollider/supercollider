/*
*** Demonstration and tests of bootAlt, an alternative for Server:boot ***

bootAlt features :
* all boot steps happen in a single task
* running scsynths can be recovered or rebooted
* posts info on all steps and the times they take
* adds arguments for onComplete and timeout for more flexibility

* assumes startAliveThread is always true (which it probably should be)
* manual test suite below

*/

// test all branches in bootAlt:
// turn posting on and off for debugging
Server.postingBootInfo = true;
Server.postingBootInfo = false;
Server.killAll;

// get boot time for s.boot for comparison:
(
s.quit;
~t0 = thisThread.seconds;
s.waitForBoot { "booted in % secs.\n".postf(thisThread.seconds - ~t0) };
)

// test basic bootAlt
s.quit;
s.bootAlt; // boots

// make s a remote server
s.addr = NetAddr("123.234.56.78", 123);
s.bootAlt;  // cant boot and quit
s.quit;

// test bootAndQuitDisabled on local
s.addr = NetAddr.localAddr.port_(57110);  // local again
s.quit; // can quit now
s.bootAndQuitDisabled = true;
s.bootAlt; // no boot and quit
s.quit;
s.bootAndQuitDisabled = false;


// test unresposive reboot
s.bootAlt; // boot first

// then pretend unresponsive
(
s.statusWatcher.stop;
s.statusWatcher.unresponsive_(true).serverRunning_(false).serverBooting_(false);
s.state_(\isOff);
)
s.bootAlt; // -> this reboots

// test unresposive reboot 2 - recover
s.bootAlt; // boot first, then pretend unresponsive
(
s.statusWatcher.stop;
s.statusWatcher.unresponsive_(true).serverRunning_(false).serverBooting_(false);
s.state_(\isOff);
)
// recovers the server and runs prRunBootTask
s.bootAlt(recover: true);

// test boot timeout:
s.quit;
p = Server.program;  // rename server program so it does not start
Server.program = "nonono";
s.bootAlt(onFailure: { "TEST TIMEOUT".postln; }); // onFailure after 5 sec
s.bootAlt(onFailure: { "fast TIMEOUT.".postln; }, timeout: 1);
p !? { Server.program = p }; // restore



// test onComplete argument:
// Synthdef is there, nodetree inited, etc etc
s.bootAlt(onComplete: { ().play; });
// is already running, runs directly
s.bootAlt(onComplete: { ().play; });

