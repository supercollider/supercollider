/*
	Serial port support.
	Copyright (c) 2006 stefan kersten.

	====================================================================

	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <atomic>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <boost/atomic.hpp>
#include <functional>

#include <stdexcept>
#include <sstream>

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSched.h"
#include "SCBase.h"

#include "SC_FIFO.h"
#include "SC_Lock.h"

class SerialPort
{
public:
	enum Parity
	{
		kNoParity,
		kEvenParity,
		kOddParity
	};

	struct Options
	{
		Options()
			: exclusive(false),
			  baudrate(9600),
			  databits(8),
			  stopbit(true),
			  parity(kNoParity),
			  crtscts(false),
			  xonxoff(false)
		{ }

		bool	exclusive;
		size_t	baudrate;
		size_t	databits;
		bool	stopbit;
		Parity	parity;
		bool	crtscts;
		bool	xonxoff;
	};

	static const int kNumOptions = 7;
	static const int kBufferSize = 8192;
	static const int kReadTimeoutMs = 1000;

	typedef SC_FIFO<uint8_t,kBufferSize> FIFO;

	struct Error : std::runtime_error
	{
		explicit Error(const char* what)
			: std::runtime_error(what)
		{ }
	};

	struct SysError : public Error
	{
		explicit SysError(int e=errno)
			: Error(strerror(e))
		{ }
	};

	static PyrSymbol* s_dataAvailable;
	static PyrSymbol* s_doneAction;

public:
	SerialPort(PyrObject* obj, const char* serialport, const Options& options);
	~SerialPort();

	bool isRunning() const { return m_running; }
	int fd() const { return m_fd; }
	const Options& options() const { return m_options; }

	bool put(uint8_t byte);
	bool get(uint8_t* byte);
	int rxErrors();

	void stop();
	void cleanup();
	bool isCurrentThread() const;

protected:
	void threadLoop();

	void dataAvailable();
	void doneAction();

private:
	// language interface
	PyrObject*		m_obj;

	std::atomic<bool>	m_dodone;

	// serial interface
	Options			m_options;
	int			m_fd;
	std::atomic<bool>	m_open;
	struct termios		m_termio;
	struct termios		m_oldtermio;

	// rx buffers
	int			m_rxErrors[2];
	FIFO			m_rxfifo;
	uint8_t			m_rxbuffer[kBufferSize];

	// rx thread
	std::atomic<bool>	m_running;
	SC_Thread				m_thread;
};

PyrSymbol* SerialPort::s_dataAvailable = 0;
PyrSymbol* SerialPort::s_doneAction = 0;

SerialPort::SerialPort(PyrObject* obj, const char* serialport, const Options& options)
	: m_obj(obj),
	  m_options(options),
	  m_fd(-1)
{
	// open non blocking
	m_fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_fd == -1)  {
		throw SysError(errno);
	}

	// exclusiveness
#if defined(TIOCEXCL)
	if (m_options.exclusive) {
		if (ioctl(m_fd, TIOCEXCL) == -1) {
			throw SysError(errno);
		}
	}
#endif // TIOCEXCL

	if (fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1) {
		int e = errno;
		close(m_fd);
		throw SysError(e);
	}

	// initialize serial connection

	// get current settings and remember them
	struct termios toptions;
	if (tcgetattr(m_fd, &toptions) < 0) {
		int e = errno;
		close(m_fd);
		throw SysError(e);
	}
	memcpy(&m_oldtermio, &toptions, sizeof(toptions));

	// baudrate
	speed_t brate;
	switch (m_options.baudrate) {
		case 1200:
			brate = B1200;
			break;
		case 1800:
			brate = B1800;
			break;
		case 2400:
			brate = B2400;
			break;
		case 4800:
			brate = B4800;
			break;
		case 9600:
			brate = B9600;
			break;
		case 19200:
			brate = B19200;
			break;
		case 38400:
			brate = B38400;
			break;
// #ifndef _POSIX_C_SOURCE
#if defined(B7200)
		case 7200:
			brate = B7200;
			break;
#endif
#if defined(B7200)
		case 14400:
			brate = B14400;
			break;
#endif
#if defined(B28800)
		case 28800:
			brate = B28800;
			break;
#endif
#if defined(B57600)
		case 57600:
			brate = B57600;
			break;
#endif
#if defined(B76800)
		case 76800:
			brate = B76800;
			break;
#endif
#if defined(B115200)
		case 115200:
			brate = B115200;
			break;
#endif
#if defined(B230400)
		case 230400:
			brate = B230400;
			break;
#endif
// #endif // !_POSIX_C_SOURCE
		default:
			close(m_fd);
			throw Error("unsupported baudrate");
	}

	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	// data bits
	toptions.c_cflag &= ~CSIZE;
	switch (m_options.databits)
	{
		case 5:
			toptions.c_cflag |= CS5;
			break;
		case 6:
			toptions.c_cflag |= CS6;
			break;
		case 7:
			toptions.c_cflag |= CS7;
			break;
		default:
			m_options.databits = 8;
			toptions.c_cflag |= CS8;
			break;
	}

	// stop bit
	if (m_options.stopbit) {
		toptions.c_cflag |= CSTOPB;
	} else {
		toptions.c_cflag &= ~CSTOPB;
	}

	// parity
	switch (m_options.parity)
	{
		case kNoParity:
			toptions.c_cflag &= ~PARENB;
			break;
		case kEvenParity:
			toptions.c_cflag |= PARENB;
			toptions.c_cflag &= ~PARODD;
			break;
		case kOddParity:
			toptions.c_cflag |= (PARENB | PARODD);
			break;
	}

	// h/w flow control
#if !defined(_POSIX_C_SOURCE) || defined(__USE_MISC)
	if (m_options.crtscts) {
		toptions.c_cflag &= ~CRTSCTS;
	} else {
		toptions.c_cflag |= CRTSCTS;
	}
#endif // !_POSIX_C_SOURCE || __USE_MISC

	// s/w flow control
	if (m_options.xonxoff) {
		toptions.c_iflag |= (IXON | IXOFF | IXANY);
	} else {
		toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
	}

	// (nescivi) by default carriage returns are translated to line feeds,
	// we don't want that
	toptions.c_iflag &= ~ICRNL;

	// enable READ & ignore ctrl lines
	toptions.c_cflag |= (CREAD | CLOCAL);
	// non-canonical (raw) i/o
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// disable post processing
	toptions.c_oflag &= ~OPOST;

	// see:  http://unixwiz.net/techtips/termios-vmin-vtime.html
	// NOTE: unused for non-blocking reads
// 	toptions.c_cc[VMIN]  = 0;
// 	toptions.c_cc[VTIME] = 20;

	if (tcsetattr(m_fd, TCSAFLUSH, &toptions) < 0) {
		int e = errno;
		close(m_fd);
		throw SysError(e);
	}
	memcpy(&m_termio, &toptions, sizeof(toptions));

	m_rxErrors[0] = m_rxErrors[1] = 0;

	try {
		SC_Thread thread(std::bind(&SerialPort::threadLoop, this));
		m_thread = std::move(thread);
	} catch(std::exception & e) {
		close(m_fd);
		throw e;
	}

	m_open = true;
	m_dodone = true;
}

SerialPort::~SerialPort()
{
	m_running = false;

	if ( m_open ){
		tcflush(m_fd, TCIOFLUSH);
		tcsetattr(m_fd, TCSANOW, &m_oldtermio);
		close(m_fd);
		m_open = false;
	}

	if (m_thread.joinable() && m_thread.get_id() != std::this_thread::get_id()) {
		m_thread.join();
	} else {
		assert(0 && "We're about to destroy m_thread from it's own thread, and without first joining!");
	}
}

void SerialPort::stop(){
	m_running = false;
}

void SerialPort::cleanup(){
	m_running = false;
	m_dodone = false;
	if ( m_open ){
		tcflush(m_fd, TCIOFLUSH);
		tcsetattr(m_fd, TCSANOW, &m_oldtermio);
		close(m_fd);
		m_open = false;
	};
}

bool SerialPort::isCurrentThread() const {
	return m_thread.get_id() == std::this_thread::get_id();
}

bool SerialPort::put(uint8_t byte)
{
	return write(m_fd, &byte, sizeof(byte)) == sizeof(byte);
}

bool SerialPort::get(uint8_t* byte)
{
	if (m_rxfifo.IsEmpty())
		return false;
	*byte = m_rxfifo.Get() & 0xFF;
	return true;
}

int SerialPort::rxErrors()
{
	// errors since last query
	int x         = m_rxErrors[1];
	int res       = x-m_rxErrors[0];
	m_rxErrors[0] = x;
	return res;
}

void SerialPort::dataAvailable()
{
	int status = lockLanguageOrQuit(m_running);
	if (status == EINTR)
		return;
	if (status) {
		postfl("error when locking language (%d)\n", status);
		return;
	}

	PyrSymbol *method = s_dataAvailable;
	if (m_obj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, method, 1);
		g->canCallOS = false;
	}
	gLangMutex.unlock();
}

void SerialPort::doneAction()
{
	int status = lockLanguageOrQuit(m_dodone);
	if (status == EINTR)
		return;
	if (status) {
		postfl("error when locking language (%d)\n", status);
		return;
	}

	PyrSymbol *method = s_doneAction;
	if (m_obj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, method, 1);
		g->canCallOS = false;
	}
	gLangMutex.unlock();
}

void SerialPort::threadLoop()
{
	const int fd = m_fd;
	const int max_fd = fd+1;

	m_running = true;
	m_rxErrors[1] = 0;

	while (true) {
		fd_set rfds;

		FD_ZERO(   &rfds);
		FD_SET(fd, &rfds);

		struct timeval timeout;
		timeout.tv_sec = kReadTimeoutMs/1000;
		timeout.tv_usec = (kReadTimeoutMs%1000)*1000;

		int n = select(max_fd, &rfds, 0, 0, &timeout);
		// 	int fdset = FD_ISSET(fd, &rfds);
		// 	printf( "fdset %i, n %i, errno %i\n", fdset, n, errno );
		if ( m_open ){
			if ((n > 0) && FD_ISSET(fd, &rfds)) {
				// printf("poll input\n");
				int nr = 0;
				// while (true) {
				if ( m_open ){
					int n2 = read(fd, m_rxbuffer, kBufferSize);
					//  printf("read %d, errno %i, errbadf %i, %i, %i\n", n2, errno, EBADF, EAGAIN, EIO);
					if (n2 > 0) {
						// write data to ringbuffer
						for (int i=0; i < n2; ++i) {
							if (!m_rxfifo.Put(m_rxbuffer[i])) {
								m_rxErrors[1]++;
								break;
							}
						}
						nr += n2;
					} else if ((n2 == 0) && (n == 1) ) { // added by nescivi, to check for disconnected device. In this case the read is 0 all the time and otherwise eats up the CPU
						//	printf( "done\n" );
						goto done;
					} else if ((n2 == 0) || ((n2 == -1) && (errno == EAGAIN))) {
						//	printf( "break\n");
						break;
					} else {
#ifndef NDEBUG
						printf("SerialPort HUP\n");
#endif
						goto done;
					}
				}
				//}
				if (!m_running) {
					// close and cleanup
					goto done;
				}
				if (nr > 0) {
					dataAvailable();
				}
			} else if (n == -1) {
				goto done;
			}
		}
		if (!m_running) {
			// close and cleanup
			goto done;
		}
	}

done:
	// doneAction();
	if ( m_open ){
		tcflush(fd, TCIOFLUSH);
		tcsetattr(fd, TCSANOW, &m_oldtermio);
		close(fd);
	};
	m_open = false;
	m_running = false;
	if ( m_dodone )
		doneAction();
#ifndef NDEBUG
	printf("SerialPort closed\n");
#endif
}

// =====================================================================
// primitives

static SerialPort* getSerialPort(PyrSlot* slot)
{
	if (NotPtr(&slotRawObject(slot)->slots[0]))
		return NULL;
	return (SerialPort*)slotRawPtr(&slotRawObject(slot)->slots[0]);
}

static int prSerialPort_Open(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *args = g->sp - 1 - SerialPort::kNumOptions;

	int err;

	PyrSlot* self = args+0;

	if (getSerialPort(self) != 0)
		return errFailed;

	char portName[PATH_MAX];
	err = slotStrVal(args+1, portName, sizeof(portName));
	printf("portName %s\n", portName);
	if (err) return err;

	SerialPort::Options options;
	SerialPort* port = 0;

	options.exclusive = IsTrue(args+2);

	int baudrate;
	err = slotIntVal(args+3, &baudrate);
	if (err) return err;
	options.baudrate = baudrate;

	int databits;
	err = slotIntVal(args+4, &databits);
	if (err) return err;
	options.databits = databits;

	options.stopbit = IsTrue(args+5);

	int parity;
	err = slotIntVal(args+6, &parity);
	if (err) return err;
	options.parity = (SerialPort::Parity)parity;

	options.crtscts = IsTrue(args+7);
	options.xonxoff = IsTrue(args+8);

	try {
		port = new SerialPort(slotRawObject(self), portName, options);
	} catch (SerialPort::Error& e) {
		std::ostringstream os;
		os << "SerialPort Error: " << e.what();
		post(os.str().c_str());
		return errFailed;
	}

	SetPtr(slotRawObject(self)->slots+0, port);

	return errNone;
}

static int prSerialPort_Close(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = (SerialPort*)getSerialPort(self);
	if (port == 0) return errFailed;
	port->stop();
	return errNone;
}

static int prSerialPort_Cleanup(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = (SerialPort*)getSerialPort(self);

	if (port == 0) return errFailed;
	if (port->isCurrentThread()) {
		post("Cannot cleanup SerialPort from this thread. Call from AppClock thread.");
		return errFailed;
	}

	port->cleanup();

	post("SerialPort Cleanup\n");

	delete port;
	SetNil(slotRawObject(self)->slots+0);
	return errNone;
}

static int prSerialPort_Next(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = (SerialPort*)getSerialPort(self);
//	printf( "port %i", port );
	if (port == 0) return errFailed;

	uint8_t byte;
	if (port->get(&byte)) {
		SetInt(self, byte);
	} else {
		SetNil(self);
	}

	return errNone;
}

static int prSerialPort_Put(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *args = g->sp - 1;

	PyrSlot* self = args+0;
	SerialPort* port = (SerialPort*)getSerialPort(self);
	if (port == 0) return errFailed;

	PyrSlot* src = args+1;

	int val;
	if (IsChar(src)) {
		val = slotRawChar(src);
	} else {
		int err = slotIntVal(src, &val);
		if (err) return err;
	}

	bool res = port->put(val & 0xFF);
	SetBool(self, res);

	return errNone;
}

static int prSerialPort_RXErrors(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = (SerialPort*)getSerialPort(self);
	if (port == 0) return errFailed;
	SetInt(self, port->rxErrors());
	return errNone;
}

void initSerialPrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_SerialPort_Open",     prSerialPort_Open, 2+SerialPort::kNumOptions, 0);
	definePrimitive(base, index++, "_SerialPort_Close",    prSerialPort_Close, 1, 0);
	definePrimitive(base, index++, "_SerialPort_Next",     prSerialPort_Next, 1, 0);
	definePrimitive(base, index++, "_SerialPort_Put",      prSerialPort_Put, 2, 0);
	definePrimitive(base, index++, "_SerialPort_RXErrors", prSerialPort_RXErrors, 1, 0);
	definePrimitive(base, index++, "_SerialPort_Cleanup",    prSerialPort_Cleanup, 1, 0);

	SerialPort::s_dataAvailable = getsym("prDataAvailable");
	SerialPort::s_doneAction = getsym("prDoneAction");
}

// EOF
