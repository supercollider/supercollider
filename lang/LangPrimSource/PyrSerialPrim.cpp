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

#ifndef _WIN32
#    include <termios.h> // TIOCEXCL
#    include <sys/ioctl.h> // ioctl
#    include <errno.h> // errno
#    include <system_error> // system_error, error_code, error_category
#endif // _WIN32

#include <atomic>
#include <stdexcept>
#include <sstream>

#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSched.h"
#include "SCBase.h"
#include "SC_Lock.h"

#include <boost/asio/use_future.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using boost::uint8_t;
using boost::asio::serial_port;

extern boost::asio::io_service ioService; // defined in SC_ComPort.cpp

/**
 * \brief Serial port abstraction
 *
 * Uses SerialPort::Options for configuration. The port is opened on construction, and destruction
 * releases all resourses by canceling outstanding TX/RX requests and closing the port.
 * This class polls the port on a separate thread and immediately reads available data into an
 * internal buffer.
 */
class SerialPort {
public:
    // 7, not 6 - the last two options passed in are condensed into flow_control
    static const int kNumOptions = 7;
    static const int kBufferSize = 8192;

    /// Type of the underlying FIFO buffer.
    using FIFO = boost::lockfree::spsc_queue<uint8_t, boost::lockfree::capacity<kBufferSize>>;

    /**
     * \brief Serial port configuration options.
     *
     * \note The two SuperCollider options \c xonxoff and \c crtscts are used together to set
     * Options::flow_control.
     */
    struct Options {
        /// Whether to request exclusive access to this port. Not implemented on Windows.
        bool exclusive = false;

        serial_port::baud_rate baudrate { 9600 };

        /// Corresponds to \c databits in SC code
        serial_port::character_size charsize { 8 };

        /// Number of stop bits to send. In SC code, true = 2, false = 1
        serial_port::stop_bits::type stop_bits { serial_port::stop_bits::two };

        serial_port::parity::type parity { serial_port::parity::none };

        /// Whether to use XON/XOFF signals (software), RTS/CTS signals (hardware), or neither.
        serial_port::flow_control::type flow_control { serial_port::flow_control::hardware };
    };

    static PyrSymbol* s_dataAvailable;
    static PyrSymbol* s_doneAction;

public:
    /**
     * \brief Opens port, sets options, and starts polling thread
     *
     * May throw \c std::system_error or \c boost::system::system_error during creation if opening
     * the port fails or if setting options fails.
     */
    SerialPort(PyrObject* obj, const char* serialport, const Options& options):
        m_obj(obj),
        m_port(ioService, serialport),
        m_options(options),
        m_rxErrors(0) {
        using namespace boost::asio;

        m_port.set_option(options.baudrate);
        m_port.set_option(serial_port::parity(options.parity));
        m_port.set_option(options.charsize);
        m_port.set_option(serial_port::stop_bits(options.stop_bits));

        setFlowControl(options.flow_control);
        setExclusive(options.exclusive);

        m_readThread = SC_Thread { &SerialPort::doRead, this };
    }

    ~SerialPort() { m_readThread.join(); }

    const Options& options() const { return m_options; }

    /// Blocking single-byte write. Returns whether the write was successful.
    bool put(uint8_t byte) {
        boost::system::error_code ec {};
        m_port.write_some(boost::asio::buffer(&byte, sizeof(byte)), ec);
        return !ec;
    }

    /// Non-blocking single-byte read. Returns whether the read was successful.
    bool get(uint8_t* byte) {
        uint8_t ret;

        bool success = m_rxfifo.pop(ret);
        if (!success)
            return false;

        *byte = ret & 0xFF;
        return true;
    }

    /// The number of read errors since the last time this method was called.
    /**
     * A read error occurs when the FIFO is too full to receive data; the extra bytes are lost.
     */
    int rxErrorsSinceLastQuery() { return m_rxErrors.exchange(0); }

    /// Cancels all outstanding reads and writes and closes the port.
    void stop() {
        m_port.cancel();
        m_port.close();
    }

private:
    /// Sets this serial port to exclusive mode. Has no effect on Windows. Throws \c system_error
    /// on failure.
    void setExclusive(bool);

    /// Sets flow control behavior. Throws \c system_error if mode is not supported or if failure
    /// occurs during set.
    void setFlowControl(serial_port::flow_control::type control);

    /// Calls a one-arg SuperCollider command on this object.
    void runCommand(PyrSymbol* cmd) {
        gLangMutex.lock();
        if (m_obj) {
            VMGlobals* g = gMainVMGlobals;
            g->canCallOS = true;
            ++g->sp;
            SetObject(g->sp, m_obj);
            runInterpreter(g, cmd, 1);
            g->canCallOS = false;
        }
        gLangMutex.unlock();
    }

