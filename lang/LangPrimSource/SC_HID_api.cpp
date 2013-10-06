/*
	HIDAPI based HID support.
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

#if HAVE_HIDAPI

// needed for string conversions
#include <wchar.h>
#include <cstdlib>

#include <hidapi.h>
#include <hidapi_parser.h>

#include <boost/thread.hpp>
#include <map>

typedef std::map<int, hid_dev_desc* > hid_map_t;

struct SC_HID_APIManager
{
public:
	static SC_HID_APIManager& instance();

	int start();
	int stop();
	
	int build_devicelist();
	int free_devicelist();
	
	int open_device(  int vendor, int product ); // const char* serial_number=NULL
	int close_device( int joy_idx );
	void close_all_devices();

	struct hid_dev_desc * get_device( int joy_idx );

	void setPyrObject( PyrObject * obj );

	static PyrSymbol* s_hidapi;
	static PyrSymbol* s_hidElementData;
	static PyrSymbol* s_hidDeviceData;
// 	static PyrSymbol* s_hidClosed;

	SC_HID_APIManager();
	~SC_HID_APIManager();
	
	int initialize_hidapi();
	
	void elementData( int, struct hid_device_element * );
	void deviceData( int, struct hid_dev_desc * );

	struct hid_device_info *devinfos;

protected:	
	void threadLoop();
	void handleDevice( int, struct hid_dev_desc *, boost::atomic<bool> const & shouldBeRunning);
	void handleElement( int, struct hid_device_element *, boost::atomic<bool> const & shouldBeRunning);
// 	void deviceClosed(int, boost::atomic<bool> const & shouldBeRunning);

private:
	hid_map_t hiddevices;    // declares a vector of hiddevices
	int number_of_hids;

	// language interface
	PyrObject*		m_obj;

	boost::thread		m_thread;
	boost::atomic<bool>	m_running;
	boost::atomic<bool>	mShouldBeRunning;
};

PyrSymbol* SC_HID_APIManager::s_hidapi = 0;
PyrSymbol* SC_HID_APIManager::s_hidDeviceData = 0;
PyrSymbol* SC_HID_APIManager::s_hidElementData = 0;
// PyrSymbol* SC_HID_APIManager::s_hidClosed = 0;

static void hid_element_cb( struct hid_device_element *el, void *data);
static void hid_descriptor_cb( struct hid_dev_desc *dd, void *data);

void hid_element_cb( struct hid_device_element *el, void *data)
{
  SC_HID_APIManager::instance().elementData( *((int*) data), el );
}

void hid_descriptor_cb( struct hid_dev_desc *dd, void *data)
{
  SC_HID_APIManager::instance().deviceData( *((int*) data), dd );
}

void SC_HID_APIManager::deviceData( int id, struct hid_dev_desc * dd ){
  handleDevice( id, dd, mShouldBeRunning );
}

void SC_HID_APIManager::elementData( int id, struct hid_device_element * ele ){
  handleElement( id, ele, mShouldBeRunning );
}

void SC_HID_APIManager::setPyrObject( PyrObject * obj ){
    m_obj = obj;
}

void SC_HID_APIManager::close_all_devices(){
  hid_map_t::const_iterator it;
  for(it=hiddevices.begin(); it!=hiddevices.end(); ++it){
    struct hid_dev_desc * devdesc = it->second;
    hid_close_device( devdesc );
  }
  hiddevices.clear();
}

int SC_HID_APIManager::open_device( int vendor, int product ){ // , const char* serial_number
  struct hid_dev_desc * newdevdesc = hid_open_device( vendor, product, NULL );
  
  if (!newdevdesc){
    fprintf(stderr, "HIDAPI : Unable to open device %d, %d\n", vendor, product );
    return 0;
  } else {   
    hiddevices.insert( std::pair<int,hid_dev_desc*>(number_of_hids, newdevdesc) );
//     hiddevices[ number_of_hids ] = newdevdesc;
    newdevdesc->index = number_of_hids;
    
    // TODO something useful for the callback functions - test if this works
//     hid_set_descriptor_callback( newdevdesc->descriptor, (hid_descriptor_callback) SC_HID_APIManager::hid_descriptor_cb, &newdevdesc->index );
//     hid_set_element_callback( newdevdesc->descriptor, (hid_element_callback) SC_HID_APIManager::hid_element_cb, &newdevdesc->index );
    hid_set_descriptor_callback( newdevdesc, (hid_descriptor_callback) hid_descriptor_cb, &newdevdesc->index );
    hid_set_element_callback( newdevdesc, (hid_element_callback) hid_element_cb, &newdevdesc->index );

    number_of_hids++;

    return newdevdesc->index;
  }
}

int SC_HID_APIManager::close_device( int joy_idx ){  
  struct hid_dev_desc * hidtoclose = hiddevices.find( joy_idx )->second;
  if ( hidtoclose == NULL ){
    fprintf(stderr, "HIDAPI : could not find device to close %d\n", joy_idx);
    return 1; // not a fatal error
  } else {
//     deviceClosed( joy_idx, mShouldBeRunning );
    hid_close_device( hidtoclose );
    hiddevices.erase( joy_idx );
  }
  return 1;
}

struct hid_dev_desc * SC_HID_APIManager::get_device( int joy_idx ){  
  if ( hiddevices.count( joy_idx ) >= 0 ){
    struct hid_dev_desc * hidget = hiddevices.find( joy_idx )->second;
    return hidget;
  } else {
    fprintf(stderr, "HIDAPI : device was not open %d\n", joy_idx);
    return NULL;
  }
}

SC_HID_APIManager& SC_HID_APIManager::instance()
{
	static SC_HID_APIManager instance;
	return instance;
}

SC_HID_APIManager::SC_HID_APIManager()
	: m_running(false)
{
  number_of_hids = 0;
}

SC_HID_APIManager::~SC_HID_APIManager()
{
  close_all_devices();
}

int SC_HID_APIManager::start()
{
  int result;
  number_of_hids = 0;
  mShouldBeRunning = true;
  if ( !m_running ){
    result = initialize_hidapi();
  }
  if ( !m_running ){
    return errFailed;
  }
  return result;
}

int SC_HID_APIManager::stop()
{
  close_all_devices();
  m_running = false;
  mShouldBeRunning = false;
  // clean up thread:
  m_thread.join();
  return errNone;
}

int SC_HID_APIManager::initialize_hidapi(){
  m_running = false;
  
  if (hid_init()){
    fprintf(stderr, "Unable to initialize hidapi\n");
    return errFailed;
  }  
  // setup eventloop, or start it
  try {
    m_running = true;
    boost::thread thread(boost::bind(&SC_HID_APIManager::threadLoop, this));
    m_thread = boost::move(thread);
  } catch(std::exception & e) {
    throw e;
  }
  return errNone;
}

int SC_HID_APIManager::build_devicelist(){  
  struct hid_device_info *cur_dev;
  devinfos = hid_enumerate(0x0, 0x0);

  cur_dev = devinfos;
  int count = 0;
  while (cur_dev) {
    count++;
    cur_dev = cur_dev->next;
  }
//   number_of_hids = count;
//   cur_dev = devs;	
//   while (cur_dev) {
// 	hidInfo( joy_idx, cur_dev, mShouldBeRunning );
// 	cur_dev = cur_dev->next;
//   }
  return count;
}
  
int SC_HID_APIManager::free_devicelist(){  
  hid_free_enumeration(devinfos);
  devinfos = NULL;
  return errNone;
}

// void SC_HID_APIManager::hidInfo( int joy_idx, struct hid_device_info * cur_dev, boost::atomic<bool> const & shouldBeRunning ){
//   int status = lockLanguageOrQuit(shouldBeRunning);
//   if (status == EINTR)
//     return;
//   if (status) {
//     postfl("error when locking language (%d)\n", status);
//     return;
//   }
// 
// // this may need a string conversion
//   PyrString *dev_man_name = newPyrString(g->gc, cur_dev->manufacturer_string );
//   PyrString *dev_prod_name = newPyrString(g->gc, cur_dev->product_string );
//   PyrString *dev_path = newPyrString(g->gc, cur_dev->path );
//   PyrString *dev_serial = newPyrString(g->gc, cur_dev->serial_number );
//   if (compiledOK) {
//     VMGlobals* g = gMainVMGlobals;
//     g->canCallOS = false;
//     ++g->sp; SetObject(g->sp, m_obj);
//     ++g->sp; SetInt(g->sp, joy_idx );
//     ++g->sp; SetInt(g->sp, cur_dev->vendor_id );
//     ++g->sp; SetInt(g->sp, cur_dev->product_id );
//     ++g->sp; SetObject(g->sp, dev_path );
//     ++g->sp; SetObject(g->sp, dev_serial );
//     ++g->sp; SetObject(g->sp, dev_man_name );
//     ++g->sp; SetObject(g->sp, dev_prod_name );
//     ++g->sp; SetInt(g->sp, cur_dev->release_number );
//     ++g->sp; SetInt(g->sp, cur_dev->interface_number );
//     runInterpreter(g, s_hidInfo, 1);
//     g->canCallOS = false;    
//   }
//   gLangMutex.unlock();
// }

/*
void SC_HID_APIManager::deviceClosed( int joy_idx, boost::atomic<bool> const & shouldBeRunning ){
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
//     ++g->sp; SetObject(g->sp, m_obj);
    ++g->sp; SetObject(g->sp, s_hidapi->u.classobj ); // set the class HID_API
    ++g->sp; SetInt(g->sp, joy_idx );
    runInterpreter(g, s_hidClosed, 2);
    g->canCallOS = false;    
  }
  gLangMutex.unlock();  
}
*/

