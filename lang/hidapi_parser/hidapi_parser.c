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
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
// for MinGW < 5.3 include locally provided "../windows hidsdi.h" rather
// than #include <hidsdi.h>:
// #include "../windows/hidsdi.h"
#include <hidsdi.h>
#endif

#include "hidapi_parser.h"



// SET IN CMAKE
// #define DEBUG_PARSER

//// ---------- HID descriptor parser

// main items
#define HID_INPUT 0x80
#define HID_OUTPUT 0x90
#define HID_COLLECTION 0xA0
#define HID_FEATURE 0xB0
#define HID_END_COLLECTION 0xC0

// HID Report Items from HID 1.11 Section 6.2.2
#define HID_USAGE_PAGE 0x04
#define HID_USAGE 0x08
#define HID_USAGE_MIN 0x18
#define HID_USAGE_MAX 0x28

#define HID_DESIGNATOR_INDEX 0x38
#define HID_DESIGNATOR_MIN 0x48
#define HID_DESIGNATOR_MAX 0x58

#define HID_STRING_INDEX 0x78
#define HID_STRING_MIN 0x88
#define HID_STRING_MAX 0x98

#define HID_DELIMITER 0xA8

#define HID_LOGICAL_MIN 0x14
#define HID_LOGICAL_MAX 0x24

#define HID_PHYSICAL_MIN 0x34
#define HID_PHYSICAL_MAX 0x44

#define HID_UNIT_EXPONENT 0x54
#define HID_UNIT 0x64

#define HID_REPORT_SIZE 0x74
#define HID_REPORT_ID 0x84

#define HID_REPORT_COUNT 0x94

#define HID_PUSH 0xA4
#define HID_POP 0xB4

#define HID_RESERVED 0xC4 // above this it is all reserved


// HID Report Usage Pages from HID Usage Tables 1.12 Section 3, Table 1
// #define HID_USAGE_PAGE_GENERICDESKTOP  0x01
// #define HID_USAGE_PAGE_KEY_CODES       0x07
// #define HID_USAGE_PAGE_LEDS            0x08
// #define HID_USAGE_PAGE_BUTTONS         0x09

// HID Report Usages from HID Usage Tables 1.12 Section 4, Table 6
// #define HID_USAGE_POINTER  0x01
// #define HID_USAGE_MOUSE    0x02
// #define HID_USAGE_JOYSTICK 0x04
// #define HID_USAGE_KEYBOARD 0x06
// #define HID_USAGE_X        0x30
// #define HID_USAGE_Y        0x31
// #define HID_USAGE_Z        0x32
// #define HID_USAGE_RX       0x33
// #define HID_USAGE_RY       0x34
// #define HID_USAGE_RZ       0x35
// #define HID_USAGE_SLIDER   0x36
// #define HID_USAGE_DIAL     0x37
// #define HID_USAGE_WHEEL    0x38


// HID Report Collection Types from HID 1.12 6.2.2.6
#define HID_COLLECTION_PHYSICAL    0x00
#define HID_COLLECTION_APPLICATION 0x01
#define HID_COLLECTION_LOGICAL 0x02
#define HID_COLLECTION_REPORT 0x03
#define HID_COLLECTION_NAMED_ARRAY 0x04
#define HID_COLLECTION_USAGE_SWITCH 0x05
#define HID_COLLECTION_USAGE_MODIFIER 0x06
#define HID_COLLECTION_RESERVED 0x07
#define HID_COLLECTION_VENDOR 0x80

// HID Input/Output/Feature Item Data (attributes) from HID 1.11 6.2.2.5
/// more like flags - for input, output, and feature
#define HID_ITEM_CONSTANT 0x1 // data(0), constant(1)
#define HID_ITEM_VARIABLE 0x2 // array(0), variable(1)
#define HID_ITEM_RELATIVE 0x4 // absolute(0), relative(1)
#define HID_ITEM_WRAP 0x8 // no wrap(0), wrap(1)
#define HID_ITEM_LINEAR 0x10 // linear(0), non linear(1)
#define HID_ITEM_PREFERRED 0x20 // no preferred(0), preferred(1)
#define HID_ITEM_NULL 0x40 // no null(0), null(1)
#define HID_ITEM_VOLATILE 0x60 // non volatile(0), volatile(1)
#define HID_ITEM_BITFIELD 0x80 // bit field(0), buffered bytes(1)

// Report Types from HID 1.11 Section 7.2.1
#define HID_REPORT_TYPE_INPUT   1
#define HID_REPORT_TYPE_OUTPUT  2
#define HID_REPORT_TYPE_FEATURE 3


#define BITMASK1(n) ((1ULL << (n)) - 1ULL)
#define INVERTBITMASK1(n) ( 255 -(n))
#define BITTESTMASK1(n) (1ULL << (n))

// struct hid_device_descriptor * hid_new_descriptor(){
//   struct hid_device_descriptor * descriptor;
//   descriptor = (struct hid_device_descriptor *) malloc( sizeof( struct hid_device_descriptor) );
// //     hid_descriptor_init( descriptor );
//
//   descriptor->first = NULL;
//   hid_set_descriptor_callback(descriptor, NULL, NULL);
//   hid_set_element_callback(descriptor, NULL, NULL);
//   return descriptor;
// }