    /// Runs the doneAction callback function on the SuperCollider \c SerialPort object.
    void doneAction() { runCommand(s_doneAction); }

    /// Notifies the object that data is available, unblocking any blocking reads.
    void dataAvailable() { runCommand(s_dataAvailable); }

    /// Reads endlessly from port until an error is encountered, at which point the SerialPort
    /// object's `doneAction` runs.
    void doRead() {
        auto const&& buf = boost::asio::buffer(m_rxbuffer, kBufferSize);
        while (true) {
            auto txFuture = m_port.async_read_some(buf, boost::asio::use_future);
            try {
                // blocks until read completes
                auto bytesTransferred = txFuture.get();
                for (std::size_t index = 0; index != bytesTransferred; ++index) {
                    uint8_t byte = m_rxbuffer[index];
                    bool putSuccessful = m_rxfifo.push(byte);
                    if (!putSuccessful)
                        m_rxErrors++;
                }

                if (bytesTransferred > 0) {
                    dataAvailable();
                }
            } catch (const boost::system::system_error& e) {
                // TODO: pass info on error to sclang? ignore spurious errors?
                doneAction();
                return;
            }
        }
    }

    PyrObject* m_obj; ///< Language object representing this port.
    boost::asio::serial_port m_port; ///< Port object.
    Options m_options; ///< Serial interface options.
    SC_Thread m_readThread; ///< Polling thread.

    /// Count of read errors since last query
    std::atomic<int> m_rxErrors;

    /// I/O buffer
    FIFO m_rxfifo;

    /// Temp buffer for reads
    uint8_t m_rxbuffer[kBufferSize];
};

PyrSymbol* SerialPort::s_dataAvailable = 0;
PyrSymbol* SerialPort::s_doneAction = 0;

void SerialPort::setFlowControl(serial_port::flow_control::type control) {
    /* FIXME backported from 3.9 impl -- boost.asio doesn't correctly detect support for these features
     * on macOS and possibly Linux (https://github.com/boostorg/asio/issues/65). Remove this if that issue
     * is ever solved and we upgrade to that as minimum version. */
#if !defined(_WIN32) && (!defined(_POSIX_C_SOURCE) || defined(__USE_MISC))
    int fd = m_port.native_handle();
    termios toptions;
    if (::tcgetattr(fd, &toptions) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()));
    }

    if (control == serial_port::flow_control::hardware) {
        toptions.c_cflag &= ~CRTSCTS;
    } else {
        toptions.c_cflag |= CRTSCTS;
    }

    if (control == serial_port::flow_control::hardware) {
        toptions.c_iflag |= (IXON | IXOFF | IXANY);
    } else {
        toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
    }

    if (::tcsetattr(fd, TCSAFLUSH, &toptions) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()));
    }
#else // !_WIN32 && ( !_POSIX_C_SOURCE || __USE_MISC )
    m_port.set_option(serial_port::flow_control(control));
#endif // !_WIN32 && ( !_POSIX_C_SOURCE || __USE_MISC )
}

void SerialPort::setExclusive(bool b) {
#ifndef _WIN32
    if (!b) {
        return;
    }

    int fd = m_port.native_handle();
    if (ioctl(fd, TIOCEXCL) == -1) {
        throw std::system_error(std::error_code(errno, std::system_category()));
    }
#endif // _WIN32
}

// ================================================================================================
// helpers

static SerialPort* getSerialPort(PyrSlot* slot) {
    if (NotPtr(&slotRawObject(slot)->slots[0]))
        return NULL;
    return (SerialPort*)slotRawPtr(&slotRawObject(slot)->slots[0]);
}

/// Translates between SC parity IDs and boost::asio::serial_port parity types.
static serial_port::parity::type asParityType(int i) {
    using parity = serial_port::parity;
    switch (i) {
    case 0:
        return parity::none;
    case 1:
        return parity::even;
    case 2:
        return parity::odd;
    default:
        printf("*** WARNING: SerialPort: unknown parity: %d. Defaulting to none.\n", i);
        return parity::none;
    }
}

/// Translates between SC xonxoff/crtscts args and boost::asio::serial_port flow_control types.
/**
 * \note Throws std::runtime_error if both arguments are true.
 */
static serial_port::flow_control::type asFlowControlType(bool hardware, bool software) {
    using flow_control = serial_port::flow_control;

    if (hardware && software) {
        throw std::runtime_error("SerialPort: xonxoff and crtscts cannot both be true");
    } else if (hardware) {
        return flow_control::hardware;
    } else if (software) {
        return flow_control::software;
    } else {
        return flow_control::none;
    }
}

