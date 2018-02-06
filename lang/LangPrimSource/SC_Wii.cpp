// problem: communication with the event loop
// pipe cannot be read
// on MacOSX, I could use the EventLoopTimer instead, like in HID

// to try: put discovery within thread (so extra command in manager)

/*
 *  SC_Wii.cpp
 *  SC3lang
 *
 *  Created by Marije Baalman on Fri 18 May 2007.
 *  Copyright (c) 2007 Marije Baalman All rights reserved.
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

 */

#ifdef __APPLE__
    #include <Carbon/Carbon.h>
#endif

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


#ifdef HAVE_WII

#ifdef __APPLE__
	#include <mach/mach.h>
	#include <mach/mach_error.h>

	extern "C"{
	#include "WiiMote_OSX/wiiremote.h"
	}
#endif // __APPLE__

#ifdef __linux__
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	extern "C"{
	#include <cwiid.h>
// 	#include <wiimote.h>
// 	#include <wiimote_api.h>
// 	#include <wiimote_event.h>
// 	#include <wiimote_link.h>
	}
#endif // __linux__

// common:
#include <string.h>
#include <errno.h>


//--------- PyrSymbols ------------
PyrSymbol * s_wiiDisconnected;
PyrSymbol * s_wiiConnected;
PyrSymbol * s_handleEvent;

PyrSymbol * s_handleBatteryEvent;
PyrSymbol * s_handleExtensionEvent;
PyrSymbol * s_handleNunchukEvent;
PyrSymbol * s_handleClassicEvent;
PyrSymbol * s_handleIREvent;
PyrSymbol * s_handleAccEvent;
PyrSymbol * s_handleButtonEvent;

static PyrSymbol* s_wii = 0;
static PyrSymbol* s_wiiCalibrationInfoClass = 0;
static PyrSymbol* s_wiiLEDStateClass = 0;
static PyrSymbol* s_readError = 0;

extern bool compiledOK;
//int gNumberOfWiiDevices = 0;

#ifdef __APPLE__
EventLoopTimerRef gWiiTimer = NULL; // timer for element data updates
#endif


// --------------- SC_WII structure --------------
struct SC_WII
{
	SC_WII(PyrObject *obj);
	~SC_WII();

	bool open();
	bool close();
	bool wii_connect();
	bool wii_disconnect();
	int enable( bool enab );
	void disconnected();
	void connected();
	void get_address();
// 	void set_address( char * addr );
// 	void speaker_init( int format );

	bool update();

#ifdef __linux__
	void handleBatteryEvent( uint8_t battery );
	void handleButtonEvent( uint16_t buttons );
	void handleAccEvent( uint8_t acc[3] );
	void handleIREvent( int id, cwiid_ir_src ir );
	void handleNunchukEvent( struct cwiid_nunchuk_mesg nunchuk );
	void handleClassicEvent( struct cwiid_classic_mesg classic );
	void handleExtensionEvent( int ext_type );
#endif
#ifdef __APPLE__
// 	void handleBatteryEvent(  );
// 	void handleButtonEvent(  );
// 	void handleAccEvent(  );
// 	void handleIREvent(  );
// 	void handleNunchukEvent(  );
// 	void handleClassicEvent(  );
	void handleEvent();
#endif

	void readError();

	static PyrObject* getObject(PyrSlot* slot)
	{
		return isKindOfSlot(slot, s_wii->u.classobj) ? slotRawObject(slot) : 0;
	}

	static SC_WII* getDevice(PyrObject* obj)
	{
		return (SC_WII*)slotRawPtr(obj->slots);
	}

#ifdef __APPLE__
	WiiRemoteRef	m_wiiremote;
	char			m_address[32];
#endif
#ifdef __linux__
	cwiid_wiimote_t *m_wiiremote;
	int id;
	unsigned char rpt_mode;
	unsigned char led_state;
	char			m_address[19];
#endif
	bool			m_connected;
	int				m_searching;

	SC_WII*			m_next;
	PyrObject*		m_obj;
};

// ---------------- SC_WIIManager structure --------------

struct SC_WIIManager
{
public:
	static SC_WIIManager& instance();

	int start( float updtime );
	int stop();
#ifdef __linux__
	cwiid_wiimote_t *  discover();
#endif
#ifdef __APPLE__
	WiiRemoteRef discover();
#endif

	int add(SC_WII *dev);
	int remove(SC_WII *dev);

/// kind of private, but used in callback function and getWiiData
	SC_WII*			m_devices;

private:
	SC_WIIManager();
	~SC_WIIManager();

	void loop();

	bool		m_running;
	float		m_updatetime;
};


#ifdef __linux__
/// linux specific cwiid functions
#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))

void set_bit( unsigned char * bf, unsigned char b )
{
	if (!(*bf & b) )
		toggle_bit( *bf, b );
}

void clear_bit( unsigned char * bf, unsigned char b )
{
	if ((*bf & b) )
		toggle_bit( *bf, b );
}

cwiid_mesg_callback_t cwiid_callback;
void set_led_state(cwiid_wiimote_t *wiimote, unsigned char led_state);
void set_rpt_mode(cwiid_wiimote_t *wiimote, unsigned char rpt_mode);

cwiid_err_t err;
void err(cwiid_wiimote_t *wiimote, const char *s, va_list ap)
{
	if (wiimote) printf("%d:", cwiid_get_id(wiimote));
	else printf("-1:");
	vprintf(s, ap);
	printf("\n");
}

void set_led_state(cwiid_wiimote_t *wiimotet, unsigned char led_state)
{
// 	printf( "set_led_state %i\n", led_state );
	if (cwiid_set_led(wiimotet, led_state)) {
		post( "WII Error: Unable to set led state\n");
	}
}

void set_rpt_mode(cwiid_wiimote_t *wiimotet, unsigned char rpt_mode)
{
	if (cwiid_set_rpt_mode(wiimotet, rpt_mode)) {
		post( "WII Error: Unable to set report mode\n");
	}
}

#endif

#ifdef __APPLE__
void GetWii_Events();
static pascal void IdleTimerWii (EventLoopTimerRef inTimer, void* userData);
static EventLoopTimerUPP GetTimerUPPWii (void);

static pascal void IdleTimerWii (EventLoopTimerRef inTimer, void* userData)
{
	#pragma unused (inTimer, userData)
	GetWii_Events ();
}

static EventLoopTimerUPP GetTimerUPPWii (void)
{
	static EventLoopTimerUPP	sTimerUPP = NULL;

	if (sTimerUPP == NULL)
		sTimerUPP = NewEventLoopTimerUPP (IdleTimerWii);

	return sTimerUPP;
}

#endif


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
#ifdef __linux__
	cwiid_set_err(err);
#endif

#ifdef __APPLE__
	NumVersion	outSoftwareVersion;
	BluetoothHCIVersionInfo	outHardwareVersion;

