// You can now specify the device to start with 
// the string is "API : DeviceName"
// NOTE: you need to specify a string that is a substring of
// 	the 2 device names that specify input and output to
//	have both devices recognised correctly
//	Your options will be printed each time the server starts

// examples
//s.options.device_("ASIO : ASIO4ALL v2");
//s.options.device_("MME : Microsoft Soundmapper");
//s.options.device_("MME : Realtek AC97 Audio");
//s.options.dump;

Server.program = "scsynth.exe";
s.boot;

SwingOSC.program = "java -jar SwingOSC\\build\\SwingOSC.jar";
g=SwingOSC.default;
g.options.protocol_(\tcp);
g.boot;

GUI.swing;

// make window for server
{ s.makeWindow(); }.defer(3);