struct hid_device_element * hid_new_element(){
  struct hid_device_element * element = (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
  element->next = NULL;
  element->parent_collection = NULL;
  element->index = -1;
  element->repeat = 0;

  element->usage_min = 0;
  element->usage_max = 0;
  element->logical_min = 0;
  element->logical_max = 0;
  element->phys_min = 0;
  element->phys_max = 0;
  element->report_id = 0;
  element->unit = 0;
  element->unit_exponent = 0;

  element->rawvalue = 0;

  return element;
}

void hid_free_element( struct hid_device_element * ele ){
  free( ele );
}

struct hid_device_collection * hid_new_collection(){
  struct hid_device_collection * collection = (struct hid_device_collection *) malloc( sizeof( struct hid_device_collection ) );
  collection->first_collection = NULL;
  collection->next_collection = NULL;
  collection->parent_collection = NULL;
  collection->first_element = NULL;
  collection->num_collections = 0;
  collection->num_elements = 0;
  collection->index = -1;
  collection->usage_page = 0;
  collection->usage_index = 0;
  return collection;
}

void hid_free_collection( struct hid_device_collection * coll ){
  struct hid_device_element * cur_element = coll->first_element;
  struct hid_device_element * next_element;
  while (cur_element != NULL ) {
    next_element = cur_element->next;
    free( cur_element );
    cur_element = next_element;
  }
  struct hid_device_collection * cur_collection = coll->first_collection;
  struct hid_device_collection * next_collection;
  while (cur_collection != NULL ) {
    next_collection = cur_collection->next_collection;
    free( cur_collection );
    cur_collection = next_collection;
  }
  free( coll );
}

void hid_set_descriptor_callback( struct hid_dev_desc * devd, hid_descriptor_callback cb, void *user_data ){
    devd->_descriptor_callback = cb;
    devd->_descriptor_data = user_data;
}

void hid_set_readerror_callback( struct hid_dev_desc * devd, hid_descriptor_callback cb, void *user_data ){
    devd->_readerror_callback = cb;
    devd->_readerror_data = user_data;
}

void hid_set_element_callback( struct hid_dev_desc * devd, hid_element_callback cb, void *user_data ){
    devd->_element_callback = cb;
    devd->_element_data = user_data;
}

void hid_set_from_making_element( struct hid_device_element * making, struct hid_device_element * new_element ){

	new_element->type = making->type;
	new_element->isrelative = (making->type & HID_ITEM_RELATIVE ) > 0;
	new_element->isarray = ( (making->type & HID_ITEM_VARIABLE ) == 0 );
	new_element->isvariable = (making->type & HID_ITEM_CONSTANT ) == 0;

	new_element->usage_page = making->usage_page;
	new_element->logical_min = making->logical_min;
	new_element->logical_max = making->logical_max;
	new_element->usage_min = making->usage_min;
	new_element->usage_max = making->usage_max;
	if ( (making->phys_min == 0) && (making->phys_max == 0) ){
	  new_element->phys_min = making->logical_min;
	  new_element->phys_max = making->logical_max;

	} else {
	  new_element->phys_min = making->phys_min;
	  new_element->phys_max = making->phys_max;
	}
	new_element->unit = making->unit;
	new_element->unit_exponent = making->unit_exponent;

	new_element->report_size = making->report_size;
	new_element->report_id = making->report_id;
}

int hid_element_get_signed_value( int inputvalue, int bytesize ){
  int outputvalue;
  int bitSignIndex = bytesize*8 - 1;
  int signBit = 0x1 << bitSignIndex;
  if ( signBit & inputvalue ){
    unsigned int bitMask = BITMASK1( bytesize*8 );
    unsigned int uvalue = (unsigned int) inputvalue;
    unsigned int negvalue = ~(uvalue);
    negvalue = ~(uvalue) & bitMask;
    negvalue = negvalue + 1;
    outputvalue = -1 * negvalue;
  } else {
    outputvalue = inputvalue;
  }
  return outputvalue;
}

// int hid_parse_report_descriptor( char* descr_buf, int size, struct hid_device_descriptor * descriptor ){
int hid_parse_report_descriptor( unsigned char* descr_buf, int size, struct hid_dev_desc * device_desc ){
  struct hid_device_collection * device_collection = hid_new_collection();
  device_desc->device_collection = device_collection;

  struct hid_device_collection * parent_collection = device_desc->device_collection;
  struct hid_device_collection * prev_collection = 0;
  struct hid_device_element * prev_element = 0;

  struct hid_device_element * making_element = hid_new_element();

  int current_usages[256];
  int current_usage_index = 0;
  int current_report_size;

  int current_usage_min = -1;
  int current_usage_max = -1;

  int current_report_count = 0;

//   unsigned char current_input;
//   unsigned char current_output;
  int collection_nesting = 0;

  int next_byte_tag = -1;
  int next_byte_size = 0;
  int next_byte_type = 0;
  int next_val = 0;

  unsigned char toadd = 0;
  int byte_count = 0;

  int i,j;

  int numreports = 1;
  int report_lengths[256];
  int report_ids[256];
  report_ids[0] = 0;
  report_lengths[0] = 0;

  int k;
  int index;

  device_collection->num_collections = 0;
  device_collection->num_elements = 0;
#ifdef DEBUG_PARSER
  printf("----------- parsing report descriptor --------------\n " );
#endif
  for ( i = 0; i < size; i++){
#ifdef DEBUG_PARSER
	  printf("\nbuffer value: %02hhx ", descr_buf[i]);
	  printf("\tbyte_type %i, %i, %i \t", next_byte_tag, next_byte_size, next_val);
#endif
	  if ( next_byte_tag != -1 ){
// 	      unsigned char ubyte = (unsigned char) descr_buf[i];
// 	      char sbyte = descr_buf[i]; // descr_buf is signed already
	      int shift = byte_count*8;
	      int bufval = (int) descr_buf[i];
	      next_val |= (bufval << shift);
#ifdef DEBUG_PARSER
	      printf("\t nextval shift: %i", next_val);
#endif
	      byte_count++;
	      if ( byte_count == next_byte_size ){
		switch( next_byte_tag ){
		  case HID_USAGE_PAGE:
		    making_element->usage_page = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\tusage page: 0x%02hhx", making_element->usage_page);
#endif
		    break;
		  case HID_USAGE:
		    making_element->usage = next_val;
		    current_usage_min = -1;
		    current_usage_max = -1;
		    current_usages[ current_usage_index ] = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\tusage: 0x%02hhx, %i", current_usages[ current_usage_index ], current_usage_index );
#endif
		    current_usage_index++;
		    break;
		  case HID_COLLECTION:
		  {
		    //TODO: COULD ALSO READ WHICH KIND OF COLLECTION
		    struct hid_device_collection * new_collection = hid_new_collection();
		    if ( parent_collection->num_collections == 0 ){
		      parent_collection->first_collection = new_collection;
		    }
		    if ( device_collection->num_collections == 0 ){
		      device_collection->first_collection = new_collection;
		    } else {
		      prev_collection->next_collection = new_collection;
		    }
		    new_collection->parent_collection = parent_collection;
		    new_collection->type = next_val;
		    new_collection->usage_page = making_element->usage_page;
		    new_collection->usage_index = making_element->usage;
		    new_collection->usage_min = making_element->usage_min;
		    new_collection->usage_max = making_element->usage_max;
		    new_collection->index = device_collection->num_collections;
		    device_collection->num_collections++;
		    if ( device_collection != parent_collection ){
		      parent_collection->num_collections++;
		    }
		    parent_collection = new_collection;
		    prev_collection = new_collection;
		    collection_nesting++;
#ifdef DEBUG_PARSER
		    printf("\n\tcollection: %i, %i", collection_nesting, next_val );
#endif
		    break;
		  }
		  case HID_USAGE_MIN:
		    current_usage_min = next_val;
		    making_element->usage_min = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\tusage min: %i", current_usage_min);
#endif
		    break;
		  case HID_USAGE_MAX:
		    current_usage_max = next_val;
		    making_element->usage_max = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\tusage max: %i", current_usage_max);
#endif
		    break;
		  case HID_LOGICAL_MIN:
		    making_element->logical_min = hid_element_get_signed_value( next_val, byte_count );
#ifdef DEBUG_PARSER
		    printf("\n\tlogical min: %i", making_element->logical_min);
#endif
		    break;
		  case HID_LOGICAL_MAX:
 		    if ( making_element->logical_min >= 0 ){
 		      making_element->logical_max = next_val;
 		    } else {
		      making_element->logical_max = hid_element_get_signed_value( next_val, byte_count );
		    }
#ifdef DEBUG_PARSER
		    printf("\n\tlogical max: %i", making_element->logical_max);
#endif
		    break;
		  case HID_PHYSICAL_MIN:
		    making_element->phys_min = hid_element_get_signed_value( next_val, byte_count );
#ifdef DEBUG_PARSER
		    printf("\n\tphysical min: %i", making_element->phys_min);
#endif
		    break;
		  case HID_PHYSICAL_MAX:
 		    if ( making_element->phys_min >= 0 ){
 		      making_element->phys_max = next_val;
 		    } else {
		      making_element->phys_max = hid_element_get_signed_value( next_val, byte_count );
		    }
#ifdef DEBUG_PARSER
		    printf("\n\tphysical max: %i", making_element->phys_min);
#endif
		    break;
		  case HID_REPORT_COUNT:
		    current_report_count = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\treport count: %i", current_report_count);
#endif
		    break;
		  case HID_REPORT_SIZE:
		    making_element->report_size = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\treport size: %i", making_element->report_size);
#endif
		    break;
		  case HID_REPORT_ID:
		    making_element->report_id = next_val;
		    // check if report id already exists
		    int reportexists = 0;
		    for ( j = 0; j < numreports; j++ ){
		      reportexists = (report_ids[j] == making_element->report_id);
		    }
		    if ( !reportexists ){
		      report_ids[ numreports ] = making_element->report_id;
		      report_lengths[ numreports ] = 0;
		      numreports++;
		    }
#ifdef DEBUG_PARSER
		    printf("\n\treport id: %i", making_element->report_id);
#endif
		    break;
		  case HID_POP:
		    // TODO: something useful with pop
#ifdef DEBUG_PARSER
		    printf("\n\tpop: %i", next_val );
#endif
		    break;
		  case HID_PUSH:
		    // TODO: something useful with push
#ifdef DEBUG_PARSER
		    printf("\n\tpop: %i", next_val );
#endif
		    break;
		  case HID_UNIT:
		    making_element->unit = next_val;
#ifdef DEBUG_PARSER
		    printf("\n\tunit: %i", next_val );
#endif
		    break;
		  case HID_UNIT_EXPONENT:
		    making_element->unit_exponent = hid_element_get_signed_value( next_val, byte_count );
#ifdef DEBUG_PARSER
		    printf("\n\tunit exponent: %i", next_val );
#endif
		    break;
		  case HID_INPUT:
#ifdef DEBUG_PARSER
		    printf("\n\tinput: %i", next_val);
		    printf("\tmaking_element->usage: %i", making_element->usage);
#endif
		    making_element->type = next_val;
		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = hid_new_element();
// 			= (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
			new_element->io_type = 1;
			new_element->index = device_collection->num_elements;
			new_element->parent_collection = parent_collection;
			hid_set_from_making_element( making_element, new_element );
			if ( current_usage_min == -1 ){
			  new_element->usage = current_usages[j]; /// FIXME
			} else {
			  new_element->usage = current_usage_min + j;
			}
			new_element->report_index = j;

			new_element->value = 0;
			new_element->array_value = 0;
			if ( parent_collection->num_elements == 0 ){
			    parent_collection->first_element = new_element;
			}
			if ( device_collection->num_elements == 0 ){
			    device_collection->first_element = new_element;
			}
			device_collection->num_elements++;
			if ( parent_collection != device_collection ) {
			  parent_collection->num_elements++;
			}
			if ( prev_element != NULL ){
			    prev_element->next = new_element;
			}
			prev_element = new_element;
		    }
		    for ( j=0; j < current_usage_index; j++ ){
			current_usages[j] = 0;
		    }
		    current_usage_index = 0;
		    current_usage_min = -1;
		    current_usage_max = -1;
		    making_element->usage_min = -1;
		    making_element->usage_max = -1;
		    making_element->usage = 0;
		    break;
		  case HID_OUTPUT:
#ifdef DEBUG_PARSER
		    printf("\n\toutput: %i", next_val);
		    printf("\tmaking_element->usage: %i", making_element->usage);
#endif
		    making_element->type = next_val;
		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = hid_new_element();
// 			struct hid_device_element * new_element = (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
			new_element->io_type = 2;
			new_element->index = device_collection->num_elements;
			new_element->parent_collection = parent_collection;
			hid_set_from_making_element( making_element, new_element );
			if ( current_usage_min == -1 ){
			  new_element->usage = current_usages[j]; /// FIXME
			} else {
			  new_element->usage = current_usage_min + j;
			}
			new_element->report_index = j;

			index = 0;
			for ( k=0; k<numreports; k++ ){
			  if ( making_element->report_id == report_ids[k] ){
			    index = k;
			    break;
			  }
			}
			report_lengths[index] += making_element->report_size;

			new_element->value = 0;
			new_element->array_value = 0;
			if ( parent_collection->num_elements == 0 ){
			    parent_collection->first_element = new_element;
			}
			if ( device_collection->num_elements == 0 ){
			    device_collection->first_element = new_element;
			}
			device_collection->num_elements++;
			if ( parent_collection != device_collection ) {
			  parent_collection->num_elements++;
			}
			if ( prev_element != NULL ){
			    prev_element->next = new_element;
			}
			prev_element = new_element;
		    }
		    for ( j=0; j < current_usage_index; j++ ){
			current_usages[j] = 0;
		    }
		    current_usage_index = 0;
		    current_usage_min = -1;
		    current_usage_max = -1;
		    making_element->usage_min = -1;
		    making_element->usage_max = -1;
		    making_element->usage = 0;
		    break;
		  case HID_FEATURE:
#ifdef DEBUG_PARSER
		    printf("\n\tfeature: %i", next_val);
		    printf("\tcurrent_usage: %i", making_element->usage);
#endif
		    making_element->type = next_val;
		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = hid_new_element();
			new_element->io_type = 3;
			new_element->index = device_collection->num_elements;
			new_element->parent_collection = parent_collection;
			hid_set_from_making_element( making_element, new_element );
			if ( current_usage_min == -1 ){
			  new_element->usage = current_usages[j]; /// FIXME
			} else {
			  new_element->usage = current_usage_min + j;
			}
			new_element->report_index = j;

			new_element->value = 0;
			new_element->array_value = 0;
			if ( parent_collection->num_elements == 0 ){
			    parent_collection->first_element = new_element;
			}
			if ( device_collection->num_elements == 0 ){
			    device_collection->first_element = new_element;
			}
			device_collection->num_elements++;
			if ( parent_collection != device_collection ) {
			  parent_collection->num_elements++;
			}
			if ( prev_element != NULL ){
			    prev_element->next = new_element;
			}
			prev_element = new_element;
		    }
		    for ( j=0; j < current_usage_index; j++ ){
			current_usages[j] = 0;
		    }
		    current_usage_index = 0;
		    current_usage_min = -1;
		    current_usage_max = -1;
		    making_element->usage_min = -1;
		    making_element->usage_max = -1;
		    making_element->usage = 0;
		    break;
#ifdef DEBUG_PARSER
		  default:
		    if ( next_byte_tag >= HID_RESERVED ){
		      printf("\n\treserved bytes 0x%02hhx, %i", next_byte_tag, next_val );
		    } else {
		      printf("\n\tundefined byte type 0x%02hhx, %i", next_byte_tag, next_val );
		    }
#endif
		}
	      next_byte_tag = -1;
	      }
	  } else {
#ifdef DEBUG_PARSER
	    printf("\tsetting next byte type: %i, 0x%02hhx ", descr_buf[i], descr_buf[i] );
#endif
	    if ( descr_buf[i] == HID_END_COLLECTION ){ // JUST one byte
// 	      prev_collection = parent_collection;
	      making_element->usage_page = parent_collection->usage_page;
	      making_element->usage = parent_collection->usage_index;
	      parent_collection = parent_collection->parent_collection;
	      for ( j=0; j < current_usage_index; j++ ){
		current_usages[j] = 0;
	      }
	      making_element->usage_min = -1;
	      making_element->usage_max = -1;
	      current_usage_index = 0;
	      current_usage_min = -1;
	      current_usage_max = -1;
	      collection_nesting--;
#ifdef DEBUG_PARSER
	      printf("\n\tend collection: %i, %i\n", collection_nesting, descr_buf[i] );
#endif
	    } else {
	      byte_count = 0;
	      next_val = 0;
// 	      u_next_val = 0;
	      next_byte_tag = descr_buf[i] & 0xFC;
	      next_byte_type = descr_buf[i] & 0x0C;
	      next_byte_size = descr_buf[i] & 0x03;
	      if ( next_byte_size == 3 ){
		  next_byte_size = 4;
	      }
#ifdef DEBUG_PARSER
	      printf("\t next byte type:  0x%02hhx, %i, %i ", next_byte_tag, next_byte_type, next_byte_size );
#endif
	    }
	  }
  }