// 	post("aka.wiiremote 1.0B6-UB by Masayuki Akamatsu");

	if (IOBluetoothGetVersion(&outSoftwareVersion, &outHardwareVersion)==kIOReturnSuccess)
	{
		if (outSoftwareVersion.majorRev < 1 && outSoftwareVersion.minorAndBugRev < 0x63)
		{
			error("WII: requires Bluetooth version 1.6.3 or later.");
			return;
		}
	}
	else
	{
		error("WII: can't get Bluetooth version.");
		return;
	}
#endif

}

SC_WIIManager::~SC_WIIManager()
{
#ifdef __APPLE__
	if (gWiiTimer)
    	{
		RemoveEventLoopTimer(gWiiTimer);
		gWiiTimer = NULL;
	}
#endif
}

int SC_WIIManager::start( float updtime )
{

/// nothing to do for Linux

#ifdef __APPLE__
	m_updatetime = (updtime / 1000); // convert to seconds
	post( "WII: eventloop updatetime %f\n", m_updatetime );
	//double eventtime = (double) updtime;
	if(gWiiTimer)
	{
        RemoveEventLoopTimer(gWiiTimer);
		gWiiTimer = NULL;
	}
	InstallEventLoopTimer (GetCurrentEventLoop(), 0, (EventTimerInterval) m_updatetime, GetTimerUPPWii (), 0, &gWiiTimer);
#endif

	return errNone;
}

#ifdef __linux__
cwiid_wiimote_t * SC_WIIManager::discover()
{
	bdaddr_t bdaddr = {0, 0, 0, 0, 0, 0};
	cwiid_wiimote_t* wiimotediscovered;

	if ( (wiimotediscovered = cwiid_open(&bdaddr, 0)) == NULL ) {
		return NULL;
		}
	if (cwiid_set_mesg_callback(wiimotediscovered, cwiid_callback)) {
		post("ERROR: WII: Unable to set message callback\n");
		if ( cwiid_close(wiimotediscovered) ){
			post("ERROR: WII: Unable to close Wii\n");
			}
		return NULL;
		}

/// adding these as devices in the primitive! this is the only way to be able to connect, as the connection uses much more info than I thought!

	return( wiimotediscovered );
}
#endif

#ifdef __APPLE__
WiiRemoteRef SC_WIIManager::discover()
{
/*	WiiRemoteRef newwii;
	char address[32];
	newwii = (WiiRemoteRef)malloc(sizeof(WiiRemoteRec));
	if (newwii != NULL)
		{
		wiiremote_init(newwii);
		bool	result;
		result = wiiremote_search( newwii, address);	// start searching the device
//		m_searching++;
		post("WII: searching wiimote %i\n",result);

		}
	return( newwii );
	*/
}
#endif


int SC_WIIManager::stop()
{
/// nothing to do on LINUX
#ifdef __APPLE__
 	if (gWiiTimer)
 	{
 		RemoveEventLoopTimer(gWiiTimer);
 		gWiiTimer = NULL;
 	}
#endif
	return errNone;
}

int SC_WIIManager::add(SC_WII* dev)
{
// 	post("WII: SC_WIIManager::add\n");

	if (dev->m_next) return false;
	if (m_devices == dev ) {
		m_devices = dev;
		}
	else {
		dev->m_next = m_devices;
		m_devices = dev;
	}

#ifdef __linux__
 	dev->id = cwiid_get_id( dev->m_wiiremote );

	if (cwiid_enable(dev->m_wiiremote, CWIID_FLAG_MESG_IFC)) {
		post("ERROR: WII: Unable to enable Wii\n");
		}

	dev->rpt_mode = CWIID_RPT_STATUS;
	dev->led_state = 0;
	set_rpt_mode( dev->m_wiiremote, dev->rpt_mode );
	set_led_state( dev->m_wiiremote, dev->led_state );
#endif

//#ifdef __APPLE__
//	dev->wii_connect();
//#endif

	return true;

}

int SC_WIIManager::remove(SC_WII* dev)
{
	SC_WII *prev = 0, *cur = m_devices;

	while (cur) {
		if (cur == dev) {
			if (prev) prev->m_next = dev->m_next;
			else m_devices = dev->m_next;
			dev->m_next = 0;
			delete dev;
			return errNone;
		}
		prev = cur;
		cur = cur->m_next;
	}

	return errFailed;
}

#ifdef __linux__
void cwiid_callback(cwiid_wiimote_t *wiimotet, int mesg_count,
                    union cwiid_mesg mesg[], struct timespec *timestamp)
{
	int i, j;
	int valid_source;
	int thisid, id;

	SC_WII *dev = SC_WIIManager::instance().m_devices;

	id = 0;
	thisid = cwiid_get_id( wiimotet );
	while (dev) {
		if ( thisid == dev->id )
			{
			break;
			}
		dev = dev->m_next;
		}

	for (i=0; i < mesg_count; i++)
	{
	switch (mesg[i].type) {
		case CWIID_MESG_STATUS:
			dev->handleBatteryEvent( mesg[i].status_mesg.battery );
 			dev->handleExtensionEvent( mesg[i].status_mesg.ext_type );
			break;
		case CWIID_MESG_BTN:
			dev->handleButtonEvent( mesg[i].btn_mesg.buttons );
			break;
		case CWIID_MESG_ACC:
			dev->handleAccEvent( mesg[i].acc_mesg.acc );
			break;
		case CWIID_MESG_IR:
			for (j = 0; j < CWIID_IR_SRC_COUNT; j++) {
				dev->handleIREvent( j, mesg[i].ir_mesg.src[j] );
			}
			break;
		case CWIID_MESG_NUNCHUK:
			dev->handleNunchukEvent( mesg[i].nunchuk_mesg );
			break;
		case CWIID_MESG_CLASSIC:
			dev->handleClassicEvent( mesg[i].classic_mesg );
			break;
		case CWIID_MESG_ERROR:
			dev->disconnected();
			dev->readError();
			break;
		default:
			post("WII: Unknown Report");
			break;
		}
	}
}
#endif

#ifdef __APPLE__
void GetWii_Events (){
	SC_WII *dev = SC_WIIManager::instance().m_devices;
// 	 m_devices;
	int debugcnt = 0;
	while (dev) {
		bool	connection;
//		post( "WII: device %i, t %p, w %p, n %p\n", debugcnt, dev, dev->m_wiiremote, dev->m_next);
		debugcnt++;
		if ( dev->m_wiiremote != NULL )
			{
			connection = wiiremote_isconnected(dev->m_wiiremote);
			if ( dev->m_connected == false && connection == true)	// if the device is connected, but wasn't before
				{
	//			post( "WII: wiimote got connected\n");
				wiiremote_getstatus(dev->m_wiiremote);
				dev->connected();
				}
			else if (dev->m_connected == true && connection == false) // if device was disconnected
				{
	//			post( "WII: wiimote got disconnected\n");
				dev->disconnected();
				}
			else if ( dev->m_searching > 0 )
				{
				dev->wii_connect();
				}
			if ( dev->m_connected ) {
	//			post( "WII: wiimote is connected\n");
				wiiremote_getstatus(dev->m_wiiremote);
				dev->handleEvent();
				}
			else {
	//			post("WII: wiimote not connected\n");
				}
			}
		else
			{
	//		post("WII: read error\n");
			dev->readError();
			}
		dev = dev->m_next;
		}
}
#endif