void SC_HID_APIManager::handleElement( int joy_idx, struct hid_device_element * ele, boost::atomic<bool> const & shouldBeRunning ){
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
//     ++g->sp; SetObject(g->sp, m_obj);
    ++g->sp; SetObject(g->sp, s_hidapi->u.classobj ); // set the class HID_API
    ++g->sp; SetInt(g->sp, joy_idx );
    ++g->sp; SetInt(g->sp, ele->index );
    ++g->sp; SetInt(g->sp, ele->usage_page );
    ++g->sp; SetInt(g->sp, ele->usage );
    ++g->sp; SetInt(g->sp, ele->value );
    ++g->sp; SetFloat(g->sp, hid_element_map_logical( ele ) );
    runInterpreter(g, s_hidElementData, 7 );
    g->canCallOS = false;    
  }
  gLangMutex.unlock();
}

void SC_HID_APIManager::handleDevice( int joy_idx, struct hid_dev_desc * devd, boost::atomic<bool> const & shouldBeRunning ){
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
//     ++g->sp; SetObject(g->sp, m_obj);
    ++g->sp; SetObject(g->sp, s_hidapi->u.classobj ); // set the class HID_API
    ++g->sp; SetInt(g->sp, joy_idx );
    ++g->sp; SetInt(g->sp, devd->device_collection->num_elements );
    runInterpreter(g, s_hidDeviceData, 3);
    g->canCallOS = false;    
  }
  gLangMutex.unlock();
}

