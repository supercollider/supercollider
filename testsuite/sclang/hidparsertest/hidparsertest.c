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

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
// #include <conio.h>

#include <hidapi.h>
#include "hidapi_parser.h"


// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

void list_devices( void ){
	struct hid_device_info *devs, *cur_dev;
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: 0x%04hx 0x%04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
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

// static hid_device * open_device( char vendor_id, char product_id ){
// 	handle = 
// 	return handle;
// }

#define MAX_STR 255

void print_element_info( struct hid_device_element *element ){
  
  printf( "index: %i, usage_page: %i, usage: %i, iotype: %i, type: %i, \n \
	  \tusage_min: %i, usage_max: %i, \n \
	  \tlogical_min: %i, logical_max: %i, \n \
	  \tphys_min: %i, phys_max: %i, unit_exponent: %i, unit: %i, \n \
	  \treport_size: %i, report_id: %i, report_index: %i \n",
	  element->index, element->usage_page, element->usage, element->io_type, element->type,
	  element->usage_min, element->usage_max,
	  element->logical_min, element->logical_max,
	  element->phys_min, element->phys_max, 
	  element->unit_exponent, element->unit,
	  element->report_size, element->report_id, element->report_index );
}

void print_collection_info( struct hid_device_collection *collection ){
  int i;
  printf( "COLLECTION index: %i, usage_page: %i, usage: %i, num_elements: %i, num_collections: %i \n",
	  collection->index, collection->usage_page, collection->usage_index, 
	  collection->num_elements, collection->num_collections );
 
  struct hid_device_collection * cur_collection = collection->first_collection;
  
  printf( "number of collections in collection: %i\n", collection->num_collections );
  for ( i=0; i<collection->num_collections; i++ ){
    if ( cur_collection != NULL ){
      printf("cur_collection %i\n", cur_collection );
      print_collection_info( cur_collection );
      cur_collection = cur_collection->next_collection;
    }
  }

  
  struct hid_device_element * cur_element = collection->first_element;
  
  printf( "number of elements in collection: %i\n", collection->num_elements );
  for ( i=0; i<collection->num_elements; i++ ){
    if ( cur_element != NULL ){
      printf("cur_element %i\n", cur_element );
      print_element_info( cur_element );
      cur_element = cur_element->next;
    }
  }
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

static void my_element_cb(const struct hid_device_element *el, void *data)
{
    printf("in %s\t", __func__);
    printf("element: usage %i, value %i, index %i\t", el->usage, el->value, el->index );
    printf("user_data: %s\n", (const char *)data);
}

static void my_descriptor_cb(const struct hid_dev_desc *dd, void *data)
{
    printf("in %s\t", __func__);
//     printf("element: usage %i, value %i, index %i\n", el->usage, el->value, el->index );
    printf("user_data: %s\n", (const char *)data);
}

int main(int argc, char* argv[]){

  int res;
  unsigned char buf[256];
  unsigned char descr_buf[HIDAPI_MAX_DESCRIPTOR_SIZE];
    
  struct hid_dev_desc *devdesc;
//   struct hid_device_descriptor *descriptor;
//   hid_device *handle;
  
#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

  if (hid_init())
    return -1;
  list_devices();
  
  int vendor_id;
  int product_id;
  
  if (argc == 3 ){          
    vendor_id = atoi( argv[1] );
    product_id = atoi( argv[2] );
  } else {
    printf( "please run again with vendor and product id to open specified device, e.g. hidparsertest 0x044f 0xd003\n" );
      return 0;
  }
  printf( "vendor %i, product %i", vendor_id, product_id );
  

  devdesc = hid_open_device( vendor_id, product_id, NULL );
  if (!devdesc){
    fprintf(stderr, "Unable to open device %d, %d\n", vendor_id, product_id );
    return 1;
  }
//   handle = hid_open( 0x044f, 0xd003, NULL);
//   if (!handle) {
// 	printf("unable to open device\n");
//  	return 1;
//   }
//   print_device_info( handle );
  
  print_device_info( devdesc->device );
    
//   descriptor = hid_read_descriptor( handle );
//   if ( descriptor == NULL ){
//     printf("unable to read descriptor\n");
//     return 1;
//   }

//   res = hid_get_report_descriptor( handle, descr_buf, HIDAPI_MAX_DESCRIPTOR_SIZE );
//   if (res < 0){
//     printf("Unable to read report descriptor\n");
//     return 1;
//   } else {
//     descriptor = (hid_device_descriptor *) malloc( sizeof( hid_device_descriptor) );
//     hid_descriptor_init( descriptor );
//     hid_parse_report_descriptor( descr_buf, res, descriptor );
// 
//   }

  // Set the hid_read() function to be non-blocking.
//   hid_set_nonblocking(handle, 1);

  print_collection_info( devdesc->device_collection );
  
//   printf("press key to continue\n" );
//   getchar();

  struct hid_device_element * cur_element = devdesc->device_collection->first_element;
  
  printf( "number of elements in device: %i\n", devdesc->device_collection->num_elements );
  while (cur_element != NULL ) {
    printf("cur_element %i\n", cur_element );
    print_element_info( cur_element );
//     printf("press key to continue\n" );
//     getchar();
    cur_element = cur_element->next;
  }
  
//   printf("press key to continue\n" );
//   getchar();

  char my_custom_data[40] = "Hello!";
  hid_set_descriptor_callback( devdesc, (hid_descriptor_callback) my_descriptor_cb, my_custom_data );
  hid_set_element_callback( devdesc, (hid_element_callback) my_element_cb, my_custom_data );  
  
//   Request state (cmd 0x81). The first byte is the report number (0x1).
//   buf[0] = 0x1;
//   buf[1] = 0x81;
//   hid_write(handle, buf, 17);
//   if (res < 0)
// 	  printf("Unable to write() (2)\n");

 	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while (1) {
		res = hid_read(devdesc->device, buf, sizeof(buf));
		if ( res > 0 ) {
		  hid_parse_input_report( buf, res, devdesc );
		}
		#ifdef WIN32
		Sleep(500);
		#else
		usleep(500*100);
		#endif
	}

	hid_close_device( devdesc );
// 	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	system("pause");
#endif

	return 0;
 
}