//---------------- SC_WII functions -----------------

SC_WII::SC_WII(PyrObject* obj)
	: m_next(0),
	  m_obj(obj)
{
// #ifdef __linux__
// 	m_report = nil;
// #endif
	m_wiiremote = NULL;
	m_searching = 0;
	m_connected = false;
// 	m_address;
	SetPtr(obj->slots+0, this);
}

SC_WII::~SC_WII()
{
	close();
}

bool SC_WII::open()
{
#ifdef __APPLE__
	m_wiiremote = (WiiRemoteRef)malloc(sizeof(WiiRemoteRec));
	if (m_wiiremote != NULL)
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
// 		return SC_WIIManager::instance().add(this);
		}
#endif
#ifdef __linux__
//  	m_wiiremote = (wiimote_t*)malloc(sizeof(wiimote_t));
// 	m_report = (wiimote_report_t)malloc(sizeof(wiimote_report_t));
//  	if ( m_wiiremote != NULL )
//  		{
// 		*m_wiiremote = WIIMOTE_INIT;
// 		m_report = WIIMOTE_REPORT_INIT;
// 		m_connected = false;
// 		return SC_WIIManager::instance().add(this);
//  		}
#endif
	return( false );
}

bool SC_WII::close()
{
	if (m_wiiremote != NULL)
	{
#ifdef __APPLE__
		if (wiiremote_isconnected(m_wiiremote))
			wiiremote_disconnect(m_wiiremote);
		; //m_wiiremote, sizeof(WiiRemoteRec));
		m_wiiremote = NULL;
		m_searching = 0;
#endif
#ifdef __linux__
		if ( cwiid_close( m_wiiremote ) ){
// 			m_wiiremote = NULL;
			post( "error closing device\n" );
			fflush( stdout );
			return errFailed;
		}
		m_wiiremote = NULL;
// 		return errNone;
// 		if (wiimote_is_open(m_wiiremote))
// 			wiimote_disconnect(m_wiiremote);
#endif

	}
// 	SetNil(m_obj->slots+0);
	return SC_WIIManager::instance().remove(this);
}

bool SC_WII::wii_connect()
{
#ifdef __APPLE__
	if (wiiremote_isconnected(m_wiiremote))
	{
		post("WII: wiimote is already connected\n");
		m_connected = true;
	}
	else
	{
		bool	result;
		result = wiiremote_search( m_wiiremote, m_address);	// start searching the device
		m_searching++;
		post("WII: searching wiimote %i\n",result);
		if ( result )
			m_connected = true;
		else
			m_connected = false;
	}
#endif
#ifdef __linux__
// 	if (wiimote_connect(m_wiiremote, m_address) < 0) {
// 		post("WII: unable to open wiimote: %s at address %s\n", wiimote_get_error(), m_address);
// 		post("WII: unable to open wiimote at address %s\n",m_address);
// 		m_connected = false;
// 	}
// 	else
		m_connected = true;
#endif
	return( m_connected );
}

void SC_WII::connected()
{
	bool result;
	m_connected = true;
	m_searching = 0;
#ifdef __APPLE__
	result = wiiremote_led( m_wiiremote, 0, 0, 0, 0);
//	if ( !result )
//		wii_disconnect();
#endif
// 	post("WII: wiiremote connected\n");
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_wiiConnected, 1);
		g->canCallOS = false;
	}
	gLangMutex.unlock();
}

bool SC_WII::wii_disconnect()
{
	bool	result = false;
	m_searching = 0;
	/// TODO: remove from wii_manager

#ifdef __APPLE__
	wiiremote_stopsearch( m_wiiremote );
	result = wiiremote_disconnect( m_wiiremote);
#endif
#ifdef __linux__
// 	result = wiimote_disconnect(m_wiiremote);
#endif

	// call disconnect action
	if ( result )
		disconnected();
	return( result );
}

int SC_WII::enable( bool enab )
{
#ifdef __linux__
	if ( enab ) {
		if (cwiid_enable(m_wiiremote, CWIID_FLAG_MESG_IFC)) {
		return errFailed;
		}
	} else {
		if (cwiid_disable(m_wiiremote, CWIID_FLAG_MESG_IFC)) {
		return errFailed;
		}
	}
	return errNone;
#endif
	return errNone;
}

void SC_WII::disconnected()
{
	m_connected = false;
	m_searching = 0;
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_wiiDisconnected, 1);
		g->canCallOS = false;
	}
	gLangMutex.unlock();
}

// void SC_WII::speaker_init( int format )
// {
// 	if ( format == 4 )
// 		wiimote_speaker_init(m_wiiremote, WIIMOTE_FMT_S4, 0xff);
// 	else if ( format == 8 )
// 		wiimote_speaker_init(m_wiiremote, WIIMOTE_FMT_S8, 0xff);
// }

void SC_WII::get_address()
{
#ifdef __APPLE__
	if (m_wiiremote->device == NULL)
	{
		return;
	}
	else
	{
		char	str[32];
		wiiremote_getaddress(m_wiiremote, str);
		strcpy(m_address, str);
	}
#endif
}

// bool SC_WII::update()
// {
// 	if (wiimote_update(m_wiiremote) < 0) {
// 		wiimote_disconnect(m_wiiremote);
// 		post( "WII: wiimote got disconnected\n");
// 		disconnected();
// 		return( false );
// 	}
// 	return( true );
// }

// void SC_WII::set_address( char * addr )
// {
// 	strcpy( m_address, addr );
// // 	post( "WII: addr %s, m_address %s\n", addr, m_address );
// }