#ifdef DEBUG_PARSER
  printf("----------- end parsing report descriptor --------------\n " );
#endif

  device_desc->number_of_reports = numreports;
  device_desc->report_lengths = (int*) malloc( sizeof( int ) * numreports );
  device_desc->report_ids = (int*) malloc( sizeof( int ) * numreports );
  for ( j = 0; j<numreports; j++ ){
      device_desc->report_lengths[j] = report_lengths[j];
      device_desc->report_ids[j] = report_ids[j];
  }

#ifdef DEBUG_PARSER
  printf("----------- end setting report ids --------------\n " );
#endif

  return 0;
}

void hid_element_set_value_from_input( struct hid_device_element * element, int value ){
    element->rawvalue = value;
    if (element->logical_min < 0){
        // value should be interpreted as signed value
        // so: check report size, test the highest bit, if one, invert and add one, otherwise keep value
        int bitSignIndex = element->report_size - 1;
        int signBit = 0x1 << bitSignIndex;
        if (signBit & value){
            unsigned int bitMask = BITMASK1(element->report_size);
            unsigned int uvalue = (unsigned int)value;
            unsigned int negvalue = ~(uvalue);
            negvalue = ~(uvalue)& bitMask;
            negvalue = negvalue + 1;
            element->value = -1 * negvalue;
        }
        else {
            element->value = value;
        }
    }
    else {
        // value should be interpreted as unsigned value
        // so: keep value as is
        if (element->isarray){ // array elements should be parsed differently
            if (value == 0){ // previous key was pressed, so keep previous usage
                element->value = 0;
                element->array_value = 0;
            }
            else { // new key, so value + usage min is the current usage
                element->usage = element->usage_min + value;
                element->value = 1;
                element->array_value = value;
            }
        }
        else {
            element->value = value;
        }
    }
}

float hid_element_map_logical( struct hid_device_element * element ){
  float result;
  if ( element->isarray ){
    result = (float) element->value;
  } else {
    result = ( (float) element->value - (float) element->logical_min)/( (float) element->logical_max - (float) element->logical_min );
  }
  return result;
}

/** TODO: this needs a linking with the math library */
float hid_element_resolution( struct hid_device_element * element ){
    float result = 0;
//     result = ( element->logical_max - element->logical_min) / ( ( element->phys_max - element->phys_min) * pow(10, element->unit_exponent) );
    return result;
}

float hid_element_map_physical( struct hid_device_element * element ){
  float result;
  float logicalvalue = hid_element_map_logical(element);
  result = logicalvalue * ( element->phys_max - element->phys_min ) + element->phys_min;
  return result;
}

/** is this used anywhere? */
void hid_element_set_rawvalue( struct hid_device_element * element, int value ){
  element->value = value;
}

/** is this used anywhere? */
void hid_element_set_logicalvalue( struct hid_device_element * element, float value ){
  int mapvalue;
  mapvalue = (int) ( value * ( (float) element->logical_max - (float) element->logical_min ) ) - element->logical_min;
  element->value = mapvalue;
}

struct hid_device_element * hid_get_next_input_element( struct hid_device_element * curel ){

  struct hid_device_element * nextel = curel->next;
  while ( nextel != NULL ){
      if ( nextel->io_type == 1 ){
	  return nextel;
      } else {
	  nextel = nextel->next;
      }
  }
  return nextel; // is NULL
//  return curel; // return the previous element
  // is NULL
}

struct hid_device_element * hid_get_next_input_element_with_reportid( struct hid_device_element * curel, int reportid ){

  struct hid_device_element * nextel = curel->next;
  while ( nextel != NULL ){
      if ( nextel->io_type == 1 && ( nextel->report_id == reportid ) ){
	  return nextel;
      } else {
	  nextel = nextel->next;
      }
  }
  return nextel;
 // return curel; // return the previous element
  // is NULL
}

struct hid_device_element * hid_get_next_output_element( struct hid_device_element * curel ){

  struct hid_device_element * nextel = curel->next;
  while ( nextel != NULL ){
      if ( nextel->io_type == 2 ){
	  return nextel;
      } else {
	  nextel = nextel->next;
      }
  }
  return nextel;
//  return curel; // return the previous element
  // is NULL
}

struct hid_device_element * hid_get_next_output_element_with_reportid( struct hid_device_element * curel, int reportid ){

  struct hid_device_element * nextel = curel->next;
  while ( nextel != NULL ){
      if ( nextel->io_type == 2 && ( nextel->report_id == reportid ) ){
	  return nextel;
      } else {
	  nextel = nextel->next;
      }
  }
  return NULL;
//   return curel; // return the previous element
  // is NULL
}

struct hid_device_element * hid_get_next_feature_element( struct hid_device_element * curel ){

