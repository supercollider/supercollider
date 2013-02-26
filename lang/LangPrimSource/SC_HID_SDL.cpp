/*
	SDL based HID support.
	Copyright (c) 2013 Marije Baalman

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

	Linux Input Device interface, 2004 <sk>
*/

#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrPrimitive.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"

#include <boost/atomic.hpp>

#include "SC_LanguageClient.h"

extern bool compiledOK;

#if HAVE_SDL_HID

#include <boost/thread.hpp>
#include <map>
// #include <SDL2/SDL.h>
#include <SDL/SDL.h>

typedef std::map<int,SDL_Joystick*> joy_map_t;

typedef std::map<int,SDL_Joystick*>::const_iterator joy_map_iterator;

PyrSymbol * s_sdlhid;

struct SC_HID_SDLManager
{
public:
	static SC_HID_SDLManager& instance();

	int start();
	int stop();
	
	int buildDeviceList();
	int open_joystick( int joy_idx );
	int close_joystick( int joy_idx );
	SDL_Joystick * get_joystick( int joy_idx );

	void close_all_joysticks();

	static PyrSymbol* s_joystickAxis;
	static PyrSymbol* s_joystickButton;
	static PyrSymbol* s_joystickHat;
	static PyrSymbol* s_joystickBall;
	static PyrSymbol* s_joystickClosed;
// 	static PyrSymbol* s_joystickInfo;

public:
	SC_HID_SDLManager();
	~SC_HID_SDLManager();
	
	int initialize_SDL();

protected:	
	void threadLoop();
	void handleEvent(SDL_Event * evt, boost::atomic<bool> const & shouldBeRunning);
	void joystickClosed(int, boost::atomic<bool> const & shouldBeRunning);
// 	void joystickInfo();


private:
	joy_map_t joysticks;    // declares a vector of joysticks

	// language interface
	PyrObject*		m_obj;

	boost::thread		m_thread;
	boost::atomic<bool>	m_running;
	boost::atomic<bool>	mShouldBeRunning;
};

PyrSymbol* SC_HID_SDLManager::s_joystickAxis = 0;
PyrSymbol* SC_HID_SDLManager::s_joystickButton = 0;
PyrSymbol* SC_HID_SDLManager::s_joystickHat = 0;
PyrSymbol* SC_HID_SDLManager::s_joystickBall = 0;
PyrSymbol* SC_HID_SDLManager::s_joystickClosed = 0;
// PyrSymbol* SC_HID_SDLManager::s_joystickInfo = 0;

void SC_HID_SDLManager::close_all_joysticks(){
  joy_map_t::const_iterator it;
  for(it=joysticks.begin(); it!=joysticks.end(); ++it){
    SDL_JoystickClose( it->second );
  }
  joysticks.clear();
}

int SC_HID_SDLManager::open_joystick( int joy_idx ){
  SDL_Joystick * newjoy = SDL_JoystickOpen( joy_idx );
  if (!newjoy){
        // FIXME: use sc-standard for printing errors
    fprintf(stderr, "HID SDL : Unable to open joystick %d\n", joy_idx);
    return 0;
  } else {
    joysticks[ joy_idx ] = newjoy;
    // FIXME: send info back to language
//     lo_send_from( t, s, LO_TT_IMMEDIATE, "/joystick/open", "is", joy_idx, SDL_JoystickNameForIndex( joy_idx ) );
    return 1;
  }
}

int SC_HID_SDLManager::close_joystick( int joy_idx ){
  if ( joysticks.count( joy_idx ) >= 0 ){
  /* this is SDL2.0 stuff
    if ( !SDL_JoystickGetAttached( joysticks.find( joy_idx )->second ) ){
        // FIXME: use sc-standard for printing errors
      fprintf(stderr, "HID SDL : joystick was already closed %d\n", joy_idx);
      return 0;
    } else {
    */
    // FIXME: add closed action call?
//    joystickClosed( joy_idx, mShouldBeRunning );
      SDL_JoystickClose( joysticks.find( joy_idx )->second );
      joysticks.erase( joysticks.find( joy_idx ) );
      return 1;
    //}
  } else {
      fprintf(stderr, "HID SDL : joystick was not open %d\n", joy_idx);
      return 0;
  }
}