#ifdef __APPLE__
void SC_WII::handleEvent()
{
	if (m_wiiremote->device == NULL)
		return;	// do nothing

	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		// buttons
//		post( "buttondata %i\n", m_wiiremote->buttonData);
//		++g->sp; SetInt(g->sp, m_wiiremote->buttonData);
		PyrObject *butArray = newPyrArray(g->gc, 11 * sizeof(int), 0, true);
		PyrSlot *butArraySlots = butArray->slots;
		SetInt(butArray->slots+butArray->size++, (int) ((0x0008 & m_wiiremote->buttonData) > 0) ); //A
		SetInt(butArray->slots+butArray->size++, (int) ((0x0004 & m_wiiremote->buttonData) > 0) ); //B
		SetInt(butArray->slots+butArray->size++, (int) ((0x0002 & m_wiiremote->buttonData) > 0) ); //1
		SetInt(butArray->slots+butArray->size++, (int) ((0x0001 & m_wiiremote->buttonData) > 0) ); //2
		SetInt(butArray->slots+butArray->size++, (int) ((0x0010 & m_wiiremote->buttonData) > 0) ); //minus
		SetInt(butArray->slots+butArray->size++, (int) ((0x0080 & m_wiiremote->buttonData) > 0) ); //home
		SetInt(butArray->slots+butArray->size++, (int) ((0x1000 & m_wiiremote->buttonData) > 0) ); // plus
		SetInt(butArray->slots+butArray->size++, (int) ((0x0800 & m_wiiremote->buttonData) > 0) ); // up
		SetInt(butArray->slots+butArray->size++, (int) ((0x0400 & m_wiiremote->buttonData) > 0) ); // down
		SetInt(butArray->slots+butArray->size++, (int) ((0x0100 & m_wiiremote->buttonData) > 0) ); // left
		SetInt(butArray->slots+butArray->size++, (int) ((0x0200 & m_wiiremote->buttonData) > 0) ); // right
		butArray->size = 11;
		++g->sp; SetObject(g->sp, butArray);


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
			++g->sp; SetFloat(g->sp, (float) m_wiiremote->accX / 256);
			++g->sp; SetFloat(g->sp, (float) m_wiiremote->accY / 256);
			++g->sp; SetFloat(g->sp, (float) m_wiiremote->accZ / 256);
			++g->sp; SetInt(g->sp, m_wiiremote->orientation);
		} else {
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetFloat(g->sp, 0);
			++g->sp; SetInt(g->sp, 0);
		}
		if (m_wiiremote->isExpansionPortAttached && m_wiiremote->isExpansionPortEnabled)
			{
			++g->sp; SetInt(g->sp, m_wiiremote->expType);
			// Classic Controller
			if (m_wiiremote->expType == WiiClassicController)
				{
				// buttons
				//++g->sp; SetInt(g->sp, m_wiiremote->cButtonData);
				PyrObject *outArray = newPyrArray(g->gc, 15 * sizeof(char), 0, true);
				PyrSlot *outArraySlots = outArray->slots;
				SetInt(outArray->slots+outArray->size++, (int) ((0x0008 & m_wiiremote->cButtonData) > 0) ); //X
				SetInt(outArray->slots+outArray->size++, (int) ((0x0020 & m_wiiremote->cButtonData) > 0) ); //Y
				SetInt(outArray->slots+outArray->size++, (int) ((0x0010 & m_wiiremote->cButtonData) > 0) ); //A
				SetInt(outArray->slots+outArray->size++, (int) ((0x0040 & m_wiiremote->cButtonData) > 0) ); //B
				SetInt(outArray->slots+outArray->size++, (int) ((0x2000 & m_wiiremote->cButtonData) > 0) ); //L

				SetInt(outArray->slots+outArray->size++, (int) ((0x0200 & m_wiiremote->cButtonData) > 0) ); //R
				SetInt(outArray->slots+outArray->size++, (int) ((0x0080 & m_wiiremote->cButtonData) > 0) ); //ZL
				SetInt(outArray->slots+outArray->size++, (int) ((0x0004 & m_wiiremote->cButtonData) > 0) ); //ZR
				SetInt(outArray->slots+outArray->size++, (int) ((0x0001 & m_wiiremote->cButtonData) > 0) ); //Up
				SetInt(outArray->slots+outArray->size++, (int) ((0x4000 & m_wiiremote->cButtonData) > 0) ); //Down

				SetInt(outArray->slots+outArray->size++, (int) ((0x0002 & m_wiiremote->cButtonData) > 0) ); //Left
				SetInt(outArray->slots+outArray->size++, (int) ((0x8000 & m_wiiremote->cButtonData) > 0) );//Right
				SetInt(outArray->slots+outArray->size++, (int) ((0x1000 & m_wiiremote->cButtonData) > 0) );//Minus
				SetInt(outArray->slots+outArray->size++, (int) ((0x0800 & m_wiiremote->cButtonData) > 0) );//Home
				SetInt(outArray->slots+outArray->size++, (int) ((0x0400 & m_wiiremote->cButtonData) > 0) );//Plus

				outArray->size = 15;
				++g->sp; SetObject(g->sp, outArray);

				// Joystick 1
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cStickX1 / 0x3F);
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cStickY1 / 0x3F);

				// Joystick 2
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cStickX2 / 0x1F);
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cStickY2 / 0x1F);

				// Analog
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cAnalogL / 0x1F);
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->cAnalogR / 0x1F);
			}
			// Nunchuk
			if (m_wiiremote->expType == WiiNunchuk)
			{
				// Buttons
				//++g->sp; SetInt(g->sp, m_wiiremote->nButtonData);
				PyrObject *butArrayN = newPyrArray(g->gc, 2 * sizeof(int), 0, true);
				PyrSlot *butArraySlotsN = butArrayN->slots;
				SetInt(butArrayN->slots+butArrayN->size++, (int) ((0x01 & m_wiiremote->nButtonData) < 1) );
				SetInt(butArrayN->slots+butArrayN->size++, (int) ((0x02 & m_wiiremote->nButtonData) < 1) );
				butArrayN->size = 2;
				++g->sp; SetObject(g->sp, butArrayN);

				// Joystick
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->nStickX / 256);
				++g->sp; SetFloat(g->sp, (float) m_wiiremote->nStickY / 256);

				// Motion Sensor
				if (m_wiiremote->isMotionSensorEnabled)
				{
					++g->sp; SetFloat(g->sp, (float) m_wiiremote->nAccX / 256);
					++g->sp; SetFloat(g->sp, (float) m_wiiremote->nAccY / 256);
					++g->sp; SetFloat(g->sp, (float) m_wiiremote->nAccZ / 256);
					++g->sp; SetInt(g->sp, m_wiiremote->nOrientation);
				} else {
					++g->sp; SetFloat(g->sp, 0);
					++g->sp; SetFloat(g->sp, 0);
					++g->sp; SetFloat(g->sp, 0);
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
			++g->sp; SetInt(g->sp, 0);
		}
	++g->sp; SetFloat(g->sp, m_wiiremote->batteryLevel);
	runInterpreter(g, s_handleEvent, 19);
	g->canCallOS = false;
	}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleBatteryEvent( uint8_t battery ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		++g->sp; SetFloat(g->sp, (float) battery / CWIID_BATTERY_MAX );
		runInterpreter(g, s_handleBatteryEvent, 2);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleExtensionEvent( int ext_type ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		++g->sp; SetInt(g->sp, ext_type );
		runInterpreter(g, s_handleExtensionEvent, 2);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleButtonEvent( uint16_t buttons ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);

// post( "handle Button Event\n" );
// fflush( stdout );

// 		++g->sp; SetInt(g->sp, (int) buttons );
		PyrObject *outArray = newPyrArray(g->gc, 11 * sizeof(int), 0, true);
		PyrSlot *outArraySlots = outArray->slots;
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_A & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_B & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_1 & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_2 & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_MINUS & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_HOME & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_PLUS & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_UP & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_DOWN & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_LEFT & buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_BTN_RIGHT & buttons) > 0) );
		outArray->size = 11;
		++g->sp; SetObject(g->sp, outArray);

		runInterpreter(g, s_handleButtonEvent, 2);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleAccEvent( uint8_t acc[3] ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		++g->sp; SetFloat(g->sp, (float) acc[CWIID_X]/CWIID_ACC_MAX );
		++g->sp; SetFloat(g->sp, (float) acc[CWIID_Y]/CWIID_ACC_MAX );
		++g->sp; SetFloat(g->sp, (float) acc[CWIID_Z]/CWIID_ACC_MAX );
		runInterpreter(g, s_handleAccEvent, 4);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleNunchukEvent( struct cwiid_nunchuk_mesg nunchuk ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);

		PyrObject *outArray = newPyrArray(g->gc, 2 * sizeof(int), 0, true);
		PyrSlot *outArraySlots = outArray->slots;
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_NUNCHUK_BTN_Z & nunchuk.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_NUNCHUK_BTN_C & nunchuk.buttons) > 0) );
		outArray->size = 2;
		++g->sp; SetObject(g->sp, outArray);