// ================================================================================================
// primitives

static int prSerialPort_Open(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1 - SerialPort::kNumOptions;

    int err;

    PyrSlot* self = args + 0;

    if (getSerialPort(self))
        return errFailed;

    char portName[PATH_MAX];
    err = slotStrVal(args + 1, portName, sizeof(portName));
    if (err)
        return err;

    SerialPort::Options options {};
    SerialPort* port = 0;

    options.exclusive = IsTrue(args + 2);

    int baudrate;
    err = slotIntVal(args + 3, &baudrate);
    if (err)
        return err;
    options.baudrate = serial_port::baud_rate { static_cast<unsigned int>(baudrate) };

    int charsize;
    err = slotIntVal(args + 4, &charsize);
    if (err)
        return err;
    options.charsize = serial_port::character_size { static_cast<unsigned int>(charsize) };

    options.stop_bits = IsTrue(args + 5) ? serial_port::stop_bits::two : serial_port::stop_bits::one;

    int parity;
    err = slotIntVal(args + 6, &parity);
    if (err)
        return err;
    options.parity = asParityType(parity);

    // crtscts = use hardware signals for control flow
    bool useHardware = IsTrue(args + 7);
    // xonxoff = use XON/XOFF message bytes for control flow
    bool useSoftware = IsTrue(args + 8);
    options.flow_control = asFlowControlType(useHardware, useSoftware);

    try {
        port = new SerialPort(slotRawObject(self), portName, options);
    } catch (boost::system::system_error& e) {
        delete port;
        if (e.code().value() == boost::system::errc::no_such_file_or_directory) {
            throw std::runtime_error(std::string("SerialPort: port '") + portName + "' does not exist");
        } else {
            throw;
        }
    } catch (std::exception& e) {
        delete port;
        // TODO: check error types to provide better messages, such as when port doesn't exist
        throw;
    }

    SetPtr(slotRawObject(self)->slots + 0, port);

    return errNone;
}

/** \brief Stop a SerialPort object.
 *
 * When `close` is triggered, a complex sequence of events happens:
 * 1. Port is closed and SerialPort object is marked as closed
 * 2. This SC thread eventually yields, allowing the SerialPort's doneAction to run via read thread
 * 3. That doneAction causes a deferred call to `prSerialPort_Cleanup` and then yields
 * 4. `Cleanup` sets the dataptr to nil and destroys the C++ object, causing the read thread to join
 */
static int prSerialPort_Close(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* self = g->sp;
    SerialPort* port = getSerialPort(self);
    if (!port)
        return errFailed;
    port->stop();

    return errNone;
}

static int prSerialPort_Cleanup(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* self = g->sp;
    SerialPort* port = getSerialPort(self);

    if (!port)
        return errFailed;

    delete port;
    SetNil(slotRawObject(self)->slots + 0);
    return errNone;
}

static int prSerialPort_Next(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* self = g->sp;
    SerialPort* port = getSerialPort(self);
    if (!port)
        return errFailed;

    uint8_t byte;
    if (port->get(&byte)) {
        SetInt(self, byte);
    } else {
        SetNil(self);
    }

    return errNone;
}

static int prSerialPort_Put(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp - 1;

    PyrSlot* self = args + 0;
    SerialPort* port = getSerialPort(self);
    if (!port)
        return errFailed;

    PyrSlot* src = args + 1;

    int val;
    if (IsChar(src)) {
        val = slotRawChar(src);
    } else {
        int err = slotIntVal(src, &val);
        if (err)
            return err;
    }

    bool res = port->put(val & 0xFF);
    SetBool(self, res);

    return errNone;
}

static int prSerialPort_RXErrors(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* self = g->sp;
    SerialPort* port = getSerialPort(self);
    if (!port)
        return errFailed;

    SetInt(self, port->rxErrorsSinceLastQuery());
    return errNone;
}

void initSerialPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_SerialPort_Open", prSerialPort_Open, 2 + SerialPort::kNumOptions, 0);
    definePrimitive(base, index++, "_SerialPort_Close", prSerialPort_Close, 1, 0);
    definePrimitive(base, index++, "_SerialPort_Next", prSerialPort_Next, 1, 0);
    definePrimitive(base, index++, "_SerialPort_Put", prSerialPort_Put, 2, 0);
    definePrimitive(base, index++, "_SerialPort_RXErrors", prSerialPort_RXErrors, 1, 0);
    definePrimitive(base, index++, "_SerialPort_Cleanup", prSerialPort_Cleanup, 1, 0);

    SerialPort::s_dataAvailable = getsym("prDataAvailable");
    SerialPort::s_doneAction = getsym("prDoneAction");
}