SDL_Joystick * SC_HID_SDLManager::get_joystick( int joy_idx ){
  SDL_Joystick * myjoy;
  if ( joysticks.count( joy_idx ) >= 0 ){
    myjoy = joysticks.find( joy_idx )->second;
    /* this is SDL2 stuff
    if ( !SDL_JoystickGetAttached( myjoy ) ){
        // FIXME: use sc-standard for printing errors
      fprintf(stderr, "HID SDL : joystick was closed %d\n", joy_idx);
      return NULL;
    } else {
    */
      return myjoy;
    //}
  } else {
      fprintf(stderr, "HID SDL : joystick was not open %d\n", joy_idx);
      return NULL;
  }
}

SC_HID_SDLManager& SC_HID_SDLManager::instance()
{
	static SC_HID_SDLManager instance;
	return instance;
}

SC_HID_SDLManager::SC_HID_SDLManager()
	: m_running(false)
{
// 	if (pipe(m_cmdFifo) == -1) {
// 		m_cmdFifo[0] = m_cmdFifo[1] = -1;
// 	}
// 	devicesChanged();
}

SC_HID_SDLManager::~SC_HID_SDLManager()
{
// 	close(m_cmdFifo[0]);
// 	close(m_cmdFifo[1]);
}

int SC_HID_SDLManager::start()
{
  int result;
  mShouldBeRunning = true;
  if ( !m_running ){
    result = initialize_SDL();
  }
  if ( !m_running ){
    return errFailed;
  }
  return result;
}

int SC_HID_SDLManager::stop()
{
  close_all_joysticks();
  m_running = false;
  mShouldBeRunning = false;
  // clean up thread:
  m_thread.join();
  return errNone;
}


// initialize SDL for joystick access:
int SC_HID_SDLManager::initialize_SDL(){
  m_running = false;
  // FIXME: make a define check based on the library version of the SDL library
  // FIXED: We don't need video, but without it SDL will fail to work in SDL_WaitEvent()
  if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  //if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENT | SDL_INIT_JOYSTICK) < 0)
  {
    // FIXME: use sc-standard for printing errors
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    return errFailed;
  }
  
  // setup eventloop, or start it
  try {
    m_running = true;
    boost::thread thread(boost::bind(&SC_HID_SDLManager::threadLoop, this));
    m_thread = boost::move(thread);
  } catch(std::exception & e) {
    throw e;
  }
  return errNone;
}

int SC_HID_SDLManager::buildDeviceList(){
  int num_joysticks = SDL_NumJoysticks();
  /*
  int joy_idx;
  for(joy_idx = 0; joy_idx < num_joysticks; ++joy_idx){
    if ( joysticks.count( joy_idx )>0 ){
      SDL_Joystick * joy = joysticks.find( joy_idx )->second;
      // joystick is open and we just send the info back
      //joystickInfo( joy_idx, joy, mShouldBeRunning );
    } else {
      // open and close joystick, and send info
      SDL_Joystick * newjoy = SDL_JoystickOpen( joy_idx );
      //joystickInfo( joy_idx, newjoy, mShouldBeRunning );
      SDL_JoystickClose( newjoy );
    }
  }
  */
  return num_joysticks;
}