// 		++g->sp; SetInt(g->sp, (int) nunchuk.buttons);
		++g->sp; SetFloat(g->sp, (float) nunchuk.stick[CWIID_X]/256);
		++g->sp; SetFloat(g->sp, (float) nunchuk.stick[CWIID_Y]/256);
		++g->sp; SetFloat(g->sp, (float) nunchuk.acc[CWIID_X]/CWIID_ACC_MAX );
		++g->sp; SetFloat(g->sp, (float) nunchuk.acc[CWIID_Y]/CWIID_ACC_MAX );
		++g->sp; SetFloat(g->sp, (float) nunchuk.acc[CWIID_Z]/CWIID_ACC_MAX );
		runInterpreter(g, s_handleNunchukEvent, 7);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleClassicEvent( struct cwiid_classic_mesg classic ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		PyrObject *outArray = newPyrArray(g->gc, 15 * sizeof(char), 0, true);
		PyrSlot *outArraySlots = outArray->slots;
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_X & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_Y & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_A & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_B & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_L & classic.buttons) > 0) );

		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_R & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_ZL & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_ZR & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_UP & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_DOWN & classic.buttons) > 0) );

		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_LEFT & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_RIGHT & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_MINUS & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_HOME & classic.buttons) > 0) );
		SetInt(outArray->slots+outArray->size++, (int) ((CWIID_CLASSIC_BTN_PLUS & classic.buttons) > 0) );

		outArray->size = 15;
		++g->sp; SetObject(g->sp, outArray);

// 		++g->sp; SetInt(g->sp, (int) classic.buttons);
		++g->sp; SetFloat(g->sp, (float) classic.l_stick[CWIID_X]/CWIID_CLASSIC_L_STICK_MAX );
		++g->sp; SetFloat(g->sp, (float) classic.l_stick[CWIID_Y]/CWIID_CLASSIC_L_STICK_MAX );
		++g->sp; SetFloat(g->sp, (float) classic.r_stick[CWIID_X]/CWIID_CLASSIC_R_STICK_MAX );
		++g->sp; SetFloat(g->sp, (float) classic.r_stick[CWIID_Y]/CWIID_CLASSIC_R_STICK_MAX );
		++g->sp; SetFloat(g->sp, (float) classic.l/CWIID_CLASSIC_LR_MAX );
		++g->sp; SetFloat(g->sp, (float) classic.r/CWIID_CLASSIC_LR_MAX );
		runInterpreter(g, s_handleClassicEvent, 8);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

#ifdef __linux__
void SC_WII::handleIREvent( int id, cwiid_ir_src ir ){
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
		++g->sp; SetObject(g->sp, m_obj);
		++g->sp; SetInt(g->sp, id);
		++g->sp; SetInt(g->sp, ir.valid);
		++g->sp; SetFloat(g->sp, (float) ir.pos[CWIID_X]/CWIID_IR_X_MAX);
		++g->sp; SetFloat(g->sp, (float) ir.pos[CWIID_Y]/CWIID_IR_Y_MAX);
		++g->sp; SetFloat(g->sp, (float) ir.size/256);
		runInterpreter(g, s_handleIREvent, 6);
		g->canCallOS = false;
		}
	gLangMutex.unlock();
}
#endif

void SC_WII::readError()
{
	gLangMutex.lock();
	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		g->canCallOS = false;
 		++g->sp; SetObject(g->sp, m_obj);
		runInterpreter(g, s_readError, 1);
		g->canCallOS = false;
	}
	gLangMutex.unlock();
// 	SC_WIIManager::instance().remove( this );
}



//------------ primitives ---------------

int prWii_Start(VMGlobals* g, int numArgsPushed)
{
	float updtime;
	int err;
 	PyrSlot* args = g->sp - 1;
//  	PyrSlot* args = g->sp;

 	err = slotFloatVal(args+1, &updtime);
 	if (err) return err;

	post( "update time %f", updtime );

//  	if (!g->canCallOS) return errCantCallOS;
// 	return SC_WIIManager::instance().start();
	return SC_WIIManager::instance().start(updtime);
}

int prWii_Stop(VMGlobals* g, int numArgsPushed)
{
// 	if (!g->canCallOS) return errCantCallOS;
	return SC_WIIManager::instance().stop();
}

int prWii_Discover(VMGlobals* g, int numArgsPushed)
{
	int curid, nmotes;
	int err;
 	PyrSlot* args = g->sp - 1;

	err = slotIntVal(args, &curid);
	if (err) return err;

	if (!isKindOfSlot(args+1, class_array))
		return errWrongType;
	PyrObject* allDevsArray = slotRawObject(&args[1]);
	PyrSlot* slotsArray = allDevsArray->slots;

#ifdef __linux__
	cwiid_wiimote_t * thiswii;

	thiswii = SC_WIIManager::instance().discover();
	if ( thiswii == NULL ){
		SetFalse(g->sp-2);
		post( "no device found\n" );
		return errNone;
	}
#endif

	if ( !isKindOfSlot(slotsArray+curid, s_wii->u.classobj ) ) {
		SetFalse(g->sp-2);
		return errWrongType;
	}
	PyrObject* obj = SC_WII::getObject(slotsArray+curid);
	SC_WII* dev = SC_WII::getDevice(obj);

#ifdef __APPLE__
	dev->wii_connect();
#endif

 //	post( "dev %p, wii %p\n", dev, dev->m_wiiremote );

// 	if (!dev) return errFailed;
// 	free( dev->m_wiiremote );
#ifdef __linux__
	dev->m_wiiremote = thiswii;
#endif
	if ( SC_WIIManager::instance().add( dev ) ) {
		post( "device added\n" );
		SetTrue(g->sp-2);
	} else {
		SetFalse(g->sp-2);
		post( "device was already added\n" );
	}

	return errNone;
}

