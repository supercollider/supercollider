/*
 *  SC_Wii.cpp
 *  SC3lang
 *
 *  Created by Marije Baalman on Fri 18 May 2007.
 *  Copyright (c) 2007 Marije Baalman All rights reserved.
 *  inspired by Pete Moss' code for the ugens, using the DarwiinRemote code
 *	part of ...
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
#ifdef SC_DARWIN
//#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include <Carbon/Carbon.h>

//#include "WiiMote_OSX/wiiremote.h"
extern "C"{
#include "wiiremote.h"
}

#include <mach/mach.h>
#include <mach/mach_error.h>
#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"

//--------- PyrSymbols ------------
PyrSymbol * s_wiiDisconnected;
PyrSymbol * s_wiiConnected;
PyrSymbol * s_handleEvent;

static PyrSymbol* s_wii = 0;
static PyrSymbol* s_wiiCalibrationInfoClass = 0;
static PyrSymbol* s_wiiLEDStateClass = 0;
static PyrSymbol* s_readError = 0;

extern bool compiledOK;
//int gNumberOfWiiDevices = 0;
//EventLoopTimerRef gTimer = NULL; // timer for element data updates

//extern "C"{
// void			wiiremote_init(WiiRemoteRef wiiremote);
//Boolean			wiiremote_isconnected(WiiRemoteRef wiiremote);
//Boolean			wiiremote_search(WiiRemoteRef wiiremote, char *address);
//Boolean			wiiremote_stopsearch(WiiRemoteRef wiiremote);
//Boolean			wiiremote_connect(WiiRemoteRef wiiremote);
//Boolean			wiiremote_disconnect(WiiRemoteRef wiiremote);
//void			wiiremote_getaddress(WiiRemoteRef wiiremote, char *address);
//Boolean			wiiremote_motionsensor(WiiRemoteRef wiiremote, Boolean enabled);
//Boolean			wiiremote_irsensor(WiiRemoteRef wiiremote, Boolean enabled);
//Boolean			wiiremote_vibration(WiiRemoteRef wiiremote, Boolean enabled);
//Boolean			wiiremote_led(WiiRemoteRef wiiremote, Boolean enabled1, Boolean enabled2, Boolean enabled3, Boolean enabled4);
//Boolean			wiiremote_expansion(WiiRemoteRef wiiremote, Boolean enabled);
//Boolean			wiiremote_getstatus(WiiRemoteRef wiiremote);



// --------------- SC_WII structure --------------
struct SC_WII
{
	SC_WII(PyrObject *obj);
	~SC_WII();

	bool open();
	bool close();
	bool wii_connect();
	bool wii_disconnect();
	void disconnected();
	void connected();
	void get_address();

	void handleEvent();
	void readError();


	static PyrObject* getObject(PyrSlot* slot)
	{
		return isKindOfSlot(slot, s_wii->u.classobj) ? slot->uo : 0;
	}

	static SC_WII* getDevice(PyrObject* obj)
	{
		return (SC_WII*)obj->slots[0].uptr;
	}

	WiiRemoteRef		m_wiiremote;
	char			m_address[32];
	bool			m_connected;
	
	SC_WII*			m_next;
	PyrObject*		m_obj;
};
/*
struct SC_WIIClassic{
	SC_WIIClassic(PyrObject *obj);
	~SC_WIIClassic();
	void getData();

// 	SC_WII*			m_wii;

	SC_WIIClassic*		m_next;
	PyrObject*		m_obj;
};

struct SC_WIINunChuck{
	SC_WIINunChuck(PyrObject *obj);
	~SC_WIINunChuck();

	void getData();

// 	SC_WII*			m_wii;

	SC_WIINunChuck*		m_next;
	PyrObject*		m_obj;
};

struct SC_WIIRemote{
	SC_WIIRemote(PyrObject *obj);
	~SC_WIIRemote();

 	void getData();
//  	SC_WII*			m_wii;

	int	accX;
	int	accY;
	int	accZ;
	int	buttonData;
	
	float	lowZ;
	float	lowX;
	int	orientation;
	int	leftPoint; // is point 0 or 1 on the left. -1 when not tracking.
	
	float	posX;
	float	posY;
	float	angle;
	bool	tracking;
	
	IRData	irData[4];

	SC_WIIRemote*		m_next;
	PyrObject*		m_obj;
};*/