// void SC_HID_SDLManager::joystickInfo( int joy_idx, SDL_Joystick * joy, boost::atomic<bool> const & shouldBeRunning ){
//   int status = lockLanguageOrQuit(shouldBeRunning);
//   if (status == EINTR)
//     return;
//   if (status) {
//     postfl("error when locking language (%d)\n", status);
//     return;
//   }
//   PyrString *devname = newPyrString(g->gc, SDL_JoystickNameForIndex( joy_idx ), 0, true);
//   int numaxes = SDL_JoystickNumAxes( joy );
//   int numbuttons = SDL_JoystickNumButtons( joy );
//   int numhats = SDL_JoystickNumHats( joy );
//   int numballs = SDL_JoystickNumBalls( joy );
//   if (compiledOK) {
//     VMGlobals* g = gMainVMGlobals;
//     g->canCallOS = false;
//     ++g->sp; SetObject(g->sp, m_obj);
//     ++g->sp; SetInt(g->sp, joy_idx );
//     ++g->sp; SetObject(g->sp, devname );
//     ++g->sp; SetInt(g->sp, numaxes );
//     ++g->sp; SetInt(g->sp, numbutotns );
//     ++g->sp; SetInt(g->sp, numhats );
//     ++g->sp; SetInt(g->sp, numballs );
//     runInterpreter(g, s_joystickInfo, 1);
//     g->canCallOS = false;    
//   }
//   gLangMutex.unlock();
// }

void SC_HID_SDLManager::joystickClosed( int joy_idx, boost::atomic<bool> const & shouldBeRunning ){
  int status = lockLanguageOrQuit(shouldBeRunning);
  if (status == EINTR)
    return;
  if (status) {
    postfl("error when locking language (%d)\n", status);
    return;
  }
  if (compiledOK) {
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = false;
    ++g->sp; SetObject(g->sp, m_obj);
    ++g->sp; SetInt(g->sp, joy_idx );
    runInterpreter(g, s_joystickClosed, 1);
    g->canCallOS = false;    
  }
  gLangMutex.unlock();  
}

void SC_HID_SDLManager::handleEvent( SDL_Event * evt, boost::atomic<bool> const & shouldBeRunning ){
  int status = lockLanguageOrQuit(shouldBeRunning);
  if (status == EINTR)
    return;
  if (status) {
    postfl("error when locking language (%d)\n", status);
    return;
  }
  switch ( evt->type ){
    case SDL_JOYAXISMOTION:
      if (compiledOK) {
	VMGlobals* g = gMainVMGlobals;
	g->canCallOS = false;
	++g->sp; SetObject(g->sp, m_obj);
	++g->sp; SetInt(g->sp, evt->jaxis.which);
	++g->sp; SetInt(g->sp, evt->jaxis.axis);
	++g->sp; SetInt(g->sp, evt->jaxis.value);
	runInterpreter(g, s_joystickAxis, 4);
	g->canCallOS = false;    
      }
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      if (compiledOK) {
	VMGlobals* g = gMainVMGlobals;
	g->canCallOS = false;
	++g->sp; SetObject(g->sp, m_obj);
	++g->sp; SetInt(g->sp, evt->jbutton.which);
	++g->sp; SetInt(g->sp, evt->jbutton.button);
	++g->sp; SetInt(g->sp, evt->jbutton.state);
	runInterpreter(g, s_joystickButton, 4);
	g->canCallOS = false;    
      }
      break;
    case SDL_JOYHATMOTION:
      if (compiledOK) {
	VMGlobals* g = gMainVMGlobals;
	g->canCallOS = false;
	++g->sp; SetObject(g->sp, m_obj);
	++g->sp; SetInt(g->sp, evt->jhat.which);
	++g->sp; SetInt(g->sp, evt->jhat.hat);
	++g->sp; SetInt(g->sp, evt->jhat.value);
	runInterpreter(g, s_joystickButton, 4);
	g->canCallOS = false;    
      }
      break;
    case SDL_JOYBALLMOTION:
      if (compiledOK) {
	VMGlobals* g = gMainVMGlobals;
	g->canCallOS = false;
	++g->sp; SetObject(g->sp, m_obj);
	++g->sp; SetInt(g->sp, evt->jball.which);
	++g->sp; SetInt(g->sp, evt->jball.ball);
	++g->sp; SetInt(g->sp, evt->jball.xrel);
	++g->sp; SetInt(g->sp, evt->jball.yrel);
	runInterpreter(g, s_joystickBall, 5);
	g->canCallOS = false;    
      }
      break;
    }
    gLangMutex.unlock();
}