int prWii_Open(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

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
	PyrSlot* args = g->sp;
	int err;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	dev->get_address();

	SetSymbol(args, getsym(dev->m_address));

	return errNone;
}


// int prWiiSetAddress(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// #ifdef __linux__
// 	char path[19];
// #endif
// #ifdef __APPLE__
// 	char path[32];
// #endif
// 	err = slotStrVal(args+1, path, sizeof(path));
// 	if (err) return err;
// // 	post( "WII: address %s\n", path );
//
// 	dev->set_address( path );
//
// 	return errNone;
// }

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
	PyrObject* infoObj = slotRawObject(&args[1]);

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
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
#endif
#ifdef __linux__
/*		SetInt(infoObj->slots+0, dev->m_wiiremote->cal.x_zero);
		SetInt(infoObj->slots+1, dev->m_wiiremote->cal.y_zero);
		SetInt(infoObj->slots+2, dev->m_wiiremote->cal.z_zero);
		SetInt(infoObj->slots+3, dev->m_wiiremote->cal.x_scale);
		SetInt(infoObj->slots+4, dev->m_wiiremote->cal.y_scale);
		SetInt(infoObj->slots+5, dev->m_wiiremote->cal.z_scale);

		if (dev->m_wiiremote->mode.ext == 1)
		{
			SetInt(infoObj->slots+6, dev->m_wiiremote->ext.nunchuk.cal.x_zero);
			SetInt(infoObj->slots+7, dev->m_wiiremote->ext.nunchuk.cal.y_zero);
			SetInt(infoObj->slots+8, dev->m_wiiremote->ext.nunchuk.cal.z_zero);
			SetInt(infoObj->slots+9, dev->m_wiiremote->ext.nunchuk.cal.x_scale);
			SetInt(infoObj->slots+10, dev->m_wiiremote->ext.nunchuk.cal.y_scale);
			SetInt(infoObj->slots+11, dev->m_wiiremote->ext.nunchuk.cal.z_scale);

			SetInt(infoObj->slots+12, dev->m_wiiremote->ext.nunchuk.cal.joyx_max);
			SetInt(infoObj->slots+13, dev->m_wiiremote->ext.nunchuk.cal.joyx_min);
			SetInt(infoObj->slots+14, dev->m_wiiremote->ext.nunchuk.cal.joyx_center);
			SetInt(infoObj->slots+15, dev->m_wiiremote->ext.nunchuk.cal.joyy_max);
			SetInt(infoObj->slots+16, dev->m_wiiremote->ext.nunchuk.cal.joyy_min);
			SetInt(infoObj->slots+17, dev->m_wiiremote->ext.nunchuk.cal.joyy_center);
		}*/
#endif
	}

	slotCopy(&args[0], &args[1]);
	return errNone;
}

// int prWiiGetLED(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* leds = slotRawObject(&args[1]);
// 	PyrSlot* bslots = leds->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->led.one);
// 		SetInt(bslots+1, dev->m_wiiremote->led.two);
// 		SetInt(bslots+2, dev->m_wiiremote->led.three);
// 		SetInt(bslots+3, dev->m_wiiremote->led.four);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetButtons(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->keys.left);
// 		SetInt(bslots+1, dev->m_wiiremote->keys.right);
// 		SetInt(bslots+2, dev->m_wiiremote->keys.down);
// 		SetInt(bslots+3, dev->m_wiiremote->keys.up);
// 		SetInt(bslots+4, dev->m_wiiremote->keys.plus);
// 		SetInt(bslots+5, dev->m_wiiremote->keys.two);
// 		SetInt(bslots+6, dev->m_wiiremote->keys.one);
// 		SetInt(bslots+7, dev->m_wiiremote->keys.b);
// 		SetInt(bslots+8, dev->m_wiiremote->keys.a);
// 		SetInt(bslots+9, dev->m_wiiremote->keys.minus);
// 		SetInt(bslots+10, dev->m_wiiremote->keys.home);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetMotion(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->axis.x);
// 		SetInt(bslots+1, dev->m_wiiremote->axis.y);
// 		SetInt(bslots+2, dev->m_wiiremote->axis.z);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetIR(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->ir1.x);
// 		SetInt(bslots+1, dev->m_wiiremote->ir1.y);
// 		SetInt(bslots+2, dev->m_wiiremote->ir1.size);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetNunchukButtons(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->ext.nunchuk.keys.z);
// 		SetInt(bslots+1, dev->m_wiiremote->ext.nunchuk.keys.c);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetNunchukJoy(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->ext.nunchuk.joyx);
// 		SetInt(bslots+1, dev->m_wiiremote->ext.nunchuk.joyy);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetNunchukMotion(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// // 	if (!isKindOfSlot(args+1, s_wiiLEDStateClass->u.classobj))
// // 		return errWrongType;
// // 	PyrObject* infoObj = slotRawObject(&args[1]);
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (!isKindOfSlot(args+1, class_array))
// 		return errWrongType;
// 	PyrObject* buttons = slotRawObject(&args[1]);
// 	PyrSlot* bslots = buttons->slots;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// // 		SetInt(bslots+0, dev->m_wiiremote->isLED1Illuminated);
// // 		SetInt(bslots+1, dev->m_wiiremote->isLED2Illuminated);
// // 		SetInt(bslots+2, dev->m_wiiremote->isLED3Illuminated);
// // 		SetInt(bslots+3, dev->m_wiiremote->isLED4Illuminated);
// #endif
// #ifdef __linux__
// 		SetInt(bslots+0, dev->m_wiiremote->ext.nunchuk.axis.x);
// 		SetInt(bslots+1, dev->m_wiiremote->ext.nunchuk.axis.y);
// 		SetInt(bslots+2, dev->m_wiiremote->ext.nunchuk.axis.z);
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetExpansion(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// 		if (dev->m_wiiremote->isExpansionPortAttached)
// 			SetInt( args, dev->m_wiiremote->expType );
// 		else
// 			SetInt( args, 0 );
// #endif
// // #ifdef __linux__
// // 		if ( dev->m_wiiremote->mode.ext == 1 )
// // 			SetInt( args, dev->m_wiiremote->ext.id );
// // 		else
// // 			SetInt( args, 0 );
// // #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiGetBattery(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp;
// 	int err;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// #ifdef __APPLE__
// 		if (dev->m_wiiremote->batteryLevel)
// 			SetFloat( args, dev->m_wiiremote->batteryLevel );
// 		else
// 			SetFloat( args, 0 );
// #endif
// // #ifdef __linux__
// // 		SetFloat( args, dev->m_wiiremote->battery );
// // #endif
// 	}
//
// 	return errNone;
// }

int prWiiSetLED(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	bool result;
	int enable1, enable2, enable3, enable4;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	if (!isKindOfSlot(args+1, class_array))
		return errWrongType;
	PyrObject* leds = slotRawObject(&args[1]);
	PyrSlot* bslots = leds->slots;

	err = slotIntVal( bslots+0, &enable1 );
	if (err) return err;

	err = slotIntVal( bslots+1, &enable2 );
	if (err) return err;

	err = slotIntVal( bslots+2, &enable3 );
	if (err) return err;

	err = slotIntVal( bslots+3, &enable4 );
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
		result = wiiremote_led( dev->m_wiiremote, enable1, enable2, enable3, enable4);
//		if ( !result )
//			dev->wii_disconnect();
#endif
#ifdef __linux__
	if ( enable1 )
		set_bit(&dev->led_state, CWIID_LED1_ON);
	else
		clear_bit(&dev->led_state, CWIID_LED1_ON);
	if ( enable2 )
		set_bit(&dev->led_state, CWIID_LED2_ON);
	else
		clear_bit(&dev->led_state, CWIID_LED2_ON);
	if ( enable3 )
		set_bit(&dev->led_state, CWIID_LED3_ON);
	else
		clear_bit(&dev->led_state, CWIID_LED3_ON);
	if ( enable4 )
		set_bit(&dev->led_state, CWIID_LED4_ON);
	else
		clear_bit(&dev->led_state, CWIID_LED4_ON);
	set_led_state(dev->m_wiiremote, dev->led_state);
// 	post( "WII: led %i %i %i %i", dev->m_wiiremote->led.one, dev->m_wiiremote->led.two, dev->m_wiiremote->led.three, dev->m_wiiremote->led.four  );
// 		dev->update();
#endif
	}

	return errNone;
}