// ---------------- SC_WIIManager structure --------------

struct SC_WIIManager
{
public:
	static SC_WIIManager& instance();

	int start();
	int stop();

	int add(SC_WII *dev);
	int remove(SC_WII *dev);
	

private:
	SC_WIIManager();
	~SC_WIIManager();

	enum
	{
		kStop,
		kAdd,
		kRemove
	};

	struct Command
	{
		int	id;
		union
		{
			SC_WII*	dev;
		}	arg;
	};

	int sendCommand(const Command& cmd);
// 	void devicesChanged();
	bool asyncAddDevice(SC_WII *dev);
	bool asyncRemoveDevice(SC_WII *dev);
	void loop();

	static void* threadFunc(void*);

	pthread_t		m_thread;
	pthread_mutex_t		m_mutex;
	bool			m_running;
	int			m_cmdFifo[2];
// 	int			m_nfds;
// 	fd_set			m_fds;
	SC_WII*			m_devices;
};



// --------- SWII Classic -------
// SC_WIIClassic::SC_WIIClassic(PyrObject* obj)
// 	: m_next(0),
// 	  m_obj(obj)
// {
// 	SetPtr(obj->slots+0, this);
// }
// 
// SC_WIIClassic::~SC_WIIClassic()
// {
// }
// 
// SC_WIINunChuck::SC_WIINunChuck(PyrObject* obj)
// 	: m_next(0),
// 	  m_obj(obj)
// {
// 	SetPtr(obj->slots+0, this);
// }
// 
// SC_WIINunChuck::~SC_WIINunChuck()
// {
// }
// 
// SC_WIIRemote::SC_WIIRemote(PyrObject* obj)
// 	: m_next(0),
// 	  m_obj(obj)
// {
// 	SetPtr(obj->slots+0, this);
// }
// 
// SC_WIIRemote::~SC_WIIRemote()
// {
// }

//---------------- SC_WII functions -----------------

SC_WII::SC_WII(PyrObject* obj)
	: m_next(0),
	  m_obj(obj)
{
	m_wiiremote = nil;
// 	m_address = nil;
	SetPtr(obj->slots+0, this);
}

SC_WII::~SC_WII()
{
	if (m_connected ) wii_disconnect();
}

bool SC_WII::open()
{
	m_wiiremote = (WiiRemoteRef)malloc(sizeof(WiiRemoteRec));
	if (m_wiiremote != nil)
		{
		wiiremote_init(m_wiiremote);
		m_wiiremote->isMotionSensorEnabled = true;
		m_wiiremote->isIRSensorEnabled = false;
		m_wiiremote->isVibrationEnabled = false;
		m_wiiremote->isExpansionPortEnabled = false;
		m_wiiremote->isLED1Illuminated = false;
		m_wiiremote->isLED2Illuminated = false;
		m_wiiremote->isLED3Illuminated = false;
		m_wiiremote->isLED4Illuminated = false;

		m_connected = false;
		return SC_WIIManager::instance().add(this);
		}
	
	return( false );
}

bool SC_WII::close()
{
	if (m_wiiremote != nil)
	{
		if (wiiremote_isconnected(m_wiiremote))
			wiiremote_disconnect(m_wiiremote);
		free(m_wiiremote); //m_wiiremote, sizeof(WiiRemoteRec));
		m_wiiremote = nil;
	}
	SetNil(m_obj->slots+0);
	return SC_WIIManager::instance().remove(this);
}

bool SC_WII::wii_connect()
{
	bool	result;

	if (wiiremote_isconnected(m_wiiremote))
	{
		m_connected = true;
	}
	else
	{
		result = wiiremote_search( m_wiiremote, m_address);	// start searching the device
		m_connected = false;
	}
	return( m_connected );
}

void SC_WII::connected()
{
	m_connected = true;
	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_wiiConnected, 1);
		g->canCallOS = false;
	}
	pthread_mutex_unlock(&gLangMutex);
}