  struct hid_device_element * nextel = curel->next;
  while ( nextel != NULL ){
      if ( nextel->io_type == 3 ){
	  return nextel;
      } else {
	  nextel = nextel->next;
      }
  }
  return nextel;
  //return curel; // return the previous element
  // is NULL
}

struct hid_parsing_byte {
    int nextVal;
    int currentSize;
    int bitIndex;
    int remainingBits;
    int shiftedByte;
};

int hid_parse_single_byte( unsigned char current_byte, struct hid_parsing_byte * pbyte ){
  int nextVal;
  unsigned char bitMask;
  unsigned char invBitMask;
  unsigned char maskedByte;
  int currentBitsize = pbyte->currentSize - pbyte->bitIndex;
  if ( currentBitsize >= pbyte->remainingBits ){
      // using the full byte
      nextVal = ( current_byte << pbyte->bitIndex );
      pbyte->bitIndex += pbyte->remainingBits;
      pbyte->remainingBits = 0;
  } else {
      // use a partial byte:
      bitMask = BITMASK1( currentBitsize );
      nextVal = bitMask & current_byte;
      nextVal = nextVal << pbyte->bitIndex;
      pbyte->remainingBits -= currentBitsize;
      // shift the remaining value
      invBitMask = INVERTBITMASK1( bitMask );
      maskedByte = current_byte & invBitMask;
      pbyte->shiftedByte = maskedByte >> currentBitsize;
      pbyte->bitIndex = pbyte->currentSize; // is this always true?
  };
  pbyte->nextVal += nextVal;
  if ( (pbyte->currentSize - pbyte->bitIndex) == 0 ){
    pbyte->bitIndex = 0;
    nextVal = pbyte->nextVal;
    pbyte->nextVal = 0;
    return nextVal;
  }
  return -1;
}

int hid_parse_input_report( unsigned char* buf, int size, struct hid_dev_desc * devdesc ){

#ifdef APPLE
  return hid_parse_input_elements_values( buf, devdesc );
#endif
#ifdef WIN32
  return hid_parse_input_elements_values( buf, size, devdesc );
#endif
#ifdef LINUX_FREEBSD
  struct hid_parsing_byte pbyte;
  pbyte.nextVal = 0;
  pbyte.currentSize = 10;
  pbyte.bitIndex = 0;
  pbyte.remainingBits = 0;
  pbyte.shiftedByte = 0;

  struct hid_device_collection * device_collection = devdesc->device_collection;
  struct hid_device_element * cur_element = device_collection->first_element;
  int newvalue;
  int i = 0;
  int starti = 0;
  int reportid = 0;

  if ( devdesc->number_of_reports > 1 ){
      reportid = (int) buf[i];
      starti = 1;
  }

  if ( cur_element->io_type != 1 || ( cur_element->report_id != reportid ) ){
      cur_element = hid_get_next_input_element_with_reportid(cur_element, reportid );
  }

  for ( i = starti; i < size; i++){
    unsigned char curbyte = buf[i];
    pbyte.remainingBits = 8;
    pbyte.shiftedByte = curbyte;
    while( pbyte.remainingBits > 0 ) {
      // get next element
      pbyte.currentSize = cur_element->report_size;
      newvalue = hid_parse_single_byte( pbyte.shiftedByte, &pbyte );
      if ( newvalue != -1 ){
	if ( devdesc->_element_callback != NULL ){
	  if ( newvalue != cur_element->rawvalue || cur_element->repeat ){
	    hid_element_set_value_from_input( cur_element, newvalue );
	    devdesc->_element_callback( cur_element, devdesc->_element_data );
	  }
	}
	cur_element = hid_get_next_input_element_with_reportid( cur_element, reportid );
      }
    }
  }
  return 0;
#endif
}

void hid_throw_readerror( struct hid_dev_desc * devd ){
  devd->_readerror_callback( devd, devd->_readerror_data );
}

int hid_send_output_report( struct hid_dev_desc * devd, int reportid ){
  char * buf;
  // find the right report id
  int index = 0;
  int i;
  for ( i=0; i<devd->number_of_reports; i++ ){
    if ( reportid == devd->report_ids[i] ){
      index = i;
      break;
    }
  }

  size_t buflength = devd->report_lengths[ index ] / 8;
  #ifdef DEBUG_PARSER
    printf("report id %i, buflength %i\t", reportid, buflength );
#endif

//   if ( reportid != 0 ){
  buflength++; // one more byte if report id is not 0
//   }
  buf = (char *) malloc( sizeof( char ) * buflength );
  memset(buf, 0x0, sizeof(char) * buflength);


  // iterate over elements, find which ones are output elements with the right report id,
  // and set their output values to the buffer


  struct hid_device_collection * device_collection = devd->device_collection;
  struct hid_device_element * cur_element = device_collection->first_element;
  if ( cur_element->io_type != 2 || ( cur_element->report_id != reportid ) ){
      cur_element = hid_get_next_output_element_with_reportid(cur_element, reportid);
  }

#ifdef DEBUG_PARSER
  printf("-----------------------\n");
#endif

  buf[0] = reportid;
  int byte_index = 1;
  int bit_offset = 0;
  int next_val = 0;


  while ( cur_element != NULL && (byte_index < buflength) ){
    int current_output = 0;
    unsigned char current_byte = 0;
    int current_bit_size = cur_element->report_size;
    int current_byte_size = (int) ceil( (float) current_bit_size / 8);
#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i, bitoffset %i, byte_index %i \n", cur_element->report_size, current_byte_size, current_bit_size, bit_offset, byte_index );
    printf("current_output %i \t", current_output );
#endif
    current_output = cur_element->value << bit_offset;
#ifdef DEBUG_PARSER
    printf("current_output shift %i \t", current_output );
#endif
    int i;
    for ( i=0; i<current_byte_size; i++ ){
	current_byte = current_output % 256;
	current_output = current_output >> 8;
	buf[ byte_index + i ] += current_byte;
#ifdef DEBUG_PARSER
    printf("current_output %i, current_byte %i, buf %i\t", current_output, current_byte, buf[ byte_index + i ] );
#endif
    }
    bit_offset += current_bit_size;
    byte_index += (int) floor( (float) current_bit_size / 8 );
    bit_offset = bit_offset%8;
#ifdef DEBUG_PARSER
    printf("bit_offset %i, byte_index %i\n", bit_offset, byte_index );
#endif
    cur_element = hid_get_next_output_element_with_reportid(cur_element, reportid);
  }
#ifdef DEBUG_PARSER
  printf("-----------------------\n");
#endif


  int res = hid_write(devd->device, (const unsigned char*)buf, buflength);

  free( buf );
  return res;
}

int hid_send_output_report_old( struct hid_dev_desc * devd, int reportid ){
  char * buf;
  // find the right report id
  int index = 0;
  int i;
  for ( i=0; i<devd->number_of_reports; i++ ){
    if ( reportid == devd->report_ids[i] ){
      index = i;
      break;
    }
  }

  size_t buflength = devd->report_lengths[ index ] / 8;
  #ifdef DEBUG_PARSER
    printf("report id %i, buflength %i\t", reportid, buflength );
#endif

  if ( reportid != 0 ){
      buflength++; // one more byte if report id is not 0
  }
  buf = (char *) malloc( sizeof( char ) * buflength );
  memset(buf, 0x0, sizeof(char) * buflength);

  // iterate over elements, find which ones are output elements with the right report id,
  // and set their output values to the buffer

  int next_byte_size;
  int next_mod_bit_size;
  int byte_count = 0;
  int next_val = 0;

  struct hid_device_collection * device_collection = devd->device_collection;
  struct hid_device_element * cur_element = device_collection->first_element;
  if ( cur_element->io_type != 2 || ( cur_element->report_id != reportid ) ){
      cur_element = hid_get_next_output_element_with_reportid(cur_element, reportid);
  }
  next_byte_size = cur_element->report_size/8;

#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i \t", cur_element->report_size, next_byte_size, next_mod_bit_size );
#endif

#ifdef DEBUG_PARSER
  printf("-----------------------\n");
#endif

  for ( i = 0; i < buflength; i++){
    unsigned char curbyte = 0;
    if ( cur_element->report_size == 8 ){
      curbyte = (unsigned char) cur_element->value;
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, index %i, value %i, report_size %i, curbyte %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->report_size, curbyte );
#endif
      cur_element = hid_get_next_output_element_with_reportid( cur_element, reportid );
      next_byte_size = cur_element->report_size/8;
    } else if ( cur_element->report_size == 16 ){
      int shift = byte_count*8;
      curbyte = (unsigned char) (cur_element->value >> shift);
      byte_count++;
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, index %i, value %i, report_size %i, curbyte %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->report_size, curbyte );
#endif
      if ( byte_count == next_byte_size ){
	cur_element = hid_get_next_output_element_with_reportid( cur_element, reportid );
	next_byte_size = cur_element->report_size/8;
      }
    } else if ( cur_element->report_size < 8 ){
      int bitindex = 0;
      char curbits = 0;
      // fill up the byte
      while( bitindex < 8 ){
	curbits = cur_element->value & BITMASK1( cur_element->report_size );
	curbits = curbits << bitindex;
	curbyte += curbits;
	bitindex += cur_element->report_size;
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, index %i, value %i, report_size %i, curbyte %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->report_size, curbyte );
#endif
	cur_element = hid_get_next_output_element_with_reportid( cur_element, reportid );
	next_byte_size = cur_element->report_size/8;
      }
    }
    buf[ i ] = curbyte;
  }