int prWiiSetVibration(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
		result = wiiremote_vibration( dev->m_wiiremote, enable1 );
//		if ( !result )
//			dev->wii_disconnect();
// 	post( "WII: rumble %i %i", enable1, result );
#endif
#ifdef __linux__
		if (cwiid_set_rumble(dev->m_wiiremote, (unsigned char) enable1)) {
			return errFailed;
		}
// 	post( "WII: rumble %i %i", dev->m_wiiremote->rumble, enable1 );
#endif
	}
	return errNone;
}

int prWiiSetExpansion(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
		result = wiiremote_expansion( dev->m_wiiremote, enable1 );
//		if ( !result )
//			dev->wii_disconnect();
#endif
#ifdef __linux__
		if ( enable1 )
			set_bit(&dev->rpt_mode, CWIID_RPT_EXT);
		else
			clear_bit(&dev->rpt_mode, CWIID_RPT_EXT);
		set_rpt_mode(dev->m_wiiremote, dev->rpt_mode);
// 	post( "WII: expansion %i %i", dev->m_wiiremote->mode.ext, enable1 );
#endif
	}

	return errNone;
}

int prWiiSetIRSensor(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
		result = wiiremote_irsensor( dev->m_wiiremote, enable1 );
//		if ( !result )
//			dev->wii_disconnect();

#endif
#ifdef __linux__
		if ( enable1 )
			set_bit(&dev->rpt_mode, CWIID_RPT_IR);
		else
			clear_bit(&dev->rpt_mode, CWIID_RPT_IR);
		set_rpt_mode(dev->m_wiiremote, dev->rpt_mode);
// 	post( "WII: ir sensor %i %i", dev->m_wiiremote->mode.ir, enable1 );
#endif
	}

	return errNone;
}

int prWiiSetMotionSensor(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
//		if ( !result )
//			dev->wii_disconnect();
#endif
#ifdef __linux__
		if ( enable1 )
			set_bit(&dev->rpt_mode, CWIID_RPT_ACC);
		else
			clear_bit(&dev->rpt_mode, CWIID_RPT_ACC);
		set_rpt_mode(dev->m_wiiremote, dev->rpt_mode);
#endif
	}

	return errNone;
}

int prWiiSetButtons(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
// buttons are always enabled
// 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
#endif
#ifdef __linux__
		if ( enable1 )
			set_bit(&dev->rpt_mode, CWIID_RPT_BTN);
		else
			clear_bit(&dev->rpt_mode, CWIID_RPT_BTN);
		set_rpt_mode(dev->m_wiiremote, dev->rpt_mode);
#endif
	}

	return errNone;
}

int prWiiEnable(VMGlobals *g, int numArgsPushed)
{
	PyrSlot* args = g->sp - 1;
	int err;
	int enable1;
	bool result;

	PyrObject* obj = SC_WII::getObject(args+0);
	if (!obj) return errWrongType;

	SC_WII* dev = SC_WII::getDevice(obj);
	if (!dev) return errFailed;

	err = slotIntVal(args+1, &enable1);
	if (err) return err;

	if (dev->m_wiiremote == NULL)
	{
		return errFailed;
	}
	else
	{
#ifdef __APPLE__
// is always enabled
// 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
#endif
#ifdef __linux__
		dev->enable( enable1 );
#endif
	}

	return errNone;
}

// int prWiiPlaySpeaker(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 4;
// 	int err;
// 	int enable1;
// 	int freq, vol;
// 	bool result;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	err = slotIntVal(args+1, &enable1);
// 	if (err) return err;
//
// 	err = slotIntVal(args+2, &freq);
// 	if (err) return err;
//
// 	err = slotIntVal(args+3, &vol);
// 	if (err) return err;
//
// // 	err = slotIntVal(args+4, &sample);
// // 	if (err) return err;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// // #ifdef __APPLE__
// // 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
// // #endif
// #ifdef __linux__
// 		uint8_t sample[20] = {
// 			0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
// 			0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c
// 		};
// 		wiimote_speaker_freq( dev->m_wiiremote, (uint8_t) freq);
// 		wiimote_speaker_volume( dev->m_wiiremote, (uint8_t) vol );
// 	if ( enable1 )
// 		wiimote_speaker_play( dev->m_wiiremote, sample, 20);
//
// // 	post( "WII: speaker %i %i", freq, vol );
// #endif
// 	}
//
// 	return errNone;
// }