bool SC_WII::wii_disconnect()
{
	bool	result;

	/// TODO: remove from wii_manager

	wiiremote_stopsearch( m_wiiremote );
	result = wiiremote_disconnect( m_wiiremote);

	// call disconnect action
	if ( result )
		disconnected();
	return( result );
}

void SC_WII::disconnected()
{
	m_connected = false;
	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_wiiDisconnected, 1);
		g->canCallOS = false;
	}
	pthread_mutex_unlock(&gLangMutex);
}

void SC_WII::get_address()
{
	if (m_wiiremote->device == nil)
	{
		return;
	}
	else
	{
		char	str[32];
		wiiremote_getaddress(m_wiiremote, str);
// 		if ( m_address != nil )
// 			free( m_address );
// 		m_address = (char*) malloc (sizeof(str));
		strcpy(m_address, str);
	}
}

void SC_WII::handleEvent()
{
	if (m_wiiremote->device == nil)
		return;	// do nothing

	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		// buttons
		++g->sp; SetInt(g->sp, m_wiiremote->buttonData);
		if (m_wiiremote->isIRSensorEnabled)
			{// IR sensor
			++g->sp; SetFloat(g->sp, m_wiiremote->posX);
			++g->sp; SetFloat(g->sp, m_wiiremote->posY);
			++g->sp; SetFloat(g->sp, m_wiiremote->angle);
			++g->sp; SetInt(g->sp, m_wiiremote->tracking);
		} else {
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
		}
		if (m_wiiremote->isMotionSensorEnabled)
			{ // motion sensor
			++g->sp; SetInt(g->sp, m_wiiremote->accX);
			++g->sp; SetInt(g->sp, m_wiiremote->accY);
			++g->sp; SetInt(g->sp, m_wiiremote->accZ);
			++g->sp; SetInt(g->sp, m_wiiremote->orientation);
		} else {
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
		}
		if (m_wiiremote->isExpansionPortAttached && m_wiiremote->isExpansionPortEnabled)
			{
			// Classic Controller
			if (m_wiiremote->expType == WiiClassicController)
				{
				// buttons
				++g->sp; SetInt(g->sp, m_wiiremote->cButtonData);
	
				// Joystick 1
				++g->sp; SetInt(g->sp, m_wiiremote->cStickX1);
				++g->sp; SetInt(g->sp, m_wiiremote->cStickY1);
	
				// Joystick 2
				++g->sp; SetInt(g->sp, m_wiiremote->cStickX2);
				++g->sp; SetInt(g->sp, m_wiiremote->cStickY2);
	
				// Analog
				++g->sp; SetInt(g->sp, m_wiiremote->cAnalogL);
				++g->sp; SetInt(g->sp, m_wiiremote->cAnalogR);
			}
			// Nunchuk
			if (m_wiiremote->expType == WiiNunchuk)
			{	
				// Buttons
				++g->sp; SetInt(g->sp, m_wiiremote->nButtonData);
				
				// Joystick
				++g->sp; SetInt(g->sp, m_wiiremote->nStickX);
				++g->sp; SetInt(g->sp, m_wiiremote->nStickY);
				
				// Motion Sensor
				if (m_wiiremote->isMotionSensorEnabled)
				{
					++g->sp; SetInt(g->sp, m_wiiremote->nAccX);
					++g->sp; SetInt(g->sp, m_wiiremote->nAccY);
					++g->sp; SetInt(g->sp, m_wiiremote->nAccZ);
					++g->sp; SetInt(g->sp, m_wiiremote->nOrientation);
				} else {
					++g->sp; SetInt(g->sp, 0);
					++g->sp; SetInt(g->sp, 0);
					++g->sp; SetInt(g->sp, 0);
					++g->sp; SetInt(g->sp, 0);
				}
			}
		} else {
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
		}

		runInterpreter(g, s_handleEvent, 13);
		g->canCallOS = false;
		}
	pthread_mutex_unlock(&gLangMutex);
}

void SC_WII::readError()
{
	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_readError, 1);
		g->canCallOS = false;
	}
	pthread_mutex_unlock(&gLangMutex);
}


//------------ SC_WIIManager functions --------
SC_WIIManager& SC_WIIManager::instance()
{
	static SC_WIIManager instance;
	return instance;
}

