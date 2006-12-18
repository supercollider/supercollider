// =====================================================================
// SerialPort
//   serial port interface
//   (currently unix/posix only)

// opening the port

(
p = SerialPort(
	"/dev/tty.usbserial-181",
	baudrate: 9600,
	crtscts: true);
)

// instance creation arguments
//
//   port           device path
//	 baudrate       baudrate [4800..115200]
//   databits       5 | 6 | 7 | 8
//   stopbits       true | false
//   parity(nil)    nil | 'even' | 'odd'
//   crtscts        hardware flow control (true | false)
//   xonxoff        software flow control (true | false)
//   exclusive      open the device exclusively (true | false)

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