void SC_HID_APIManager::threadLoop(){
  int res = 0;
  hid_map_t::const_iterator it;
  unsigned char buf[256];

  while(m_running ){
    for(it=hiddevices.begin(); it!=hiddevices.end(); ++it){
      if ( it->second != NULL ){
	res = hid_read( it->second->device, buf, sizeof(buf));
	if ( res > 0 ) {
	  hid_parse_input_report( buf, res, it->second );
	}
      }
      #ifdef WIN32
	Sleep(5);
      #else
	usleep(5000);
      #endif
    }
  }
}

// ----------  primitive calls: ---------------

int prHID_API_Start(VMGlobals* g, int numArgsPushed)
{
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot *self = args + 0;
  
  SC_HID_APIManager::instance().setPyrObject( slotRawObject(self) );
    // initialize HID_SDLManager, and start event loop
  return SC_HID_APIManager::instance().start();
}

int prHID_API_Stop(VMGlobals* g, int numArgsPushed)
{
    // stop event loop, close all devices, and cleanup manager
  return SC_HID_APIManager::instance().stop();
}

// helper function to convert from unicode to ansi
char * wchar_to_char( wchar_t * wchs ){
    int len = wcslen( wchs ) + 1;
    char * chs = (char*) malloc( sizeof(char) * len );
    std::wcstombs( chs, wchs, len );
    return chs;
}