#ifdef DEBUG_PARSER
  printf("-----------------------\n");
#endif


  int res = hid_write(devd->device, (const unsigned char*)buf, buflength);

  free( buf );
  return res;
}


struct hid_dev_desc * hid_read_descriptor( hid_device * devd ){
  struct hid_dev_desc * desc;

#ifdef APPLE
  desc = (struct hid_dev_desc *) malloc( sizeof( struct hid_dev_desc ) );
  desc->device = devd;
  hid_parse_element_info( desc );
  return desc;
#endif
#ifdef WIN32
  desc = (struct hid_dev_desc *) malloc( sizeof( struct hid_dev_desc ) );
  desc->device = devd;
  hid_parse_element_info( desc );
  return desc;
#endif
#ifdef LINUX_FREEBSD
  unsigned char descr_buf[HIDAPI_MAX_DESCRIPTOR_SIZE];
  int res;
  res = hid_get_report_descriptor( devd, descr_buf, HIDAPI_MAX_DESCRIPTOR_SIZE );
  if (res < 0){
    printf("Unable to read report descriptor\n");
    return NULL;
  } else {
    desc = (struct hid_dev_desc *) malloc( sizeof( struct hid_dev_desc ) );
    desc->device = devd;
    hid_parse_report_descriptor( descr_buf, res, desc );
    return desc;
  }
#endif
}

struct hid_dev_desc * hid_open_device_path( const char *path, unsigned short vendor, unsigned short product ){
  hid_device * handle = hid_open_path( path );
  if (!handle){
      return NULL;
  }
  struct hid_dev_desc * newdesc = hid_read_descriptor( handle );
  if ( newdesc == NULL ){
    hid_close( handle );
    return NULL;
  }
  struct hid_device_info * newinfo = hid_enumerate(vendor,product);
  //newdesc->device = handle;
  int havenotfound = strcmp(path, newinfo->path) != 0;
  while (havenotfound && (newinfo != NULL) ){
    newinfo = newinfo->next;
    havenotfound = strcmp(path, newinfo->path) != 0;
  }
  if ( newinfo == NULL ){
    hid_close( handle );
    return NULL;
  }

  newdesc->info = newinfo;

  // Set the hid_read() function to be non-blocking.
  hid_set_nonblocking( handle, 1);

  return newdesc;
}

// this one is actually not secure when vendor and product have double entries, so you do not actually know which one you are opening
struct hid_dev_desc * hid_open_device(  unsigned short vendor, unsigned short product, const wchar_t *serial_number ){
  hid_device * handle = hid_open( vendor, product, serial_number );
  if (!handle){
      return NULL;
  }
  struct hid_dev_desc * newdesc = hid_read_descriptor( handle );
  if ( newdesc == NULL ){
    hid_close( handle );
    return NULL;
  }
  struct hid_device_info * newinfo = hid_enumerate(vendor,product);
  //newdesc->device = handle;
  //TODO: if serial_number is given, the info descriptor should also point to that one!
//   int havenotfound = wcscmp(serial_number, newinfo->serial_number) == 0;
//   while (havenotfound && (newinfo != NULL) ){
//     newinfo = newinfo->next;
//     havenotfound = wcscmp(serial_number, newinfo->serial_number) == 0;
//   }
  if ( newinfo == NULL ){
    hid_close( handle );
    return NULL;
  }

  newdesc->info = newinfo;

  // Set the hid_read() function to be non-blocking.
  hid_set_nonblocking( handle, 1);

  return newdesc;
}

void hid_close_device( struct hid_dev_desc * devdesc ){
  hid_close( devdesc->device );
  hid_free_enumeration( devdesc->info );
  hid_free_collection( devdesc->device_collection );
  free( devdesc->report_ids );
  free( devdesc->report_lengths );
//   hid_free_descriptor( devdesc->descriptor );
  //TODO: more memory freeing?
}

void hid_element_set_output_value( struct hid_dev_desc * devdesc, struct hid_device_element * element, int value ){
    element->value = value;
#ifdef APPLE
    hid_send_element_output( devdesc, element );
#endif
#ifdef WIN32
    hid_send_element_output( devdesc, element );
#endif
#ifdef LINUX_FREEBSD
    hid_send_output_report( devdesc, element->report_id );
#endif
}


#ifdef WIN32

void debug_element(struct hid_device_element *element)
{
    printf("index: %d\n", element->index);
    printf("parent_collection: %p\n", element->parent_collection);
    printf("io_type: %d\n", element->io_type);
    printf("usage_page: %d\n", element->usage_page);
    printf("usage_min: %d\n", element->usage_min);
    printf("usage_max: %d\n", element->usage_max);
    printf("usage: %d\n", element->usage);
    printf("type: %d\n", element->type);
    printf("isarray: %d\n", element->isarray);
    printf("isrelative: %d\n", element->isrelative);
    printf("isvariable: %d\n", element->isvariable);
    printf("logical_min: %d\n", element->logical_min);
    printf("logical_max: %d\n", element->logical_max);
    printf("phys_min: %d\n", element->phys_min);
    printf("phys_max: %d\n", element->phys_max);
    printf("unit: %d\n", element->unit);
    printf("unit_exponent: %d\n", element->unit_exponent);
    printf("report_id: %d\n", element->report_id);
    printf("report_size: %d\n", element->report_size);
    printf("report_index: %d\n", element->report_index);
    printf("\n\n");
    fflush(stdout);
}

void debug_collection(struct hid_device_collection *collection, int col_index)
{
    printf("COLLECTION[%d] @%p:\n", col_index, collection);
    printf("index: %d\n", collection->index);
    printf("type: %d\n", collection->type);
    printf("usage_page: %d\n", collection->usage_page);
    printf("usage_index: %d\n", collection->usage_index);
    printf("parent: %p\n", collection->parent_collection);
    printf("children collections: %d\n", collection->num_collections);
    if (collection->first_collection)
        printf("first_collection: %p\n", collection->first_collection);
    if (collection->next_collection)
        printf("next_collection: %p\n", collection->next_collection);
    printf("children elements: %d\n", collection->num_elements);
    if (collection->first_element)
        printf("first_element: %p\n", collection->first_element);
    printf("\n\n");
    fflush(stdout);
}

static void add_element_to_collection(struct hid_device_collection *collection, struct hid_device_element *element)
{
    if (!collection->first_element){
        collection->first_element = element;
    }
    collection->num_elements++;
    element->parent_collection = collection;
}


static struct hid_device_element *duplicate_element_with_new_usage(struct hid_device_element *source_element, int new_usage, struct hid_device_collection *device_collection, int *index)
{
    struct hid_device_element *new_element = hid_new_element();
    memcpy(new_element, source_element, sizeof(struct hid_device_element));
    new_element->index = (*index)++;
    device_collection->num_elements++;
    // Update the collection
    add_element_to_collection(source_element->parent_collection, new_element);
    new_element->usage = new_usage;
    return new_element;
}


static void fill_element_from_button_caps(PHIDP_BUTTON_CAPS pCaps, struct hid_device_element *element)
{
    USHORT bitField = pCaps->BitField;
    element->usage_page = pCaps->UsagePage;
    element->type = bitField & 0xf;
    element->logical_min = 0;
    element->logical_max = 1;
    element->isarray = ((bitField & HID_ITEM_VARIABLE) == 0);
    element->isrelative = pCaps->IsAbsolute ? 0 : 1;
    element->isvariable = ((bitField & HID_ITEM_CONSTANT) == 0);
    element->report_id = pCaps->ReportID;
    element->report_size = pCaps->Range.UsageMax - pCaps->Range.UsageMin + 1;
    element->report_index = 1; // TODO: not sure about this one. The API does not seem to provide this. Perhaps set to 1?

}


static void fill_element_from_value_caps(PHIDP_VALUE_CAPS pCaps, struct hid_device_element *element)
{
    USHORT bitField = pCaps->BitField;
    element->usage_page = pCaps->UsagePage;
    element->type = bitField & 0xf;
    element->isarray = ((bitField & HID_ITEM_VARIABLE) == 0);
    element->isrelative = pCaps->IsAbsolute ? 0 : 1;
    element->isvariable = ((bitField & HID_ITEM_CONSTANT) == 0);
    element->logical_min = pCaps->LogicalMin;
    element->logical_max = pCaps->LogicalMax;
    element->phys_min = pCaps->PhysicalMin;
    element->phys_max = pCaps->PhysicalMax;
    element->unit = pCaps->Units;
    element->unit_exponent = pCaps->UnitsExp;
    element->report_id = pCaps->ReportID;
    element->report_size = pCaps->BitSize;
    element->report_index = pCaps->ReportCount;
}

