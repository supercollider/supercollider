/* hidapi_parser $ 
 *
 * Copyright (C) 2013, Marije Baalman <nescivi _at_ gmail.com>
 * This work was funded by a crowd-funding initiative for SuperCollider's [1] HID implementation
 * including a substantial donation from BEK, Bergen Center for Electronic Arts, Norway
 * 
 * [1] http://supercollider.sourceforge.net
 * [2] http://www.bek.no
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
// #include <curses.h>
#include <errno.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include <lo/lo.h>


#include <hidapi.h>
#include <hidapi_parser.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif


#include <map>

typedef std::map<int, hid_dev_desc* > hid_map_t;

hid_map_t hiddevices;    // declares a vector of integers
int number_of_hids = 0;

int done = 0;

#define MAX_STR 255


lo_address t;
lo_server s;
lo_server_thread st;

static void osc_element_cb( struct hid_device_element *el, void *data)
{
  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, *((int*) data) );
  lo_message_add_int32( m1, el->index );
  lo_message_add_int32( m1, el->usage_page );
  lo_message_add_int32( m1, el->usage );
  lo_message_add_int32( m1, el->value );
  lo_message_add_float( m1, hid_element_map_logical( el ) );
  lo_message_add_float( m1, hid_element_map_physical( el ) );
  lo_message_add_int32( m1, el->array_value );
  lo_send_message_from( t, s, "/hid/element/data", m1 );
  lo_message_free(m1);
}

static void osc_descriptor_cb( struct hid_dev_desc *dd, void *data)
{
  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, *((int*) data) );
  lo_message_add_int32( m1, dd->device_collection->num_elements );
  lo_send_message_from( t, s, "/hid/device/data", m1 );
  lo_message_free(m1);
}

void close_all_devices(){
  hid_map_t::const_iterator it;
  for(it=hiddevices.begin(); it!=hiddevices.end(); ++it){
    struct hid_dev_desc * devdesc = it->second;
    hid_close_device( devdesc );
  }
  hiddevices.clear();
}

void open_device( unsigned short vendor, unsigned short product, const wchar_t *serial_number=NULL  ){
  
  struct hid_dev_desc * newdevdesc = hid_open_device( vendor, product, serial_number );
  
  if (!newdevdesc){
    fprintf(stderr, "Unable to open device %d, %d\n", vendor, product );
    if ( serial_number != NULL ){
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/open/error", "iis", vendor, product, serial_number );
    } else {
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/open/error", "ii", vendor, product );
    }
    return;
  } else {      
    hiddevices[ number_of_hids ] = newdevdesc;    
    if ( serial_number != NULL ){
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/open", "iiis", number_of_hids, product, vendor, serial_number );
    } else {
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/open", "iii", number_of_hids, product, vendor );
    }
    
    newdevdesc->index = number_of_hids;
    
    hid_set_descriptor_callback( newdevdesc, (hid_descriptor_callback) osc_descriptor_cb, &newdevdesc->index );
    hid_set_element_callback( newdevdesc, (hid_element_callback) osc_element_cb, &newdevdesc->index );  

    number_of_hids++;
  }
}

void close_device( int joy_idx ){
//   hid_map_t::const_iterator it = ;
  struct hid_dev_desc * hidtoclose = hiddevices.find( joy_idx )->second;
  if ( hidtoclose == NULL ){    
    lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/close/error", "i", joy_idx );
  } else {
    lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/closed", "iii", joy_idx, hidtoclose->info->vendor_id, hidtoclose->info->product_id );
    hid_close_device( hidtoclose );
    hiddevices.erase( joy_idx );
  }
}


void send_output_to_hid( int joy_idx, int reportid ){ 
  struct hid_dev_desc * hidtosendoutput = hiddevices.find( joy_idx )->second;
  hid_send_output_report( hidtosendoutput, reportid );
}

void set_element_output( int joy_idx, int elementid, int value ){ 
  struct hid_dev_desc * devd = hiddevices.find( joy_idx )->second;
  
  if ( devd != NULL ){
    // find the right output element
    struct hid_device_collection * device_collection = devd->device_collection;
    struct hid_device_element * cur_element = device_collection->first_element;
    
    while ( (cur_element->io_type != 2 || (cur_element->index != elementid)) && cur_element != NULL ){
	cur_element = hid_get_next_output_element(cur_element);
    }
    if ( cur_element != NULL ){
	cur_element->value = value;
    }
  }  
}


/// OSC bits

void error(int num, const char *m, const char *path);
int info_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);

// int openjoystick_handler(const char *path, const char *types, lo_arg **argv,
// 		    int argc, void *data, void *user_data);

int hid_open_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);
int hid_close_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);
int hid_info_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);

int hid_element_info_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);

int hid_element_output_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);

int hid_output_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data);


int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);
int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);

int init_osc( char * ip, char *outport, char * port ){
     /* create liblo addres */
    t = lo_address_new(ip, outport); // change later to use other host

    lo_server_thread st = lo_server_thread_new(port, error);

    lo_server_thread_add_method(st, "/hid/open", "ii", hid_open_handler, NULL);
    lo_server_thread_add_method(st, "/hid/element/info", "i", hid_element_info_handler, NULL);
    lo_server_thread_add_method(st, "/hid/info", "i", hid_info_handler, NULL);
    lo_server_thread_add_method(st, "/hid/close", "i", hid_close_handler, NULL);

    lo_server_thread_add_method(st, "/hid/element/output", "iii", hid_element_output_handler, NULL);
    lo_server_thread_add_method(st, "/hid/output", "ii", hid_output_handler, NULL);

    lo_server_thread_add_method(st, "/hidapi2osc/info", "", info_handler, NULL);
    lo_server_thread_add_method(st, "/hidapi2osc/quit", "", quit_handler, NULL);
    lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);

    lo_server_thread_start(st);
 
    lo_server s = lo_server_thread_get_server( st );

    lo_send_from( t, s, LO_TT_IMMEDIATE, "/hidapi2osc/started", "" ); 
}