SC_WIIManager::SC_WIIManager()
	: m_running(false),
	  m_devices(0)
{
 	if (pipe(m_cmdFifo) == -1) {
 		m_cmdFifo[0] = m_cmdFifo[1] = -1;
 	}
 //	devicesChanged();
	NumVersion	outSoftwareVersion;
	BluetoothHCIVersionInfo	outHardwareVersion;
	
//	post("aka.wiiremote 1.0B6-UB by Masayuki Akamatsu");

	if (IOBluetoothGetVersion(&outSoftwareVersion, &outHardwareVersion)==kIOReturnSuccess)
	{
		if (outSoftwareVersion.majorRev < 1 || outSoftwareVersion.minorAndBugRev < 0x63)
		{
			error("requires Bluetooth version 1.6.3 or later.");
			return;
		}
	}
	else
	{
		error("can't get Bluetooth version.");
		return;
	}

//	post("\tSC port by Marije Baalman, compiled with the help of J. Trutzschler & many others ;-)");
}

SC_WIIManager::~SC_WIIManager()
{
	close(m_cmdFifo[0]);
	close(m_cmdFifo[1]);
}

int SC_WIIManager::start()
{
	int err;
	err = pthread_create(&m_thread, 0, &threadFunc, this);
	if (err != 0) return errFailed;
	return errNone;
}

int SC_WIIManager::stop()
{
	Command cmd;
	int err;

	cmd.id = kStop;
	err = sendCommand(cmd);
	if (err) return err;

	err = pthread_join(m_thread, 0);
	if (err != 0) return errFailed;

	return errNone;
}

int SC_WIIManager::add(SC_WII* dev)
{
	Command cmd;
	cmd.id = kAdd;
	cmd.arg.dev = dev;
	return sendCommand(cmd);
}

int SC_WIIManager::remove(SC_WII* dev)
{
	Command cmd;
	cmd.id = kRemove;
	cmd.arg.dev = dev;
	return sendCommand(cmd);
}

int SC_WIIManager::sendCommand(const Command& cmd)
{
 	return write(m_cmdFifo[1], &cmd, sizeof(cmd)) == sizeof(cmd) ? errNone : errFailed;
}

bool SC_WIIManager::asyncAddDevice(SC_WII* dev)
{
	if (dev->m_next) return false;
	dev->m_next = m_devices;
	m_devices = dev;
// 	devicesChanged();
	return true;
}

bool SC_WIIManager::asyncRemoveDevice(SC_WII* dev)
{
	SC_WII *prev = 0, *cur = m_devices;

	while (cur) {
		if (cur == dev) {
			if (prev) prev->m_next = dev->m_next;
			else m_devices = dev->m_next;
			dev->m_next = 0;
			delete dev;
// 			devicesChanged();
			return true;
		}
		prev = cur;
		cur = cur->m_next;
	}

	return false;
}

void* SC_WIIManager::threadFunc(void* arg)
{
	((SC_WIIManager*)arg)->loop();
	return 0;
}

void SC_WIIManager::loop()
{
	post("WII: event loop started\n");

	while (true) {
		Command cmd;
		int err = read(m_cmdFifo[0], &cmd, sizeof(cmd));
		if (err == -1) {
			post("WII: error in input handler: %s\n", strerror(errno));
			goto quit;
			}
		switch (cmd.id) {
			case kStop:
			goto quit;
			case kAdd:
				if (asyncAddDevice(cmd.arg.dev)) {
					post("WII: added device %p\n", cmd.arg);
				} else {
					post("WII: cannot add device\n");
				}
			break;
			case kRemove:
				if (asyncRemoveDevice(cmd.arg.dev)) {
					post("WII: removed device %p\n", cmd.arg);
				} else {
					post("WII: couldn't remove device\n");
				}
			break;
			default:
				post("WII: unknown command in input handler\n");
			}
		SC_WII *dev = m_devices;
		while (dev) {
			bool	connection;
			if ( dev->m_wiiremote != nil )
				{
				connection = wiiremote_isconnected(dev->m_wiiremote);
	
				if ( dev->m_connected == false && connection == true)	// if the device is connected, but wasn't before
					{
					wiiremote_getstatus(dev->m_wiiremote);
					dev->connected();
					}
				if (dev->m_connected == true && connection == false) // if device was disconnected
					{
					dev->disconnected();
					}
				if ( dev->m_connected ) {
					dev->handleEvent();
					}
				}
			else
				dev->readError();
			dev = dev->m_next;
			}
		}
 	quit:
		post("WII: event loop stopped\n");
}