static int hid_parse_caps(struct hid_device_element **pplast_element, struct hid_device_collection **ppcollections, struct hid_device_collection *pdevice_collection,
    const PHIDP_PREPARSED_DATA pp_data, const PHIDP_CAPS caps, int report_type, BOOL is_button, int *index)
{
    int i, j;
    USHORT numCaps;
    enum _HIDP_REPORT_TYPE api_report_type;
    struct hid_device_element *plast_element = *pplast_element;

    if (is_button){
        switch (report_type){
            case HID_REPORT_TYPE_INPUT: numCaps = caps->NumberInputButtonCaps; api_report_type = HidP_Input; break;
            case HID_REPORT_TYPE_OUTPUT: numCaps = caps->NumberOutputButtonCaps; api_report_type = HidP_Output; break;
            case HID_REPORT_TYPE_FEATURE: numCaps = caps->NumberFeatureButtonCaps; api_report_type = HidP_Feature; break;
        }
        PHIDP_BUTTON_CAPS pButtonCaps;
        pButtonCaps = malloc(numCaps * sizeof(HIDP_BUTTON_CAPS));
        if (HidP_GetButtonCaps(api_report_type, pButtonCaps, &numCaps, pp_data) != HIDP_STATUS_SUCCESS){
            free(pButtonCaps);
            return -1;
        }
        for (i = 0; i < numCaps; i++){
        	// Create a new element
            struct hid_device_element *new_element = hid_new_element();
            if (plast_element){
                plast_element->next = new_element;
            }
            // Connect to previous element
            plast_element = new_element;
            // Add element to collections, and fill element values
            PHIDP_BUTTON_CAPS pCaps = &pButtonCaps[i];
            add_element_to_collection(pdevice_collection, new_element);
            add_element_to_collection(ppcollections[pCaps->LinkCollection], new_element);
            new_element->io_type = report_type;
            new_element->index = (*index)++;
            fill_element_from_button_caps(pCaps, new_element);
            if (pCaps->IsRange){
                // If it is a range, we copy the element, and update the usage. We want to have an element per usage.
                new_element->usage = pCaps->Range.UsageMin;
#ifdef DEBUG_PARSER
                debug_element(new_element);
#endif
                for (j = pCaps->Range.UsageMin + 1; j <= pCaps->Range.UsageMax; j++){
                    new_element = duplicate_element_with_new_usage(plast_element, j, pdevice_collection, index);
                    plast_element->next = new_element;
                    plast_element = new_element;
#ifdef DEBUG_PARSER
                    debug_element(new_element);
#endif
                }
            }
            else{
                new_element->usage = pCaps->NotRange.Usage;
#ifdef DEBUG_PARSER
                debug_element(new_element);
#endif
            }
        }
        free(pButtonCaps);
    }
    else {
        switch (report_type){
            case HID_REPORT_TYPE_INPUT: numCaps = caps->NumberInputValueCaps; api_report_type = HidP_Input; break;
            case HID_REPORT_TYPE_OUTPUT: numCaps = caps->NumberOutputValueCaps; api_report_type = HidP_Output; break;
            case HID_REPORT_TYPE_FEATURE: numCaps = caps->NumberFeatureValueCaps; api_report_type = HidP_Feature; break;
        }
        PHIDP_VALUE_CAPS pValueCaps;
        pValueCaps = malloc(numCaps * sizeof(HIDP_VALUE_CAPS));
        if (HidP_GetValueCaps(api_report_type, pValueCaps, &numCaps, pp_data) != HIDP_STATUS_SUCCESS){
            free(pValueCaps);
            return -1;
        }
        for (i = 0; i < numCaps; i++){
        	// Create a new element
            struct hid_device_element *new_element = hid_new_element();
            if (plast_element){
                plast_element->next = new_element;
            }
            // Connect to previous element
            plast_element = new_element;
            // Add element to collections, and fill element values
            PHIDP_VALUE_CAPS pCaps = &pValueCaps[i];
            add_element_to_collection(pdevice_collection, new_element);
            add_element_to_collection(ppcollections[pCaps->LinkCollection], new_element);
            new_element->io_type = report_type;
            new_element->index = (*index)++;
            fill_element_from_value_caps(pCaps, new_element);

            if (pCaps->IsRange){
                // If it is a range, we copy the element, and update the usage. We want to have an element per usage.
                new_element->usage = pCaps->Range.UsageMin;
                debug_element(new_element);
                for (j = pCaps->Range.UsageMin + 1; j <= pCaps->Range.UsageMax; j++){
                    new_element = duplicate_element_with_new_usage(plast_element, j, pdevice_collection, index);
                    plast_element->next = new_element;
                    plast_element = new_element;
#ifdef DEBUG_PARSER
                    debug_element(new_element);
#endif
                }
            }
            else{
                new_element->usage = pCaps->NotRange.Usage;
#ifdef DEBUG_PARSER
                debug_element(new_element);
#endif
            }
        }
        free(pValueCaps);
    }

    // make sure the last element index is visible outside
    *pplast_element = plast_element;
    return 0;
}