lo_message get_hid_info_msg( struct hid_device_info * info )
{    
  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, info->vendor_id );
  lo_message_add_int32( m1, info->product_id );
  lo_message_add_string( m1, info->path );
  
  //TODO: fix for proper conversion of unicode to ascii
  wchar_t* wstr = info->serial_number;
  char* ascii;
  if ( wstr != NULL ){
    ascii = new char[wcslen(wstr) + 1];
    wcstombs( ascii, wstr, wcslen(wstr) );
    lo_message_add_string( m1, ascii );
    delete ascii;
  } else {
    lo_message_add_string( m1, "" );
  }

  wstr = info->manufacturer_string;
  if ( wstr != NULL ){
    ascii = new char[wcslen(wstr) + 1];
    wcstombs( ascii, wstr, wcslen(wstr) );
    lo_message_add_string( m1, ascii );
    delete ascii;
  } else {
    lo_message_add_string( m1, "" );
  }
  

  wstr = info->product_string;
  if ( wstr != NULL ){
    ascii = new char[wcslen(wstr) + 1];
    wcstombs( ascii, wstr, wcslen(wstr) );
    lo_message_add_string( m1, ascii );
    delete ascii;
  } else {
    lo_message_add_string( m1, "" );
  }
  
//   lo_message_add_string( m1, info->serial_number );
//   lo_message_add_string( m1, info->manufacturer_string );
//   lo_message_add_string( m1, info->product_string );
  
  lo_message_add_int32( m1, info->release_number );
  lo_message_add_int32( m1, info->interface_number );    
  return m1;
}

lo_message get_hid_element_info_msg( hid_device_element * el, int devid )
{    
  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, devid );
  lo_message_add_int32( m1, el->index );
  lo_message_add_int32( m1, el->usage_page );
  lo_message_add_int32( m1, el->usage );
  lo_message_add_int32( m1, el->io_type );
  lo_message_add_int32( m1, el->type );
  lo_message_add_int32( m1, el->logical_min );
  lo_message_add_int32( m1, el->logical_max );
  lo_message_add_int32( m1, el->phys_min );
  lo_message_add_int32( m1, el->phys_max );
  lo_message_add_int32( m1, el->unit_exponent );
  lo_message_add_int32( m1, el->unit );
  lo_message_add_int32( m1, el->report_size );
  lo_message_add_int32( m1, el->report_id );
  lo_message_add_int32( m1, el->report_index );
  lo_message_add_int32( m1, el->usage_min );
  lo_message_add_int32( m1, el->usage_max );
  return m1;
}