int prHID_API_BuildDeviceList(VMGlobals* g, int numArgsPushed){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot *self = args + 0;
  // no arguments
  
  int err;
  
  // iterate over available devices and return info to language to populate the list there
  int result = SC_HID_APIManager::instance().build_devicelist();
  if ( result > 0 ){
    // TODO:
	// populate array with found devices and their names:
	PyrObject* allDevsArray = newPyrArray(g->gc, result * sizeof(PyrObject), 0 , true);

	struct hid_device_info *cur_dev = SC_HID_APIManager::instance().devinfos;
	while( cur_dev ){	  
	  PyrObject* devInfo = newPyrArray(g->gc, 9 * sizeof(PyrObject), 0 , true);
	  
	  SetInt(devInfo->slots+devInfo->size++, cur_dev->vendor_id);
	  SetInt(devInfo->slots+devInfo->size++, cur_dev->product_id);

	  PyrString *dev_path_name = newPyrString(g->gc, cur_dev->path, 0, true ); 
	  SetObject(devInfo->slots+devInfo->size++, dev_path_name);
	  g->gc->GCWrite(devInfo, (PyrObject*) dev_path_name);
	  
	  char * mystring = wchar_to_char( cur_dev->serial_number );

  	  PyrString *dev_serial = newPyrString(g->gc, mystring, 0, true ); 
	  SetObject(devInfo->slots+devInfo->size++, dev_serial);
	  g->gc->GCWrite(devInfo, (PyrObject*) dev_serial);

	  if ( cur_dev->manufacturer_string != NULL ){
	    mystring = wchar_to_char( cur_dev->manufacturer_string );
	  } else {
	    mystring = "";
	  }
	  PyrString *dev_man_name = newPyrString(g->gc, mystring, 0, true ); 
	  SetObject(devInfo->slots+devInfo->size++, dev_man_name);
	  g->gc->GCWrite(devInfo, (PyrObject*) dev_man_name);
	  
	  if ( cur_dev->product_string != NULL ){
	    mystring = wchar_to_char( cur_dev->product_string );
	  } else {
	    mystring = "";
	  }  
	  PyrString *dev_prod_name = newPyrString(g->gc, mystring, 0, true );
	  SetObject(devInfo->slots+devInfo->size++, dev_prod_name);
	  g->gc->GCWrite(devInfo, (PyrObject*) dev_prod_name);
	  
	  SetInt(devInfo->slots+devInfo->size++, cur_dev->release_number);
	  SetInt(devInfo->slots+devInfo->size++, cur_dev->interface_number);

	  SetObject(allDevsArray->slots+allDevsArray->size++, devInfo );
	  g->gc->GCWrite(allDevsArray, (PyrObject*) devInfo );
	  
	  cur_dev = cur_dev->next;
	}
	
    SetObject( self, allDevsArray );
    SC_HID_APIManager::instance().free_devicelist();
  } else {
	// send back info that no devices were found, or empty array
	SetInt( self, 0 );
  }
  return errNone;
}

int prHID_API_Open( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg1  = args + 1;
  PyrSlot* arg2  = args + 2;
    
  int err;
  int vendorid;
  int productid;
  
  err = slotIntVal( arg1, &vendorid );
  if ( err != errNone ) return err;

  err = slotIntVal( arg2, &productid );
  if ( err != errNone ) return err;
  
  // TODO add optional string for serial number
  
  // open device
  int result = SC_HID_APIManager::instance().open_device( vendorid, productid );
  
  SetInt( self, result );
  
  return errNone;
}

int prHID_API_Close( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
  
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  int result = SC_HID_APIManager::instance().close_device( joyid );
  
  SetInt( self, result );
  
  return errNone;  
}