int hid_send_element_output( struct hid_dev_desc * devdesc, struct hid_device_element * element ){
    return 0;
}
int hid_parse_input_elements_values( unsigned char* buf, int size, struct hid_dev_desc * devdesc ){
    // If there is only one report, there will be no report ID in the report. Otherwise, it is the first byte
    int i;
    int report_id = 0;
    int report_length = 0;
    struct hid_device_collection * device_collection = devdesc->device_collection;
    struct hid_device_element * cur_element = device_collection->first_element;
    NTSTATUS res;

    if (devdesc->number_of_reports != 1){
        report_id = buf[0];
    }

    // I thtink on Windows we do not need to calculate the report size here, and we can just get it from the size parameter that we got from above
    for (i = 0; i < devdesc->number_of_reports; i++){
        if (devdesc->report_ids[i] == report_id){
            report_length = size;
        }
    }
    if (report_length == 0){
        // This should not happen, the report was not found
        return -1;
    }

    if (cur_element->io_type != HID_REPORT_TYPE_INPUT){
        cur_element = hid_get_next_input_element(cur_element);
    }

    // The Windows API has a function to get the data of all the buttons that are on, and another to get the value data by component page and usage
    // We get all the buttons that are on
    HANDLE dev_handle = get_device_handle(devdesc->device);
    PHIDP_PREPARSED_DATA pp_data;
    if (!HidD_GetPreparsedData(dev_handle, &pp_data)){
        return -1;
    }
    long usage_and_page_length = HidP_MaxUsageListLength(HidP_Input, 0, pp_data);
    PUSAGE_AND_PAGE usage_and_page_list = malloc(usage_and_page_length * sizeof(USAGE_AND_PAGE));
    memset(usage_and_page_list, 0, usage_and_page_length * sizeof(USAGE_AND_PAGE));

    NTSTATUS ntres = HidP_GetButtonsEx(HidP_Input, 0, usage_and_page_list, &usage_and_page_length, pp_data, buf, report_length);
    if ( ntres != HIDP_STATUS_SUCCESS){
        free(usage_and_page_list);
        return -1;
    }

    while (cur_element != NULL){
        // Check that the element is part of this report
        if (cur_element->report_id != report_id){
            cur_element = hid_get_next_input_element(cur_element);
            continue;
        }

        if (devdesc->_element_callback != NULL){
            // TODO may need to use HidP_GetUsageValueArray, if element->report_index > 1
            unsigned long new_value;
            res = HidP_GetUsageValue(HidP_Input, cur_element->usage_page, 0, cur_element->usage, &new_value, pp_data, buf, report_length);
            if (res == HIDP_STATUS_SUCCESS){
                if (new_value != cur_element->rawvalue || cur_element->repeat){
#ifdef DEBUG_PARSER
                    printf("element page %i, usage %i, index %i, value %i, rawvalue %i, newvalue %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->rawvalue, new_value);
#endif
                    hid_element_set_value_from_input(cur_element, new_value);
                    devdesc->_element_callback(cur_element, devdesc->_element_data);
                }
            }
            else if (res == HIDP_STATUS_USAGE_NOT_FOUND){
                // Then see if we can find the element's page and usage in the buttons that are set to on, if not, it is implicitly 0
                new_value = 0;
                for (i = 0; i < usage_and_page_length; i++){
                    USAGE usage = usage_and_page_list[i].Usage;
                    if (usage_and_page_list[i].UsagePage = cur_element->usage_page && usage == cur_element->usage){
                        new_value = 1;
                        break;
                    }
                }
                if (new_value != cur_element->rawvalue || cur_element->repeat){
                    hid_element_set_value_from_input(cur_element, new_value);
#ifdef DEBUG_PARSER
                    printf("element page %i, usage %i, index %i, value %i, rawvalue %i, newvalue %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->rawvalue, new_value);
#endif
                    devdesc->_element_callback(cur_element, devdesc->_element_data);
                }
            }
        }
        cur_element = hid_get_next_input_element(cur_element);
    }
    free(usage_and_page_list);

    return 0;
}
void hid_parse_element_info( struct hid_dev_desc * devdesc ){

    int i, j;
    hid_device * dev = devdesc->device;

    struct hid_device_collection * device_collection = hid_new_collection();
    devdesc->device_collection = device_collection;

    struct hid_device_collection * parent_collection = devdesc->device_collection;

    device_collection->num_collections = 0;
    device_collection->num_elements = 0;

    int numreports = 1;
    int report_lengths[256];
    int report_ids[256];
    report_ids[0] = 0;
    report_lengths[0] = 0;

    // To keep track of indices
    int new_index = 0;

    int numColls = 0;

    NTSTATUS nt_res;
    PHIDP_PREPARSED_DATA pp_data = NULL;
    HIDP_CAPS caps;

    /* Open a handle to the device */
    HANDLE dev_handle = get_device_handle(dev);

    /* Check validity of write_handle. */
    if (dev_handle == INVALID_HANDLE_VALUE) {
        /* Unable to open the device. */
        //register_error(dev, "CreateFile");
        // TODO: not sure what to do here
        return;
    }

    /* Get the Usage Page and Usage for this device. */
    BOOLEAN res = HidD_GetPreparsedData(dev_handle, &pp_data);
    if (!res){
        // TODO: what should we do here?
        return;
    }

    nt_res = HidP_GetCaps(pp_data, &caps);
    if (nt_res != HIDP_STATUS_SUCCESS) {
        // TODO: what should we do here
        return;
    }
    numColls = caps.NumberLinkCollectionNodes;

    device_collection->num_collections = numColls;
    device_collection->usage_page = caps.UsagePage;
    device_collection->usage_index = caps.Usage;

    device_collection->index = new_index++;
    device_collection->num_elements = 0;

    PHIDP_LINK_COLLECTION_NODE linkCollectionNodes;
    linkCollectionNodes = malloc(numColls * sizeof(HIDP_LINK_COLLECTION_NODE));
    struct hid_device_collection **collections = malloc(numColls * sizeof(struct hid_device_collection *));
    nt_res = HidP_GetLinkCollectionNodes( linkCollectionNodes, &numColls, pp_data );
    if (nt_res != HIDP_STATUS_SUCCESS){
        // TODO: what to do here?
        return;
    }

    // Create the N collections, and put them in an array, so that we can efficiently construct the pointers to each other using the Windows API
    for (i = 0; i < numColls; i++){
        collections[i] = hid_new_collection();
    }

    // And now fill the data and create the collection links
    for (i = 0; i < numColls; i++){
        PHIDP_LINK_COLLECTION_NODE p_collection = &linkCollectionNodes[i];
        // Documentation says that if parent is 0, then there is no parent, but then how do you indicate that the parent is at index 0???
        // I think that is wrong, and 0 indicates that the parent is the collection at index
        // I will assume that the collection at index 0 is always the outermost one
        if (i == 0){
            device_collection->first_collection = collections[i];
            collections[i]->parent_collection = device_collection;
        }
        else{
            collections[i]->parent_collection = collections[p_collection->Parent];
        }
        if (p_collection->NextSibling)
            collections[i]->next_collection = collections[p_collection->NextSibling];

        if (p_collection->FirstChild)
            collections[i]->first_collection = collections[p_collection->FirstChild];

        collections[i]->num_collections = p_collection->NumberOfChildren;
        collections[i]->type = p_collection->CollectionType;
        collections[i]->usage_page = p_collection->LinkUsagePage;
        collections[i]->usage_index = p_collection->LinkUsage;
        collections[i]->index = new_index++; // TODO not sure about this one
#ifdef DEBUG_PARSER
        debug_collection(collections[i], i);
#endif
    }
#ifdef DEBUG_PARSER
    debug_collection(device_collection, 100);
#endif

    /* Now, create the elements, parsing the (input, output, feature) x (button, values) capabilities */
    int index_element = 0;
    new_index = 0;
    struct hid_device_element *last_element = NULL;
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_INPUT, FALSE, &new_index);
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_INPUT, TRUE, &new_index);
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_OUTPUT, FALSE, &new_index);
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_OUTPUT, TRUE, &new_index);
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_FEATURE, FALSE, &new_index);
    hid_parse_caps(&last_element, collections, device_collection, pp_data, &caps, HID_REPORT_TYPE_FEATURE, TRUE, &new_index);

    /* Reports */
    // Perhaps look at this (from MSDN):  The XxxReportByteLength members of a HID collection's HIDP_CAPS structure specify the required size of input, output, and feature reports
    // On Windows it is not clear that knowing the report size is important at this point, since we get it implicitly on the read size. It's quite likely that the calculation done here
    // is not right, but it does not affect (should revisit later)
    struct hid_device_element *element = device_collection->first_element;
    while (element != NULL){
        int report_id = element->report_id;
        int report_size = element->report_size;
        int report_count = element->report_index; // TODO: report_count is not used????

        int reportexists = 0;
        for (j = 0; j < numreports; j++){
            reportexists = (report_ids[j] == report_id);
        }
        if (!reportexists){
            report_ids[numreports] = report_id;
            report_lengths[numreports] = 0;
            numreports++;
        }
        int k = 0;
        int index = 0;
        for (k = 0; k<numreports; k++){
            if (report_id == report_ids[k]){
                index = k;
                break;
            }
        }
        report_lengths[index] += report_size;
        element = element->next;
    }

    devdesc->number_of_reports = numreports;
    devdesc->report_lengths = (int*)malloc(sizeof(int) * numreports);
    devdesc->report_ids = (int*)malloc(sizeof(int) * numreports);
    for (j = 0; j<numreports; j++){
        devdesc->report_lengths[j] = report_lengths[j];
        devdesc->report_ids[j] = report_ids[j];
    }

    /* And clean up the temp structures */
    free(collections);
    free(linkCollectionNodes);
    HidD_FreePreparsedData(pp_data);
}
#endif


#ifdef APPLE

#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>

// from: https://developer.apple.com/library/mac/documentation/devicedrivers/conceptual/HID/new_api_10_5/tn2187.html#//apple_ref/doc/uid/TP40000970-CH214-SW2

int hid_send_element_output( struct hid_dev_desc * devdesc, struct hid_device_element * element ){
  IOReturn  tIOReturn;
  IOHIDDeviceRef device_handle = get_device_handle( devdesc->device );
  IOHIDElementRef tIOHIDElementRef = element->appleIOHIDElementRef;

  // get the logical mix/max for this LED element
  CFIndex minCFIndex = IOHIDElementGetLogicalMin( tIOHIDElementRef );
  CFIndex maxCFIndex = IOHIDElementGetLogicalMax( tIOHIDElementRef );
  // calculate the range
  CFIndex modCFIndex = maxCFIndex - minCFIndex + 1;
  // compute the value for this LED element
  CFIndex tCFIndex = minCFIndex + ( element->value % modCFIndex );

  uint64_t timestamp = 0; // create the IO HID Value to be sent

  IOHIDValueRef tIOHIDValueRef = IOHIDValueCreateWithIntegerValue( kCFAllocatorDefault, tIOHIDElementRef, timestamp, tCFIndex );

  if ( tIOHIDValueRef ) {
    tIOReturn = IOHIDDeviceSetValue( device_handle, tIOHIDElementRef, tIOHIDValueRef );
    CFRelease( tIOHIDValueRef );
    if ( tIOReturn == kIOReturnSuccess ){
      return 0;
    }
  }
  return -1;
}

// int hid_parse_input_report( unsigned char* buf, int size, struct hid_dev_desc * devdesc ){
int hid_parse_input_elements_values( unsigned char* buf, struct hid_dev_desc * devdesc ){
  struct hid_device_collection * device_collection = devdesc->device_collection;
  struct hid_device_element * cur_element = device_collection->first_element;
  int i=0;
  int newvalue;
  int reportid = 0;

  IOHIDDeviceRef device_handle = get_device_handle( devdesc->device );
  IOHIDValueRef newValueRef;
  IOReturn  tIOReturn;

  /*
  if ( devdesc->number_of_reports > 1 ){
      reportid = (int) buf[i];
  }

  if ( cur_element->io_type != 1 || ( cur_element->report_id != reportid ) ){
      cur_element = hid_get_next_input_element_with_reportid(cur_element, reportid );
  }
  */
//  printf( "======== start of report: cur_element %i\n", cur_element );
  if ( cur_element->io_type != 1 ){
        cur_element = hid_get_next_input_element(cur_element);
  }
  //printf( "cur_element %i", cur_element );

  while( cur_element != NULL ){
	if ( devdesc->_element_callback != NULL ){
	  tIOReturn = IOHIDDeviceGetValue( device_handle, cur_element->appleIOHIDElementRef, &newValueRef );
	 // printf("element page %i, usage %i, index %i, value %i, rawvalue %i, newvalueref %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->rawvalue, newValueRef );
	  if ( tIOReturn == kIOReturnSuccess ){
	    newvalue = IOHIDValueGetIntegerValue( newValueRef );
	    if ( newvalue != cur_element->rawvalue || cur_element->repeat ){
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, index %i, value %i, rawvalue %i, newvalue %i\n", cur_element->usage_page, cur_element->usage, cur_element->index, cur_element->value, cur_element->rawvalue, newvalue );
#endif
	      hid_element_set_value_from_input( cur_element, newvalue );
	      devdesc->_element_callback( cur_element, devdesc->_element_data );
	    }
	  }
	}
//	cur_element = hid_get_next_input_element_with_reportid( cur_element, reportid );
	cur_element = hid_get_next_input_element( cur_element );
//	printf( "cur_element %i\n", cur_element );
  }
//  printf( "======== end of report\n");
  return 0;
}