void error(int num, const char *msg, const char *path)
{
     printf("liblo server error %d in path %s: %s\n", num, path, msg);
     fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data)
{
    int i;

    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++) {
	printf("arg %d '%c' ", i, types[i]);
	lo_arg_pp( (lo_type) types[i], argv[i]);
	printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

int info_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{  
  struct hid_device_info *devs, *cur_dev;
  devs = hid_enumerate(0x0, 0x0);

  cur_dev = devs;
  int count = 0;
  while (cur_dev) {
    count++;
    cur_dev = cur_dev->next;
  }
 
  lo_bundle b = lo_bundle_new( LO_TT_IMMEDIATE );

  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, count );
  lo_bundle_add_message( b, "/hid/number", m1 );
  
  cur_dev = devs;
  while (cur_dev) {
    lo_message m2 = get_hid_info_msg( cur_dev );
    lo_bundle_add_message( b, "/hid/info", m2 );
    cur_dev = cur_dev->next;
  }

  if ( lo_send_bundle_from( t, s, b )  == -1 ){
	printf("hidapi2osc/info: OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
  }
  lo_bundle_free( b );  
  hid_free_enumeration(devs);
  
  fflush(stdout);
  return 0;
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
    done = 1;
    printf("hidapi2osc: allright, that's it, I quit\n");
    fflush(stdout);

    return 0;
}

void send_elements_hid_info(int joy_idx)
{
  hid_dev_desc * hid = hiddevices.find( joy_idx )->second;
  if ( hid == NULL ){
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/element/info/error", "i", joy_idx );
      return;
  }
  lo_bundle b = lo_bundle_new( LO_TT_IMMEDIATE );

  lo_message m1 = lo_message_new();
  lo_message_add_int32( m1, joy_idx );
  lo_message_add_int32( m1, hid->device_collection->num_elements );
  lo_bundle_add_message( b, "/hid/element/number", m1 );

  hid_device_element * cur_element = hid->device_collection->first_element;
  
  while (cur_element) {
    lo_message m2 = get_hid_element_info_msg( cur_element, joy_idx );
    lo_bundle_add_message( b, "/hid/element/info", m2 );
    cur_element = cur_element->next;
  }

  if ( lo_send_bundle_from( t, s, b )  == -1 ){
	printf("hidapi2osc/info: OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
  }
  lo_bundle_free( b );  
}

void send_hid_info(int joy_idx)
{
  hid_dev_desc * hid = hiddevices.find( joy_idx )->second;
  if ( hid != NULL ){
    lo_message m1 = get_hid_info_msg( hid->info );   
    lo_send_message_from( t, s, "/hid/info", m1 );
    lo_message_free(m1);
  } else {
    lo_send_from( t, s, LO_TT_IMMEDIATE, "/hid/info/error", "i", joy_idx );
  }
}

int hid_info_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
  printf("hidapi2osc: joystick info handler\n");

  send_hid_info( argv[0]->i );
  return 0;
}

int hid_element_info_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
  printf("hidapi2osc: joystick elements info handler\n");

  send_elements_hid_info( argv[0]->i );
  return 0;
}

int hid_element_output_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
  printf("hidapi2osc: joystick elements output handler\n");
  set_element_output( argv[0]->i, argv[1]->i, argv[2]->i );
  return 0;
}

int hid_output_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
  printf("hidapi2osc: joystick output handler\n");
  send_output_to_hid( argv[0]->i, argv[1]->i );
  return 0;
}

int hid_open_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
//   printf("hidapi2osc: joystick open handler\n");
  open_device( argv[0]->i, argv[1]->i, NULL );
  return 0;
}

int hid_close_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
  printf("hidapi2osc: joystick close handler, %i\n", argv[0]->i );
  close_device( argv[0]->i );
  return 0;
}


 
/// end OSC stuff


