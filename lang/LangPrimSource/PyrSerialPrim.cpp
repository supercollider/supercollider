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

#include <stdexcept>
#include <sstream>

#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSched.h"
#include "SCBase.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using boost::uint8_t;

extern boost::asio::io_service ioService; // defined in SC_ComPort.cpp

class SerialPort
{
public:
	static const int kNumOptions = 7;
	static const int kBufferSize = 8192;

	using FIFO = boost::lockfree::spsc_queue<uint8_t, boost::lockfree::capacity<kBufferSize>>;

	enum Parity
	{
		kNoParity,
		kEvenParity,
		kOddParity
	};

	struct Options
	{
		bool exclusive = false;
		size_t baudrate = 9600;
		size_t databits = 8;
		bool stopbit = true;
		Parity parity = kNoParity;
		bool crtscts = false;
		bool xonxoff = false;
	};

	static PyrSymbol* s_dataAvailable;
	static PyrSymbol* s_doneAction;

public:
	SerialPort(PyrObject* obj, const char* serialport, const Options& options):
		m_obj(obj),
		m_options(options),
		m_port(ioService, serialport)
	{
		using namespace boost::asio;

		m_port.set_option(serial_port::baud_rate(options.baudrate));

		switch (options.parity) {
		case kNoParity:
			m_port.set_option(serial_port::parity(serial_port::parity::none));
			break;
		case kEvenParity:
			m_port.set_option(serial_port::parity(serial_port::parity::even));
			break;
		case kOddParity:
			m_port.set_option(serial_port::parity(serial_port::parity::odd));
			break;
		}

		m_port.set_option(serial_port::character_size(options.databits));

		// FIXME: flow control / xonxoff
		// FIXME: stop bits
	}

	~SerialPort()
	{
		stop();
	}

	void startRead()
	{
		m_port.async_write_some(boost::asio::buffer(m_rxbuffer, kBufferSize),
								boost::bind(&SerialPort::doRead, this,
											boost::asio::placeholders::error,
											boost::asio::placeholders::bytes_transferred)
								);
	}

	const Options& options() const { return m_options; }

	bool put(uint8_t byte)
	{
		return m_port.write_some(boost::asio::buffer(&byte, sizeof(byte))) == sizeof(byte);
	}

	bool get(uint8_t* byte)
	{
		uint8_t ret;

		bool success = m_rxfifo.pop(ret);
		if (!success)
			return false;

		*byte = ret & 0xFF;
		return true;
	}

	int rxErrors()
	{
		// errors since last query
		int x         = m_rxErrors[1];
		int res       = x-m_rxErrors[0];
		m_rxErrors[0] = x;
		return res;
	}

	void stop()
	{
		m_port.cancel();
		m_port.close();
	}

private:
	void dataAvailable()
	{
		gLangMutex.lock();
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

	void doneAction()
	{
		gLangMutex.lock();
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

	void doRead(const boost::system::error_code& error,
				std::size_t bytesTransferred)
	{

#if 0
		// FIXME: when?
		if (error == ???) {
			if ( m_dodone )
				doneAction();
			return;
		}
#endif

		if (error) {
			// what to do?
			printf("SerialPort::doRead error: %s", error.message().c_str());
			startRead();
			return;
		}

		for (std::size_t index; index != bytesTransferred; ++index) {
			uint8_t byte = m_rxbuffer[index];
			bool putSuccessful = m_rxfifo.push(byte);
			if (!putSuccessful)
				m_rxErrors[1]++;
		}

		dataAvailable();

		startRead();
	}


	// language interface
	PyrObject*		m_obj;
	boost::asio::serial_port m_port;

	// serial interface
	Options			m_options;

	// rx buffers
	int			m_rxErrors[2];
	FIFO		m_rxfifo;
	uint8_t		m_rxbuffer[kBufferSize];
};

PyrSymbol* SerialPort::s_dataAvailable = 0;
PyrSymbol* SerialPort::s_doneAction = 0;

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

	SerialPort::Options options{};
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
	} catch (std::exception & e) {
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
	/*
	if (port->isCurrentThread()) {
		post("Cannot cleanup SerialPort from this thread. Call from AppClock thread.");
		return errFailed;
	}
	*/

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