/*
IOReturn  tIOReturn = IOHIDDeviceGetValue(
                            deviceRef,  // IOHIDDeviceRef for the HID device
                            elementRef, // IOHIDElementRef for the HID element
                            valueRef);  // IOHIDValueRef for the HID element's new value
*/

void hid_parse_element_info( struct hid_dev_desc * devdesc )
{
  hid_device * dev = devdesc->device;

  struct hid_device_collection * device_collection = hid_new_collection();
  devdesc->device_collection = device_collection;

  struct hid_device_collection * parent_collection = devdesc->device_collection;
  struct hid_device_collection * prev_collection;
  struct hid_device_element * prev_element;

  device_collection->num_collections = 0;
  device_collection->num_elements = 0;

  int numreports = 1;
  int report_lengths[256];
  int report_ids[256];
  report_ids[0] = 0;
  report_lengths[0] = 0;

  IOHIDDeviceRef device_handle = get_device_handle( dev );
  CFArrayRef elementCFArrayRef = IOHIDDeviceCopyMatchingElements(device_handle,
      NULL /* matchingCFDictRef */
      ,  kIOHIDOptionsTypeNone);
  if (elementCFArrayRef) {
    // iterate over all the elements
    CFIndex elementIndex, elementCount = CFArrayGetCount(elementCFArrayRef);
    for (elementIndex = 0; elementIndex < elementCount; elementIndex++) {
      IOHIDElementRef tIOHIDElementRef = (IOHIDElementRef)CFArrayGetValueAtIndex(elementCFArrayRef,elementIndex);
      if (tIOHIDElementRef) {
	  IOHIDElementType tIOHIDElementType = IOHIDElementGetType(tIOHIDElementRef);
	  uint32_t usagePage = IOHIDElementGetUsagePage(tIOHIDElementRef);
	  uint32_t usage     = IOHIDElementGetUsage(tIOHIDElementRef);

	  if ( tIOHIDElementType == kIOHIDElementTypeCollection ){
	      //TODO: COULD ALSO READ WHICH KIND OF COLLECTION
	      struct hid_device_collection * new_collection = hid_new_collection();
	      if ( parent_collection->num_collections == 0 ){
	    	  parent_collection->first_collection = new_collection;
	      }
	      if ( device_collection->num_collections == 0 ){
	    	  device_collection->first_collection = new_collection;
	      } else {
	    	  prev_collection->next_collection = new_collection;
	      }
	      new_collection->parent_collection = parent_collection;
	      IOHIDElementCollectionType tIOHIDElementCollectionType = IOHIDElementGetCollectionType(tIOHIDElementRef);
	      new_collection->type = (int) tIOHIDElementCollectionType;
	      new_collection->usage_page = usagePage;
	      new_collection->usage_index = usage;
	      new_collection->index = device_collection->num_collections;
	      device_collection->num_collections++;
	      if ( device_collection != parent_collection ){
	    	  parent_collection->num_collections++;
	      }
	      parent_collection = new_collection;
	      prev_collection = new_collection;
//  	      collection_nesting++;
	  } else {
	      struct hid_device_element * new_element = hid_new_element();
	      new_element->appleIOHIDElementRef = tIOHIDElementRef; // set the element ref
	      new_element->index = device_collection->num_elements;
	      // check input (1), output (2), or feature (3)
	      // type - this we parse later on again, so perhaps would be good to bittest this rightaway in general
// 		["Data","Constant"],
          Boolean isVirtual = IOHIDElementIsVirtual(tIOHIDElementRef);
//                 ["Array","Variable"]
          Boolean isArray = IOHIDElementIsArray(tIOHIDElementRef);
//                 ["Absolute","Relative"]
          Boolean isRelative = IOHIDElementIsRelative(tIOHIDElementRef);
//                 ["NoWrap","Wrap"],
          Boolean isWrapping = IOHIDElementIsWrapping(tIOHIDElementRef);
//                 ["Linear","NonLinear"],
          Boolean isNonLinear = IOHIDElementIsNonLinear(tIOHIDElementRef);
//                 ["PreferredState","NoPreferred"],
          Boolean hasPreferredState = IOHIDElementHasPreferredState(tIOHIDElementRef);
//                 ["NoNullPosition", "NullState"],
          Boolean hasNullState = IOHIDElementHasNullState(tIOHIDElementRef);
	      int type = 0;
	      new_element->type = 0;
	      type = (int) isVirtual;
	      new_element->isvariable = (int) !isVirtual;
	      new_element->type += type;
	      new_element->isarray = (int) isArray;
	      type = ((int) !isArray) << 1;
	      new_element->type += type;
	      new_element->isrelative = (int) isRelative;
	      type = ((int) isRelative) << 2;
	      new_element->type += type;
	      type = ((int) isWrapping) << 3;
	      new_element->type += type;
	      type = ((int) isNonLinear) << 4;
	      new_element->type += type;
	      type = ((int) !hasPreferredState) << 5;
	      new_element->type += type;
	      type = ((int) hasNullState) << 6;
	      new_element->type += type;
	      switch (tIOHIDElementType) {
		case kIOHIDElementTypeInput_Misc:
		{
		  new_element->io_type = 1;
// 		  printf("type: Misc, ");
		  break;
		}
		case kIOHIDElementTypeInput_Button:
		{
		  new_element->io_type = 1;
// 		  printf("type: Button, ");
		  break;
		}
		case kIOHIDElementTypeInput_Axis:
		{
		  new_element->io_type = 1;
// 		  printf("type: Axis, ");
		  break;
		}
		case kIOHIDElementTypeInput_ScanCodes:
		{
		  new_element->io_type = 1;
// 		  printf("type: ScanCodes, ");
		  break;
		}
		case kIOHIDElementTypeOutput:
		{
		  new_element->io_type = 2;
// 		  printf("type: Output, ");
		  break;
		}
		case kIOHIDElementTypeFeature:
		{
		  new_element->io_type = 3;
// 		  printf("type: Feature, ");
		  break;
		}
	      }
	      new_element->parent_collection = parent_collection;
	      new_element->usage_page = usagePage;
	      new_element->usage = usage;
	      CFIndex logicalMin = IOHIDElementGetLogicalMin(tIOHIDElementRef);
	      CFIndex logicalMax = IOHIDElementGetLogicalMax(tIOHIDElementRef);
	      new_element->logical_min = logicalMin;
	      new_element->logical_max = logicalMax;
	      CFIndex physicalMin = IOHIDElementGetPhysicalMin(tIOHIDElementRef);
	      CFIndex physicalMax = IOHIDElementGetPhysicalMax(tIOHIDElementRef);
	      new_element->phys_min = physicalMin;
	      new_element->phys_max = physicalMax;
	      uint32_t unit    = IOHIDElementGetUnit(tIOHIDElementRef);
	      uint32_t unitExp = IOHIDElementGetUnitExponent(tIOHIDElementRef);
	      new_element->unit = unit;
	      new_element->unit_exponent = unitExp;
	      uint32_t reportID    = IOHIDElementGetReportID(tIOHIDElementRef);
	      uint32_t reportSize  = IOHIDElementGetReportSize(tIOHIDElementRef);
	      uint32_t reportCount = IOHIDElementGetReportCount(tIOHIDElementRef);
	      new_element->report_size = reportSize;
	      new_element->report_id = reportID;
	      new_element->report_index = reportCount; // ?? - was j... index
	      new_element->value = 0;
	      new_element->array_value = 0;

	      int reportexists = 0;
	      for ( int j = 0; j < numreports; j++ ){
	    	  reportexists = (report_ids[j] == reportID);
	      }
	      if ( !reportexists ){
	      	report_ids[ numreports ] = reportID;
	      	report_lengths[ numreports ] = 0;
	      	numreports++;
	      }
	      int k = 0;
	      int index = 0;
	      for ( k=0; k<numreports; k++ ){
	      	if ( reportID == report_ids[k] ){
	      		index = k;
	      		break;
	      	}
	      }
	      report_lengths[index] += reportSize;

	      if ( parent_collection->num_elements == 0 ){
	    	  parent_collection->first_element = new_element;
	      }
	      if ( device_collection->num_elements == 0 ){
	    	  device_collection->first_element = new_element;
	      } else {
	    	  prev_element->next = new_element;
	      }
	      device_collection->num_elements++;
	      if ( parent_collection != device_collection ) {
	    	  parent_collection->num_elements++;
	      }
	      prev_element = new_element;
	  }
	}
    }
    CFRelease(elementCFArrayRef);
  }
  devdesc->number_of_reports = numreports;
  devdesc->report_lengths = (int*) malloc( sizeof( int ) * numreports );
  devdesc->report_ids = (int*) malloc( sizeof( int ) * numreports );
  for ( int j = 0; j<numreports; j++ ){
	  devdesc->report_lengths[j] = report_lengths[j];
	  devdesc->report_ids[j] = report_ids[j];
  }
  //return 0;
}

#endif