//------------ primitives ---------------

int prWii_Start(VMGlobals* g, int numArgsPushed)
{
// 	if (!g->canCallOS) return errCantCallOS;
	return SC_WIIManager::instance().start();
}

int prWii_Stop(VMGlobals* g, int numArgsPushed)
{
// 	if (!g->canCallOS) return errCantCallOS;
	return SC_WIIManager::instance().stop();
}

int prWii_Open(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

// this used to read the path of the device node
	char path[PATH_MAX];
	err = slotStrVal(args+1, path, sizeof(path));
	if (err) return err;

	SC_WII* dev = new SC_WII(obj);
	err = dev->open();
	if (err) {
		delete dev;
		return err;
	}

	return errNone;
}

int prWii_Close(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	return dev->close();
}

int prWiiAddress(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;
	
	dev->get_address();

	SetSymbol(args, getsym(dev->m_address));

	return errNone;
}


int prWiiConnect(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;
	
	dev->wii_connect();

	return errNone;
}

int prWiiDisconnect(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;
	
	dev->wii_disconnect();

	return errNone;
}

int prWiiCalibration(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	if (!isKindOfSlot(args+1, s_wiiCalibrationInfoClass->u.classobj))
		return errWrongType;
	PyrObject* infoObj = args[1].uo;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		SetFloat(infoObj->slots+0, dev->m_wiiremote->wiiCalibData.accX_zero);
		SetFloat(infoObj->slots+1, dev->m_wiiremote->wiiCalibData.accY_zero);
		SetFloat(infoObj->slots+2, dev->m_wiiremote->wiiCalibData.accZ_zero);
		SetFloat(infoObj->slots+3, dev->m_wiiremote->wiiCalibData.accX_1g);
		SetFloat(infoObj->slots+4, dev->m_wiiremote->wiiCalibData.accY_1g);
		SetFloat(infoObj->slots+5, dev->m_wiiremote->wiiCalibData.accZ_1g);

		if (dev->m_wiiremote->isExpansionPortAttached)
		{
			SetFloat(infoObj->slots+6, dev->m_wiiremote->nunchukCalibData.accX_zero);
			SetFloat(infoObj->slots+7, dev->m_wiiremote->nunchukCalibData.accY_zero);
			SetFloat(infoObj->slots+8, dev->m_wiiremote->nunchukCalibData.accZ_zero);
			SetFloat(infoObj->slots+9, dev->m_wiiremote->nunchukCalibData.accX_1g);
			SetFloat(infoObj->slots+10, dev->m_wiiremote->nunchukCalibData.accY_1g);
			SetFloat(infoObj->slots+11, dev->m_wiiremote->nunchukCalibData.accZ_1g);
	
			SetFloat(infoObj->slots+12, dev->m_wiiremote->nunchukJoyStickCalibData.x_max);
			SetFloat(infoObj->slots+13, dev->m_wiiremote->nunchukJoyStickCalibData.x_min);
			SetFloat(infoObj->slots+14, dev->m_wiiremote->nunchukJoyStickCalibData.x_center);
			SetFloat(infoObj->slots+15, dev->m_wiiremote->nunchukJoyStickCalibData.y_max);
			SetFloat(infoObj->slots+16, dev->m_wiiremote->nunchukJoyStickCalibData.y_min);
			SetFloat(infoObj->slots+17, dev->m_wiiremote->nunchukJoyStickCalibData.y_center);
		}
		
	}

	args[0].ucopy = args[1].ucopy;
	return errNone;
}

int prWiiGetLED(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 5;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

// 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// 		return errWrongType;
// 	PyrObject* infoObj = args[1].uo;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		SetFloat(args+1, dev->m_wiiremote->isLED1Illuminated);
		SetFloat(args+2, dev->m_wiiremote->isLED2Illuminated);
		SetFloat(args+3, dev->m_wiiremote->isLED3Illuminated);
		SetFloat(args+4, dev->m_wiiremote->isLED4Illuminated);
	}

	return errNone;
}