int prHID_API_GetInfo( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
    
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  struct hid_dev_desc * devdesc = SC_HID_APIManager::instance().get_device( joyid );
  struct hid_device_info * cur_dev = devdesc->info;
  
  if ( cur_dev != NULL ){
    PyrObject* devInfo = newPyrArray(g->gc, 9 * sizeof(PyrObject), 0 , true);
    
    SetInt(devInfo->slots+devInfo->size++, cur_dev->vendor_id);
    SetInt(devInfo->slots+devInfo->size++, cur_dev->product_id);

    PyrString *dev_path_name = newPyrString(g->gc, cur_dev->path, 0, true ); 
    SetObject(devInfo->slots+devInfo->size++, dev_path_name);
    g->gc->GCWrite(devInfo, (PyrObject*) dev_path_name);

    char * mystring;
    mystring = wchar_to_char( cur_dev->serial_number );
    PyrString *dev_serial = newPyrString(g->gc, mystring, 0, true ); 
    SetObject(devInfo->slots+devInfo->size++, dev_serial);
    g->gc->GCWrite(devInfo, (PyrObject*) dev_serial);

    mystring = wchar_to_char( cur_dev->manufacturer_string );
    PyrString *dev_man_name = newPyrString(g->gc, mystring, 0, true ); 
    SetObject(devInfo->slots+devInfo->size++, dev_man_name);
    g->gc->GCWrite(devInfo, (PyrObject*) dev_man_name);
    
    mystring = wchar_to_char( cur_dev->product_string );
    PyrString *dev_prod_name = newPyrString(g->gc, mystring, 0, true );
    SetObject(devInfo->slots+devInfo->size++, dev_prod_name);
    g->gc->GCWrite(devInfo, (PyrObject*) dev_prod_name);
    
    SetInt(devInfo->slots+devInfo->size++, cur_dev->release_number);
    SetInt(devInfo->slots+devInfo->size++, cur_dev->interface_number);
	  
    SetObject( self, devInfo );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

int prHID_API_GetNumberOfCollections( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
    
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  struct hid_dev_desc * devdesc = SC_HID_APIManager::instance().get_device( joyid );
  struct hid_device_collection * cur_dev = devdesc->device_collection;
  
  if ( cur_dev != NULL ){
    SetInt( self, cur_dev->num_collections );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

int prHID_API_GetCollectionInfo( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg1  = args + 1;
  PyrSlot* arg2  = args + 2;
    
  int err;
  int joyid;
  int collectionid;
    
  err = slotIntVal( arg1, &joyid );
  if ( err != errNone ) return err;
  
  err = slotIntVal( arg2, &collectionid );
  if ( err != errNone ) return err;
  
  struct hid_dev_desc * devdesc = SC_HID_APIManager::instance().get_device( joyid );
  struct hid_device_collection * curdev = devdesc->device_collection;
  struct hid_device_collection * curcollection = curdev->first_collection;
  struct hid_device_collection * thiscollection = NULL;
  
  bool found = curcollection->index == collectionid;
  if ( found ){
    thiscollection = curcollection;
  }
  while( curcollection != NULL && !found ){
      found = curcollection->index == collectionid;
      if ( found ){
	thiscollection = curcollection;
      }
      curcollection = curcollection->next_collection;
  }
  
  if ( thiscollection != NULL ){
    PyrObject* elInfo = newPyrArray(g->gc, 9 * sizeof(PyrObject), 0 , true);
    
    SetInt(elInfo->slots+elInfo->size++, thiscollection->index );
    SetInt(elInfo->slots+elInfo->size++, thiscollection->type );
    SetInt(elInfo->slots+elInfo->size++, thiscollection->usage_page );
    SetInt(elInfo->slots+elInfo->size++, thiscollection->usage_index );

    if ( thiscollection->parent_collection != NULL ){
      SetInt(elInfo->slots+elInfo->size++, thiscollection->parent_collection->index );
    } else {
      SetInt(elInfo->slots+elInfo->size++, -2 );      
    }

    SetInt(elInfo->slots+elInfo->size++, thiscollection->num_collections );

    if ( thiscollection->first_collection != NULL ){
      SetInt(elInfo->slots+elInfo->size++, thiscollection->first_collection->index );
    } else {
      SetInt(elInfo->slots+elInfo->size++, -1 );      
    }

    SetInt(elInfo->slots+elInfo->size++, thiscollection->num_elements );

    if ( thiscollection->first_element != NULL ){
      SetInt(elInfo->slots+elInfo->size++, thiscollection->first_element->index );
    } else {
      SetInt(elInfo->slots+elInfo->size++, -1 );      
    }
    
    SetObject( self, elInfo );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

int prHID_API_GetNumberOfElements( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg  = args + 1;
    
  int err;
  int joyid;
    
  err = slotIntVal( arg, &joyid );
  if ( err != errNone ) return err;
  
  struct hid_dev_desc * devdesc = SC_HID_APIManager::instance().get_device( joyid );
  struct hid_device_collection * cur_dev = devdesc->device_collection;
  
  if ( cur_dev != NULL ){
    SetInt( self, cur_dev->num_elements );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

int prHID_API_GetElementInfo( VMGlobals* g, int numArgsPushed ){
  PyrSlot *args = g->sp - numArgsPushed + 1;
  PyrSlot* self = args + 0;
  PyrSlot* arg1  = args + 1;
  PyrSlot* arg2  = args + 2;
    
  int err;
  int joyid;
  int elementid;
    
  err = slotIntVal( arg1, &joyid );
  if ( err != errNone ) return err;
  
  err = slotIntVal( arg2, &elementid );
  if ( err != errNone ) return err;
  
  struct hid_dev_desc * devdesc = SC_HID_APIManager::instance().get_device( joyid );
  struct hid_device_collection * curdev = devdesc->device_collection;
  struct hid_device_element * curelement = curdev->first_element;
  struct hid_device_element * thiselement = NULL;
  
  bool found = curelement->index == elementid;
  if ( found ){
    thiselement = curelement;
  }
  while( curelement != NULL && !found ){
      found = curelement->index == elementid;
      if ( found ){
	  thiselement = curelement;
      }
      curelement = curelement->next;
  }
  
  if ( thiselement != NULL ){
    PyrObject* elInfo = newPyrArray(g->gc, 16 * sizeof(PyrObject), 0 , true);
    
    SetInt(elInfo->slots+elInfo->size++, thiselement->index );
    SetInt(elInfo->slots+elInfo->size++, thiselement->io_type );
    SetInt(elInfo->slots+elInfo->size++, thiselement->type );
    SetInt(elInfo->slots+elInfo->size++, thiselement->usage_page );
    SetInt(elInfo->slots+elInfo->size++, thiselement->usage );
    
    SetInt(elInfo->slots+elInfo->size++, thiselement->logical_min );
    SetInt(elInfo->slots+elInfo->size++, thiselement->logical_max );
    SetInt(elInfo->slots+elInfo->size++, thiselement->phys_min );
    SetInt(elInfo->slots+elInfo->size++, thiselement->phys_max );
    SetInt(elInfo->slots+elInfo->size++, thiselement->unit_exponent );
    
    SetInt(elInfo->slots+elInfo->size++, thiselement->unit );
    SetInt(elInfo->slots+elInfo->size++, thiselement->report_size );
    SetInt(elInfo->slots+elInfo->size++, thiselement->report_id );
    SetInt(elInfo->slots+elInfo->size++, thiselement->report_index );
    SetInt(elInfo->slots+elInfo->size++, thiselement->value );

    SetInt(elInfo->slots+elInfo->size++, thiselement->parent_collection->index );

    SetObject( self, elInfo );
  } else {
    SetInt( self, 0 );
  }
  return errNone;  
}

void close_HID_API_Devices(){
  SC_HID_APIManager::instance().stop();
}

void initHIDAPIPrimitives()
{
  int base, index;
 
  close_HID_API_Devices();

  SC_HID_APIManager::s_hidapi = getsym("HID_API");

  base = nextPrimitiveIndex();
  index = 0;

  definePrimitive(base, index++, "_HID_API_Start", prHID_API_Start, 1, 0); // this starts the eventloop
  definePrimitive(base, index++, "_HID_API_Stop", prHID_API_Stop, 1, 0);   // this also cleans up and closes devices

  definePrimitive(base, index++, "_HID_API_BuildDeviceList", prHID_API_BuildDeviceList, 1, 0); // this gets device info about the various devices that are attached

  definePrimitive(base, index++, "_HID_API_OpenDevice", prHID_API_Open, 3, 0); // opens a specific device
  definePrimitive(base, index++, "_HID_API_CloseDevice", prHID_API_Close, 2, 0); // closes a specific device
  
  definePrimitive(base, index++, "_HID_API_GetInfo", prHID_API_GetInfo, 2, 0); // gets info about a specific device
  definePrimitive(base, index++, "_HID_API_GetNumberOfCollections", prHID_API_GetNumberOfCollections, 2, 0); // gets number of elements of a device
  definePrimitive(base, index++, "_HID_API_GetCollectionInfo", prHID_API_GetCollectionInfo, 3, 0); // gets info about a specific device element
  definePrimitive(base, index++, "_HID_API_GetNumberOfElements", prHID_API_GetNumberOfElements, 2, 0); // gets number of elements of a device
  definePrimitive(base, index++, "_HID_API_GetElementInfo", prHID_API_GetElementInfo, 3, 0); // gets info about a specific device element
  
  SC_HID_APIManager::s_hidElementData = getsym("prHIDElementData"); // send back element data
  SC_HID_APIManager::s_hidDeviceData = getsym("prHIDDeviceData"); // send back device data
//   SC_HID_APIManager::s_hidClosed = getsym("prHIDDeviceClosed"); // send back that device was closed
}

#else // no SDL HID

void initHIDAPIPrimitives()
{
	//other platforms?
}

#endif /// HAVE_API_HID


// EOF