// int prWiiEnableSpeaker(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
// 	int enable1;
// 	bool result;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	err = slotIntVal(args+1, &enable1);
// 	if (err) return err;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// // #ifdef __APPLE__
// // 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
// // #endif
// #ifdef __linux__
// 		if ( enable1 )
// 			wiimote_speaker_enable(dev->m_wiiremote);
// 		else
// 			wiimote_speaker_disable(dev->m_wiiremote);
//
// // 	post( "WII: speaker enable %i", enable1 );
//
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiInitSpeaker(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
// 	int enable1;
// 	bool result;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	err = slotIntVal(args+1, &enable1);
// 	if (err) return err;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// // #ifdef __APPLE__
// // 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
// // #endif
// #ifdef __linux__
// 		if ( enable1 )
// 			dev->speaker_init( 4 );
// 		else
// 			dev->speaker_init( 8 );
//
// // 	post( "WII: speaker init %i", enable1 );
//
// #endif
// 	}
//
// 	return errNone;
// }
//
// int prWiiMuteSpeaker(VMGlobals *g, int numArgsPushed)
// {
// 	PyrSlot* args = g->sp - 1;
// 	int err;
// 	int enable1;
// 	bool result;
//
// 	PyrObject* obj = SC_WII::getObject(args+0);
// 	if (!obj) return errWrongType;
//
// 	SC_WII* dev = SC_WII::getDevice(obj);
// 	if (!dev) return errFailed;
//
// 	err = slotIntVal(args+1, &enable1);
// 	if (err) return err;
//
// 	if (dev->m_wiiremote == NULL)
// 	{
// 		return errFailed;
// 	}
// 	else
// 	{
// // #ifdef __APPLE__
// // 		result = wiiremote_motionsensor( dev->m_wiiremote, enable1 );
// // #endif
// #ifdef __linux__
// 		if ( enable1 )
// 			wiimote_speaker_mute(dev->m_wiiremote);
// 		else
// 			wiimote_speaker_unmute(dev->m_wiiremote);
//
// // 	post( "WII: mute %i", enable1 );
// #endif
// 	}
//
// 	return errNone;
// }

void initWiiPrimitives()
{
	int base, index;

	s_wii = getsym("WiiMote");
	s_wiiCalibrationInfoClass = getsym("WiiCalibrationInfo");	// has calibration date for all axes
//	s_wiiLEDStateClass = getsym("WiiLEDState");			// has the four LED states

// 	s_wiiRemoteClass = getsym("WiiRemote");			// Remote
// 	s_wiiNunChuckClass = getsym("WiiNunChuck");		// NunChuck
// 	s_wiiClassicClass = getsym("WiiClassic");		// Classic
// 	s_wiiAction = getsym("prWiiMoteAction");

	s_wiiDisconnected = getsym("prDisconnectAction");
	s_wiiConnected = getsym("prConnectAction");
	s_readError = getsym("prReadError");

/// general event on MacOSX
	s_handleEvent = getsym("prHandleEvent");

/// separate events on Linux:
	s_handleBatteryEvent = getsym("prHandleBatteryEvent");
	s_handleExtensionEvent = getsym("prHandleExtensionEvent");
	s_handleButtonEvent = getsym("prHandleButtonEvent");
	s_handleNunchukEvent = getsym("prHandleNunchukEvent");
	s_handleClassicEvent = getsym("prHandleClassicEvent");
	s_handleIREvent = getsym("prHandleIREvent");
	s_handleAccEvent = getsym("prHandleAccEvent");

	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_Wii_Start", prWii_Start, 2, 0); // starts the eventloop
	definePrimitive(base, index++, "_Wii_Discover", prWii_Discover, 3, 0); // discovers a new device
	definePrimitive(base, index++, "_Wii_Stop", prWii_Stop, 1, 0); // stops the eventloop

	definePrimitive(base, index++, "_Wii_Open", prWii_Open, 1, 0 );
// 	definePrimitive(base, index++, "_Wii_Update", prWii_Update, 1, 0 );
// 	definePrimitive(base, index++, "_Wii_UpdateData", prWii_UpdateData, 9, 0 );
	definePrimitive(base, index++, "_Wii_Close", prWii_Close, 1, 0 );

	definePrimitive(base, index++, "_Wii_Address", prWiiAddress, 1, 0);
// 	definePrimitive(base, index++, "_Wii_SetAddress", prWiiSetAddress, 2, 0);

	definePrimitive(base, index++, "_Wii_Connect", prWiiConnect, 1, 0);
	definePrimitive(base, index++, "_Wii_Disconnect", prWiiDisconnect, 1, 0);

	definePrimitive(base, index++, "_Wii_Calibration", prWiiCalibration, 2, 0);

// 	definePrimitive(base, index++, "_Wii_GetExpansion", prWiiGetExpansion, 1, 0);
// 	definePrimitive(base, index++, "_Wii_GetBattery", prWiiGetBattery, 1, 0);
//
// 	definePrimitive(base, index++, "_Wii_GetButtons", prWiiGetButtons, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetMotion", prWiiGetMotion, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetIR", prWiiGetIR, 2, 0);
//
// 	definePrimitive(base, index++, "_Wii_GetNunchukButtons", prWiiGetNunchukButtons, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetNunchukJoy", prWiiGetNunchukJoy, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetNunchukMotion", prWiiGetNunchukMotion, 2, 0);

// 	definePrimitive(base, index++, "_Wii_GetClassicButtons", prWiiGetClassicButtons, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetClassicJoy", prWiiGetClassicJoy, 2, 0);
// 	definePrimitive(base, index++, "_Wii_GetClassicAnalog", prWiiGetClassicAnalog, 2, 0);

// 	definePrimitive(base, index++, "_Wii_GetLED", prWiiGetLED, 2, 0);

	definePrimitive(base, index++, "_Wii_SetLED", prWiiSetLED, 2, 0);

	definePrimitive(base, index++, "_Wii_SetVibration", prWiiSetVibration, 2, 0);

// 	definePrimitive(base, index++, "_Wii_InitSpeaker", prWiiInitSpeaker, 2, 0);
// 	definePrimitive(base, index++, "_Wii_PlaySpeaker", prWiiPlaySpeaker, 5, 0);
// 	definePrimitive(base, index++, "_Wii_MuteSpeaker", prWiiMuteSpeaker, 2, 0);
// 	definePrimitive(base, index++, "_Wii_EnableSpeaker", prWiiEnableSpeaker, 2, 0);

	definePrimitive(base, index++, "_Wii_Enable", prWiiEnable, 2, 0);
	definePrimitive(base, index++, "_Wii_EnableButtons", prWiiSetButtons, 2, 0);
	definePrimitive(base, index++, "_Wii_EnableIRSensor", prWiiSetIRSensor, 2, 0);
	definePrimitive(base, index++, "_Wii_EnableMotionSensor", prWiiSetMotionSensor, 2, 0);
	definePrimitive(base, index++, "_Wii_EnableExpansion", prWiiSetExpansion, 2, 0);

}
#else // NOT HAVE_WII
void initWiiPrimitives()
{
	//other platforms?
}
#endif