// this updates the joystick states each step of the loop
void SC_HID_SDLManager::threadLoop(void){
  SDL_Event event;
  while(m_running ){
    while( SDL_PollEvent(&event) ){
      switch(event.type){
	case SDL_JOYAXISMOTION:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
	case SDL_JOYHATMOTION:
	case SDL_JOYBALLMOTION:
	  handleEvent( &event, mShouldBeRunning );
	  break;
	default:
	  fprintf(stderr, "HID SDL Error: Unhandled event type: %d\n", event.type);
	  break;
      }
    /*  SDL2 stuff
      // check that joysticks are still attached:
      joy_map_iterator itr;
      for(itr = joysticks.begin(); itr != joysticks.end(); ++itr)
	if ( !SDL_JoystickGetAttached( itr->second ) ){
	  joystickClosed( itr->first, mShouldBeRunning );
	}
	*/
      SDL_Delay(1); // 1ms delay, is that useful??
    }
  }
}



// ----------  primitive calls: ---------------

int prHID_SDL_Start(VMGlobals* g, int numArgsPushed)
{
    // initialize HID_SDLManager, and start event loop
  return SC_HID_SDLManager::instance().start();
}

int prHID_SDL_Stop(VMGlobals* g, int numArgsPushed)
{
    // stop event loop, close all joysticks, and cleanup HID_SDLManager
  return SC_HID_SDLManager::instance().stop();
}

int prHID_SDL_BuildDeviceList(VMGlobals* g, int numArgsPushed){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot *self = args + 0;
  // no arguments
  
  int err;
  
  // iterate over available devices and return info to language to populate the list there
  int result = SC_HID_SDLManager::instance().buildDeviceList();
  if ( result > 0 ){
	// populate array with found devices and their names:
	PyrObject* allDevsArray = newPyrArray(g->gc, result * sizeof(PyrObject), 0 , true);
//	for ( int joy_idx=0; joy_idx < result; joy_idx++ ){
//	  PyrString *devname = newPyrString(g->gc, SDL_JoystickNameForIndex( joy_idx ), 0, true);
//	  SetObject(allDevsArray->slots+allDevsArray->size++, devname);
//	  g->gc->GCWrite(allDevsArray, (PyrObject*) devname);
//	}
	SetObject( self, allDevsArray );
  } else {
	// send back info that no devices were found, or empty array
	SetInt( self, 0 );
  }
  return errNone;
}

int prHID_SDL_Open( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
  
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  // open device
  int result = SC_HID_SDLManager::instance().open_joystick( joyid );
  
  SetInt( self, result );
  
  return errNone;
}

int prHID_SDL_Close( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
  
  
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  // open device
  int result = SC_HID_SDLManager::instance().close_joystick( joyid );
  
  SetInt( self, result );
  
  return errNone;  
}