int str2int(const char* str, int* val)
{
  char* endptr;
  errno = 0;    /* To distinguish success/failure after call */

  *val = strtol(str, &endptr, 10);

  /* Check for various possible errors */
  if ((errno == ERANGE && (*val == LONG_MAX || *val == LONG_MIN))
      || (errno != 0 && *val == 0)) {
    return 0;
  }

  if (endptr == str) {
    return 0;
  }

  return 1;
}

void print_device_info( hid_device *handle ){
  wchar_t wstr[MAX_STR];  
  int res;
  	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);
}

void list_devices( void ){
	struct hid_device_info *devs, *cur_dev;
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
}

void print_help(const char* prg)
{
  printf("Usage: %s [OPTION]\n", prg);
//   printf("List available joysticks or test a  joystick.\n");
//   printf("This programm uses SDL for doing its test instead of using the raw\n"
//          "/dev/input/jsX interface\n");
  printf("\n");
  printf("Options:\n");
  printf("  --help             Print this help\n");
  printf("  --version          Print version number and exit\n");
  printf("  --list             Search for available joysticks and list their properties\n");
//   printf("  --event JOYNUM     Display the events that are received from the joystick\n");
  printf("  --osc     	       Send the events that are received from the joystick\n");
  printf("\n");
  printf("Examples:\n");
  printf("  %s --list\n", prg);
//   printf("  %s --test 1\n", prg);
  printf("  %s --osc\n", prg);
}



int main(int argc, char** argv)
{
  if (argc == 1)
  {
    print_help(argv[0]);
    exit(1);
  }
  
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 ||
                      strcmp(argv[1], "-h") == 0))
    {
      print_help(argv[0]);
    }
    if (argc == 2 && (strcmp(argv[1], "--version") == 0))
    {
      printf("hidapi2osc 0.1.0\n");
      exit(EXIT_SUCCESS);
    }
    else if (argc == 2 && (strcmp(argv[1], "--list") == 0 ||
                           (strcmp(argv[1], "-l") == 0)))
    {
      if (hid_init())
	return -1;
      list_devices();
    }
//     else if (argc == 3 && (strcmp(argv[1], "--event") == 0 ||
//                            strcmp(argv[1], "-e") == 0))
//     {
//       int joy_idx;
//       if (!str2int(argv[2], &joy_idx))
//       {
//         fprintf(stderr, "Error: JOYSTICKNUM argument must be a number, but was '%s'\n", argv[2]);
//         exit(1);
//       }
// 
//       }
//       fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
//     }
    else if (argc == 2 && (strcmp(argv[1], "--osc") == 0 ||
                           strcmp(argv[1], "-o") == 0))
    {
  
      char *port = "57151";
      char *outport = "57120";
      char *ip = "127.0.0.1";
      
      if ( argc == 5 )
	{
	ip = argv[4];
	port = argv[3];
	outport = argv[2];
	}
      else if ( argc == 4 )
	{
	port = argv[3];
	outport = argv[2];
	}
      else if ( argc == 3 )
	{
	outport = argv[2];
	}
  
      init_osc( ip, outport, port );

      if (hid_init())
	return -1;
    
      printf("Entering hid read loop, press Ctrl-c to exit\n");

      int res = 0;
      hid_map_t::const_iterator it;
      unsigned char buf[256];
      while(!done){
	for(it=hiddevices.begin(); it!=hiddevices.end(); ++it){
	  res = hid_read( it->second->device, buf, sizeof(buf));
	  if ( res > 0 ) {
	    hid_parse_input_report( buf, res, it->second );
	  }
	}
	#ifdef WIN32
	Sleep(50);
	#else
	usleep(500*10);
	#endif
      }
      close_all_devices();
	  
      lo_send_from( t, s, LO_TT_IMMEDIATE, "/hidapi2osc/quit", "s", "nothing more to do, quitting" );
      lo_server_thread_free( st );
      lo_address_free( t );
    }
    else
    {
      fprintf(stderr, "%s: unknown arguments\n", argv[0]);
      fprintf(stderr, "Try '%s --help' for more informations\n", argv[0]);
    }
}

/* EOF */
