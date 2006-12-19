// =====================================================================
// SerialPort
//   serial port interface
//   (currently unix/posix only)

// device list
SerialPort.listDevices;	// prints to postbuffer
SerialPort.devices;		// returns array

// change device selection
SerialPort.devicePattern = "/dev/ttyUSB*"; // linux usb serial
SerialPort.devices;

SerialPort.devicePattern = nil;
SerialPort.devices;

// opening the port

// instance creation arguments
//
//   port           device path or index
//	 baudrate       baudrate [4800..115200]
//   databits       5 | 6 | 7 | 8
//   stopbits       true | false
//   parity         nil | 'even' | 'odd'
//   crtscts        hardware flow control (true | false)
//   xonxoff        software flow control (true | false)
//   exclusive      open the device exclusively (true | false)

(
p = SerialPort(
	"/dev/tty.usbserial-181",
	baudrate: 9600,
	crtscts: true);
)

// read a byte from the device

p.next;			// doesn't block
p.read;			// may suspend thisThread

// write a byte to the device

p.put(42);		// may suspend thisThread

// write multiple bytes to the device
// collection may be Int8Array or String

p.putAll("whaddayawant");
p.putAll(Int8Array[13, 10]);

// close the port

p.close;

// close all ports

SerialPort.closeAll;

// EOF