int prHID_SDL_GetInfo( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
    
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  SDL_Joystick * joy = SC_HID_SDLManager::instance().get_joystick( joyid );
  
  if ( joy != NULL ){
    // populate array with found devices and their names:
    PyrObject* devInfo = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0 , true);

//    PyrString *devname = newPyrString(g->gc, SDL_JoystickNameForIndex( joyid ), 0, true);
    PyrString *devname = newPyrString(g->gc, SDL_JoystickName( joyid ), 0, true);
    SetObject(devInfo->slots+devInfo->size++, devname);
    g->gc->GCWrite(devInfo, (PyrObject*) devname);

    int numaxes = SDL_JoystickNumAxes( joy );
    int numbuttons = SDL_JoystickNumButtons( joy );
    int numhats = SDL_JoystickNumHats( joy );
    int numballs = SDL_JoystickNumBalls( joy );

    SetInt(devInfo->slots+devInfo->size++, numaxes);
    SetInt(devInfo->slots+devInfo->size++, numbuttons);
    SetInt(devInfo->slots+devInfo->size++, numhats);
    SetInt(devInfo->slots+devInfo->size++, numballs);
    
    char guid[64]; //TODO: is this large enough for all possible GUIDs?
//    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, sizeof (guid));
    PyrString *devguid = newPyrString(g->gc, guid, 0, true);
    SetObject(devInfo->slots+devInfo->size++, devguid);
    g->gc->GCWrite(devInfo, (PyrObject*) devguid);

    SetObject( self, devInfo );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

// TODO: call for detailed info about capabilities on device, to make a map (this info seems available in the SDL library)
int prHID_SDL_GetDeviceMap( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
  
  
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  SDL_Joystick * joy = SC_HID_SDLManager::instance().get_joystick( joyid );
  
  if ( joy != NULL ){
    // populate array with found devices and their names:
    PyrObject* devInfo = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0 , true);

//     PyrString *devname = newPyrString(g->gc, SDL_JoystickNameForIndex( joyid ), 0, true);
//     SetObject(devInfo->slots+devInfo->size++, devname);
//     g->gc->GCWrite(devInfo, (PyrObject*) devname);
// 
//     int numaxes = SDL_JoystickNumAxes( joy );
//     int numbuttons = SDL_JoystickNumButtons( joy );
//     int numhats = SDL_JoystickNumHats( joy );
//     int numballs = SDL_JoystickNumBalls( joy );
// 
//     SetInt(devInfo->slots+devInfo->size++, numaxes);
//     SetInt(devInfo->slots+devInfo->size++, numbuttons);
//     SetInt(devInfo->slots+devInfo->size++, numhats);
//     SetInt(devInfo->slots+devInfo->size++, numballs);
//     
//     char guid[64]; //TODO: is this large enough for all possible GUIDs?
//     SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, sizeof (guid));
//     PyrString *devguid = newPyrString(g->gc, SDL_JoystickNameForIndex( joyid ), 0, true);
//     SetObject(devInfo->slots+devInfo->size++, devguid);
//     g->gc->GCWrite(devInfo, (PyrObject*) devguid);

    SetObject( self, devInfo );
  } else {
    SetInt( self, 0 );
  }
  
  return errNone;  
}


void close_HID_SDL_Devices(){
  SC_HID_SDLManager::instance().stop();
}

void initHIDSDLPrimitives()
{
  int base, index;
 
  close_HID_SDL_Devices();

  s_sdlhid = getsym("HID_SDL");

  base = nextPrimitiveIndex();
  index = 0;

  definePrimitive(base, index++, "_HID_SDL_Start", prHID_SDL_Start, 1, 0); // this starts the eventloop
  definePrimitive(base, index++, "_HID_SDL_Stop", prHID_SDL_Stop, 1, 0);   // this also cleans up and closes devices

  definePrimitive(base, index++, "_HID_SDL_BuildDeviceList", prHID_SDL_BuildDeviceList, 1, 0); // this gets name info about the various devices that are attached

  definePrimitive(base, index++, "_HID_SDL_Open", prHID_SDL_Open, 2, 0); // opens a specific device
  definePrimitive(base, index++, "_HID_SDL_Close", prHID_SDL_Close, 2, 0); // closes a specific device
  
  definePrimitive(base, index++, "_HID_SDL_GetInfo", prHID_SDL_GetInfo, 2, 0); // gets info about a specific device
  definePrimitive(base, index++, "_HID_SDL_GetDeviceMap", prHID_SDL_GetDeviceMap, 2, 0); // this gets info about the specific device
  
  SC_HID_SDLManager::s_joystickAxis = getsym("prJoystickAxis");     // send back joystick axis data
  SC_HID_SDLManager::s_joystickButton = getsym("prJoystickButton"); // send back joystick button data
  SC_HID_SDLManager::s_joystickHat = getsym("prJoystickHat");       // send back joystick hat data
  SC_HID_SDLManager::s_joystickBall = getsym("prJoystickBall");     // send back joystick ball data
  
  SC_HID_SDLManager::s_joystickClosed = getsym("prJoystickClosed"); // send back that device was closed
//   SC_HID_SDLManager::s_joystickInfo = getsym("prJoystickInfo");   
}

void initHIDPrimitives()
{
	//other platforms?
}

#else // no SDL HID

void initHIDSDLPrimitives()
{
	//other platforms?
}

#endif /// HAVE_SDL_HID


// EOF