int prWiiGetExpansion(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		if (dev->m_wiiremote->isExpansionPortAttached)
			SetInt( args, dev->m_wiiremote->expType );
		else
			SetInt( args, 0 );
	}

	return errNone;
}

int prWiiGetBattery(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		if (dev->m_wiiremote->batteryLevel)
			SetFloat( args, dev->m_wiiremote->batteryLevel );
		else
			SetFloat( args, 0 );
	}

	return errNone;
}

int prWiiSetLED(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 4;
	int err;
	bool result;
	int enable1, enable2, enable3, enable4;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	err = slotIntVal(args+2, &enable2);
	if (err) return err;

	err = slotIntVal(args+2, &enable3);
	if (err) return err;

	err = slotIntVal(args+2, &enable4);
	if (err) return err;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		result = wiiremote_led( dev->m_wiiremote, enable1, enable2, enable3, enable4);
	}

	return errNone;
}

int prWiiSetVibration(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 2;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		result = wiiremote_vibration( dev->m_wiiremote, enable1 );
	}

	return errNone;
}

int prWiiSetExpansion(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 2;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		result = wiiremote_expansion( dev->m_wiiremote, enable1 );
	}

	return errNone;
}

int prWiiSetIRSensor(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 2;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		result = wiiremote_irsensor( dev->m_wiiremote, enable1 );
	}

	return errNone;
}

int prWiiSetMotionSensor(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 2;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == nil)
	{
		return errFailed;
	}
	else
	{
		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
	}

	return errNone;
}


void initWiiPrimitives()
{
	int base, index;

//	releaseHIDDevices();
	
	s_wii = getsym("WiiMote");
	s_wiiCalibrationInfoClass = getsym("WiiCalibrationInfo");	// has calibration date for all axes
	s_wiiLEDStateClass = getsym("WiiLEDState");			// has the four LED states

// 	s_wiiRemoteClass = getsym("WiiRemote");			// Remote
// 	s_wiiNunChuckClass = getsym("WiiNunChuck");		// NunChuck
// 	s_wiiClassicClass = getsym("WiiClassic");		// Classic

// 	s_wiiAction = getsym("prWiiMoteAction");
	s_wiiDisconnected = getsym("prDisconnectAction");
	s_wiiConnected = getsym("prConnectAction");
	s_handleEvent = getsym("prHandleEvent");
	s_readError = getsym("prReadError");


	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_WiiStart", prWii_Start, 1, 0); // starts the eventloop
	definePrimitive(base, index++, "_WiiStop", prWii_Stop, 1, 0); // stops the eventloop

	definePrimitive(base, index++, "_WiiOpen", prWii_Open, 1, 0 );
	definePrimitive(base, index++, "_WiiClose", prWii_Close, 1, 0 );

	definePrimitive(base, index++, "_WiiAddress", prWiiAddress, 1, 0);
	definePrimitive(base, index++, "_WiiConnect", prWiiConnect, 1, 0);
	definePrimitive(base, index++, "_WiiDisconnect", prWiiDisconnect, 1, 0);
	
	definePrimitive(base, index++, "_WiiCalibration", prWiiCalibration, 2, 0);
	definePrimitive(base, index++, "_WiiGetExpansion", prWiiGetExpansion, 1, 0);
	definePrimitive(base, index++, "_WiiGetBattery", prWiiGetBattery, 1, 0);

	definePrimitive(base, index++, "_WiiGetLED", prWiiGetLED, 5, 0);
	definePrimitive(base, index++, "_WiiSetLED", prWiiSetLED, 5, 0);

	definePrimitive(base, index++, "_WiiSetVibration", prWiiSetVibration, 2, 0);

	definePrimitive(base, index++, "_WiiEnableIRSensor", prWiiSetIRSensor, 2, 0);
	definePrimitive(base, index++, "_WiiEnableMotionSensor", prWiiSetMotionSensor, 2, 0);
	definePrimitive(base, index++, "_WiiEnableExpansion", prWiiSetExpansion, 2, 0);
}
#else // !SC_DARWIN
void initWiiPrimitives()
{
	//other platforms?
}
#endif // SC_DARWIN