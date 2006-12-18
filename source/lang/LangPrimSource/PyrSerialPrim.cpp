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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

#include <stdexcept>

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "SC_LanguageClient.h"

#include "SC_FIFO.h"

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

public:
	SerialPort(PyrObject* obj, const char* serialport, const Options& options);
	~SerialPort();

	int fd() const { return m_fd; }
	const Options& options() const { return m_options; }

	bool put(uint8_t byte);
	bool get(uint8_t* byte);
	bool flush();

protected:
	static void* threadFunc(void*);
	void threadLoop();

	void dataAvailable();

private:
	// language interface
	PyrObject*			m_obj;

	// serial interface
	Options				m_options;
	int					m_fd;
	struct termios		m_termio;
	struct termios		m_oldtermio;

	// rx buffers
	int					m_xruns[2];
	FIFO				m_rxfifo;
	uint8_t				m_rxbuffer[kBufferSize];

	// rx thread
	volatile bool		m_running;
	pthread_t			m_thread;
};

PyrSymbol* SerialPort::s_dataAvailable = 0;

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
		case 57600:
			brate = B57600;
			break;
		case 115200:
			brate = B115200;
			break;
		default:
			brate = B9600;
			m_options.baudrate=9600;
			break;
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

	// enable READ & ignore ctrl lines
	toptions.c_cflag |= (CREAD | CLOCAL);
	// non-canonical (raw) i/o
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// disable post processing
	toptions.c_oflag &= ~OPOST;

	// make communication fully synchronous
	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
// 	toptions.c_cc[VMIN]  = 0;
// 	toptions.c_cc[VTIME] = 20;
    
	if (tcsetattr(m_fd, TCSAFLUSH, &toptions) < 0) {
		int e = errno;
		close(m_fd);
		throw SysError(e);
	}
	memcpy(&m_termio, &toptions, sizeof(toptions));

	m_xruns[0] = m_xruns[1] = 0;

	int e = pthread_create(&m_thread, 0, threadFunc, this);
	if (e != 0) {
		close(m_fd);
		throw SysError(e);
	}
}

SerialPort::~SerialPort()
{
	m_running = false;
	pthread_join(m_thread, 0);
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

bool SerialPort::flush()
{
	int xruns = m_xruns[1];
	if (m_xruns[0] != xruns) {
		m_xruns[0] = xruns;
		return true;
	}
	return false;
}

void* SerialPort::threadFunc(void* self)
{
	((SerialPort*)self)->threadLoop();
	return 0;
}

void SerialPort::dataAvailable()
{
    pthread_mutex_lock (&gLangMutex);
    PyrSymbol *method = s_dataAvailable;
    if (m_obj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp; SetObject(g->sp, m_obj); 
        runInterpreter(g, method, 1);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}

void SerialPort::threadLoop()
{
	const int fd = m_fd;
	const int max_fd = fd+1;

// 	printf("SerialPort: entering main loop\n");

	m_running = true;
	m_xruns[1] = 0;

	while (true) {
		fd_set rfds;
		FD_ZERO(   &rfds);
		FD_SET(fd, &rfds);

		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int n = select(max_fd, &rfds, 0, 0, &timeout);
		if (n > 0) {
			if (FD_ISSET(fd, &rfds)) {
// 				printf("poll input\n");
				int nr = 0;
				while (true) {
					int n = read(fd, m_rxbuffer, kBufferSize);
// 					printf("read %d\n", n);
					if (n > 0) {
						// write data to ringbuffer
						for (int i=0; i < n; ++i) {
							if (!m_rxfifo.Put(m_rxbuffer[i])) {
								m_xruns[1]++;
								break;
							}
						}
						nr += n;
					} else if (n == -1) {
						if (errno == EAGAIN) break;
						else goto done;
					} else {
// 						printf("SerialPort HUP\n");
						goto done;
					}
				}
				if (!m_running) {
					// close and cleanup
					goto done;
				}
				if (nr > 0) {
					dataAvailable();
				}
			}
		} else if (n == -1) {
			goto done;
		}
		if (!m_running) {
			// close and cleanup
			goto done;
		}
	}

done:
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &m_oldtermio);
	close(fd);
// 	printf("SerialPort closed\n");
}

// =====================================================================
// primitives

static SerialPort* getSerialPort(PyrSlot* slot)
{
	return (SerialPort*)slot->uo->slots[0].uptr;
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
		port = new SerialPort(self->uo, portName, options);
	} catch (SerialPort::Error& e) {
		return errFailed;
	}

	SetPtr(self->uo->slots+0, port);

	return errNone;
}

static int prSerialPort_Close(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = getSerialPort(self);
	if (port == 0) return errFailed;
	delete port;
	SetNil(self->uo->slots+0);
	return errNone;
}

static int prSerialPort_Next(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = (SerialPort*)getSerialPort(self);
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
	if (src->utag == tagChar) {
		val = src->uc;
	} else {
		int err = slotIntVal(src, &val);
		if (err) return err;
	}

	bool res = port->put(val & 0xFF);
	SetBool(self, res);

	return errNone;
}

static int prSerialPort_Flush(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* self = g->sp;
	SerialPort* port = getSerialPort(self);
	if (port == 0) return errFailed;
	SetBool(self, port->flush());
	return errNone;
}

void initSerialPrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_SerialPort_Open",  prSerialPort_Open, 2+SerialPort::kNumOptions, 0);
	definePrimitive(base, index++, "_SerialPort_Close", prSerialPort_Close, 1, 0);
	definePrimitive(base, index++, "_SerialPort_Next",  prSerialPort_Next, 1, 0);
	definePrimitive(base, index++, "_SerialPort_Put",   prSerialPort_Put, 2, 0);
	definePrimitive(base, index++, "_SerialPort_Flush", prSerialPort_Flush, 1, 0);

	SerialPort::s_dataAvailable = getsym("prDataAvailable");
}

// EOF
