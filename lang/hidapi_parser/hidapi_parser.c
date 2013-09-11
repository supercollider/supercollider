#include <stdio.h>
#include <stdlib.h>
// #include <math.h>

#include "hidapi_parser.h"

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


void hid_descriptor_init( struct hid_device_descriptor * devd){
  hid_set_descriptor_callback(devd, NULL, NULL);
  hid_set_element_callback(devd, NULL, NULL);
}

void hid_free_descriptor( struct hid_device_descriptor * devd){
  free( devd );
//   hid_set_descriptor_callback(devd, NULL, NULL);
//   hid_set_element_callback(devd, NULL, NULL);
}

void hid_set_descriptor_callback( struct hid_device_descriptor * devd, hid_descriptor_callback cb, void *user_data ){
    devd->_descriptor_callback = cb;
    devd->_descriptor_data = user_data;
}

void hid_set_element_callback( struct hid_device_descriptor * devd, hid_element_callback cb, void *user_data ){
    devd->_element_callback = cb;
    devd->_element_data = user_data;
}

int hid_parse_report_descriptor( char* descr_buf, int size, struct hid_device_descriptor * descriptor ){
  struct hid_device_element * prev_element;
  int current_usage_page;
  int current_usage;
  int current_usages[256];
  int current_usage_index = 0;
  int current_usage_min = -1;
  int current_usage_max = -1;
  int current_logical_min = 0;
  int current_logical_max = 0;
  int current_physical_min = 0;
  int current_physical_max = 0;
  int current_report_count;
  int current_report_id;
  int current_report_size;
  int current_unit = 0;
  int current_unit_exponent = 0;
  char current_input;
  char current_output;
  int collection_nesting = 0;
  
  int next_byte_tag = -1;
  int next_byte_size = 0;
  int next_byte_type = 0;
  int next_val = 0;
  
  unsigned char toadd = 0;
  int byte_count = 0;
  
  int i,j;
  
  descriptor->num_elements = 0;
  for ( i = 0; i < size; i++){
#ifdef DEBUG_PARSER
	  printf("\n%02hhx ", descr_buf[i]);
	  printf("\tbyte_type %i, %i, %i \t", next_byte_tag, next_byte_size, next_val);
#endif
	  if ( next_byte_tag != -1 ){
// 	      toadd = (unsigned char) descr_buf[i];
	      int shift = byte_count*8;
	      next_val |= (int)(((unsigned char)(descr_buf[i])) << shift);
#ifdef DEBUG_PARSER
	      printf("\t nextval shift: %i", next_val);
#endif
	      byte_count++;
	      if ( byte_count == next_byte_size ){
		switch( next_byte_tag ){
		  case HID_USAGE_PAGE:
		    current_usage_page = next_val;
#ifdef DEBUG_PARSER
		    printf("usage page: 0x%02hhx", current_usage_page);
#endif
		    break;
		  case HID_USAGE:
		    current_usage = next_val;
		    current_usages[ current_usage_index ] = next_val;
#ifdef DEBUG_PARSER
		    printf("usage: 0x%02hhx, %i", current_usages[ current_usage_index ], current_usage_index );
#endif
		    current_usage_index++;
		    break;
		  case HID_COLLECTION:
		    //TODO: COULD ALSO READ WHICH KIND OF COLLECTION
		    collection_nesting++;
#ifdef DEBUG_PARSER
		    printf("collection: %i, %i", collection_nesting, next_val );
#endif
		    break;
		  case HID_USAGE_MIN:
		    current_usage_min = next_val;
#ifdef DEBUG_PARSER
		    printf("usage min: %i", current_usage_min);
#endif
		    break;
		  case HID_USAGE_MAX:
		    current_usage_max = next_val;
#ifdef DEBUG_PARSER
		    printf("usage max: %i", current_usage_max);
#endif
		    break;
		  case HID_LOGICAL_MIN:
		    current_logical_min = next_val;
#ifdef DEBUG_PARSER
		    printf("logical min: %i", current_logical_min);
#endif
		    break;
		  case HID_LOGICAL_MAX:
		    current_logical_max = next_val;
#ifdef DEBUG_PARSER
		    printf("logical max: %i", current_logical_max);
#endif
		    break;
		  case HID_PHYSICAL_MIN:
		    current_physical_min = next_val;
#ifdef DEBUG_PARSER
		    printf("physical min: %i", current_physical_min);
#endif
		    break;
		  case HID_PHYSICAL_MAX:
		    current_physical_max = next_val;
#ifdef DEBUG_PARSER
		    printf("physical max: %i", current_physical_min);
#endif
		    break;
		  case HID_REPORT_COUNT:
		    current_report_count = next_val;
#ifdef DEBUG_PARSER
		    printf("report count: %i", current_report_count);
#endif
		    break;
		  case HID_REPORT_SIZE:
		    current_report_size = next_val;
#ifdef DEBUG_PARSER
		    printf("report size: %i", current_report_size);
#endif
		    break;
		  case HID_REPORT_ID:
		    current_report_id = next_val;
#ifdef DEBUG_PARSER
		    printf("report id: %i", current_report_id);
#endif
		    break;
		  case HID_POP:
		    // TODO: something useful with pop
#ifdef DEBUG_PARSER
		    printf("pop: %i", next_val );
#endif
		    break;
		  case HID_PUSH:
		    // TODO: something useful with push
#ifdef DEBUG_PARSER
		    printf("pop: %i", next_val );
#endif
		    break;
		  case HID_UNIT:
		    current_unit = next_val;
#ifdef DEBUG_PARSER
		    printf("unit: %i", next_val );
#endif
		    break;
		  case HID_UNIT_EXPONENT:
		    current_unit_exponent = next_val;
#ifdef DEBUG_PARSER
		    printf("unit exponent: %i", next_val );
#endif
		    break;
		  case HID_INPUT:
#ifdef DEBUG_PARSER
		    printf("input: %i", next_val);
#endif
		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
			new_element->index = descriptor->num_elements;
			new_element->io_type = 1;
			new_element->type = next_val; //TODO: parse this for more detailed info

			new_element->usage_page = current_usage_page;
			if ( current_usage_min != -1 ){
			  new_element->usage = current_usage_min + j;
			} else {
			  new_element->usage = current_usages[j];
			}
			new_element->logical_min = current_logical_min;
			new_element->logical_max = current_logical_max;
			if ( (current_physical_min == 0) && (current_physical_max == 0) ){
			  new_element->phys_min = current_logical_min;
			  new_element->phys_max = current_logical_max;
			
			} else {
			  new_element->phys_min = current_physical_min;
			  new_element->phys_max = current_physical_max;
			}
			new_element->unit = current_unit;
			new_element->unit_exponent = current_unit_exponent;
			
			new_element->report_size = current_report_size;
			new_element->report_id = current_report_id;
			new_element->report_index = j;
			
			new_element->value = 0;
			if ( descriptor->num_elements == 0 ){
			    descriptor->first = new_element;
			} else {
			    prev_element->next = new_element;
			}
			descriptor->num_elements++;
			prev_element = new_element;
		    }
// 		    current_usage_min = -1;
// 		    current_usage_max = -1;
// 		    current_usage_index = 0;
// 		    current_physical_min = 0;
// 		    current_physical_max = 0;
// 		    current_unit_exponent = 0;
		    break;
		  case HID_OUTPUT:
#ifdef DEBUG_PARSER
		    printf("output: %i", next_val);
#endif
		    		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
			new_element->index = descriptor->num_elements;
			new_element->io_type = 2;
			new_element->type = next_val; //TODO: parse this for more detailed info

			new_element->usage_page = current_usage_page;
			if ( current_usage_min != -1 ){
			  new_element->usage = current_usage_min + j;
			} else {
			  new_element->usage = current_usages[j];
			}
			new_element->logical_min = current_logical_min;
			new_element->logical_max = current_logical_max;
			if ( (current_physical_min == 0) && (current_physical_max == 0) ){
			  new_element->phys_min = current_logical_min;
			  new_element->phys_max = current_logical_max;
			
			} else {
			  new_element->phys_min = current_physical_min;
			  new_element->phys_max = current_physical_max;
			}
			new_element->unit = current_unit;
			new_element->unit_exponent = current_unit_exponent;
			
			new_element->report_size = current_report_size;
			new_element->report_id = current_report_id;
			new_element->report_index = j;
			
			new_element->value = 0;
			if ( descriptor->num_elements == 0 ){
			    descriptor->first = new_element;
			} else {
			    prev_element->next = new_element;
			}
			descriptor->num_elements++;
			prev_element = new_element;
		    }
// 		    current_usage_min = -1;
// 		    current_usage_max = -1;
// 		    current_usage_index = 0;
// 		    current_physical_min = 0;
// 		    current_physical_max = 0;
// 		    current_unit_exponent = 0;
		    break;
		  case HID_FEATURE:
#ifdef DEBUG_PARSER
		    printf("feature: %i", next_val);
#endif
		    // add the elements for this report
		    for ( j=0; j<current_report_count; j++ ){
			struct hid_device_element * new_element = (struct hid_device_element *) malloc( sizeof( struct hid_device_element ) );
			new_element->index = descriptor->num_elements;
			new_element->io_type = 3;
			new_element->type = next_val; //TODO: parse this for more detailed info

			new_element->usage_page = current_usage_page;
			if ( current_usage_min != -1 ){
			  new_element->usage = current_usage_min + j;
			} else {
			  new_element->usage = current_usages[j];
			}
			new_element->logical_min = current_logical_min;
			new_element->logical_max = current_logical_max;
			if ( (current_physical_min == 0) && (current_physical_max == 0) ){
			  new_element->phys_min = current_logical_min;
			  new_element->phys_max = current_logical_max;
			
			} else {
			  new_element->phys_min = current_physical_min;
			  new_element->phys_max = current_physical_max;
			}
			new_element->unit = current_unit;
			new_element->unit_exponent = current_unit_exponent;
			
			new_element->report_size = current_report_size;
			new_element->report_id = current_report_id;
			new_element->report_index = j;
			
			new_element->value = 0;
			if ( descriptor->num_elements == 0 ){
			    descriptor->first = new_element;
			} else {
			    prev_element->next = new_element;
			}
			descriptor->num_elements++;
			prev_element = new_element;
		    }
// 		    current_usage_min = -1;
// 		    current_usage_max = -1;
// 		    current_usage_index = 0;
// 		    current_physical_min = 0;
// 		    current_physical_max = 0;
// 		    current_unit_exponent = 0;
		    break;
#ifdef DEBUG_PARSER
		  default:
		    if ( next_byte_tag >= HID_RESERVED ){
		      printf("reserved bytes 0x%02hhx, %i", next_byte_tag, next_val );
		    } else {
		      printf("undefined byte type 0x%02hhx, %i", next_byte_tag, next_val );
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
	      collection_nesting--;
#ifdef DEBUG_PARSER
	      printf("\tend collection: %i, %i\n", collection_nesting, descr_buf[i] );
#endif
	    } else {
	      byte_count = 0;
	      next_val = 0;
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
  return 0;
}

float hid_element_map_logical( struct hid_device_element * element ){
  float result = element->logical_min + ( element->value/( element->logical_max - element->logical_min ) );
  return result;
}

float hid_element_resolution( struct hid_device_element * element ){
    float result = 0;
//     ( element->logical_max - element->logical_min) / ( ( element->phys_max - element->phys_min) * pow(10, element->unit_exponent) );
    return result;
}

float hid_element_map_physical( struct hid_device_element * element ){
  float result = 0;
  return result;
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
  return curel; // return the previous element
  // is NULL
}

int hid_parse_input_report( unsigned char* buf, int size, struct hid_device_descriptor * descriptor ){
  ///TODO: parse input from descriptors with report size like 12 correctly
  
  // Print out the returned buffer.
  struct hid_device_element * cur_element = descriptor->first;
  int i;
  int next_byte_size;
  int next_mod_bit_size;
  int byte_count = 0;
  int next_val = 0;

//   cur_element = hid_get_next_input_element( cur_element );
//   if ( cur_element == NULL ){ return 0; }
  if ( cur_element->io_type != 1 ){
      cur_element = hid_get_next_input_element(cur_element);
  }
  next_byte_size = cur_element->report_size/8;
  next_mod_bit_size = cur_element->report_size%8;

#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i \t", cur_element->report_size, next_byte_size, next_mod_bit_size );
#endif    
  
#ifdef DEBUG_PARSER
  printf("-----------------------\n");
#endif
  for ( i = 0; i < size; i++){
    unsigned char curbyte = buf[i];
#ifdef DEBUG_PARSER    
    printf("byte %02hhx \t", buf[i]);
#endif
    // read byte:    
    if ( cur_element->report_size < 8 ){
      int bitindex = 0;
      while ( bitindex < 8 ){
	// read bit
	cur_element->value = (curbyte >> bitindex) & BITMASK1( cur_element->report_size );
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, type %i, index %i, value %i\n", cur_element->usage_page, cur_element->usage, cur_element->type, cur_element->index, cur_element->value );
#endif
	bitindex += cur_element->report_size;
	if ( descriptor->_element_callback != NULL ){
	  descriptor->_element_callback( cur_element, descriptor->_element_data );
	}
	cur_element = hid_get_next_input_element( cur_element );
// 	if ( cur_element == NULL ){ return 0; }
	next_byte_size = cur_element->report_size/8;
	next_mod_bit_size = cur_element->report_size%8;

#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i \t", cur_element->report_size, next_byte_size, next_mod_bit_size );
#endif    
      }
    } else if ( cur_element->report_size == 8 ){
	cur_element->value = curbyte;
#ifdef DEBUG_PARSER
	printf("element page %i, usage %i, type %i,  index %i, value %i\n", cur_element->usage_page, cur_element->usage, cur_element->type, cur_element->index,cur_element->value );
#endif
	if ( descriptor->_element_callback != NULL ){
	  descriptor->_element_callback( cur_element, descriptor->_element_data );
	}
	cur_element = hid_get_next_input_element( cur_element );
// 	if ( cur_element == NULL ){ return 0; }
	next_byte_size = cur_element->report_size/8;
	next_mod_bit_size = cur_element->report_size%8;
	
#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i \t", cur_element->report_size, next_byte_size, next_mod_bit_size );
#endif    
    } else if ( cur_element->report_size == 16 ){
      int shift = byte_count*8;
      next_val |= (int)(((unsigned char)(curbyte)) << shift);
#ifdef DEBUG_PARSER
      printf("\t nextval shift: %i", next_val);
#endif
     byte_count++;
      if ( byte_count == next_byte_size ){
	  cur_element->value = next_val;
	  
#ifdef DEBUG_PARSER
	  printf("element page %i, usage %i, type %i,  index %i, value %i\n", cur_element->usage_page, cur_element->usage, cur_element->type, cur_element->index,cur_element->value );
#endif
	  if ( descriptor->_element_callback != NULL ){
	    descriptor->_element_callback( cur_element, descriptor->_element_data );
	  }
	  cur_element = hid_get_next_input_element( cur_element );
// 	  if ( cur_element == NULL ){ break; }
	  next_byte_size = cur_element->report_size/8;
	  next_mod_bit_size = cur_element->report_size%8;
	 
#ifdef DEBUG_PARSER
    printf("report_size %i, bytesize %i, bitsize %i \t", cur_element->report_size, next_byte_size, next_mod_bit_size );
#endif    
	  byte_count = 0;
	  next_val = 0;
      }
    }
  }
  
#ifdef DEBUG_PARSER  
  printf("\n");
#endif
  if ( descriptor->_descriptor_callback != NULL ){
    descriptor->_descriptor_callback( descriptor, descriptor->_descriptor_data );
  }
  return 0;
}



struct hid_device_descriptor * hid_read_descriptor( hid_device * devd ){
  struct hid_device_descriptor * descriptor;
  unsigned char descr_buf[HIDAPI_MAX_DESCRIPTOR_SIZE];
  int res;
  res = hid_get_report_descriptor( devd, descr_buf, HIDAPI_MAX_DESCRIPTOR_SIZE );
  if (res < 0){
    printf("Unable to read report descriptor\n");
    return NULL;
  } else {
    descriptor = (struct hid_device_descriptor *) malloc( sizeof( struct hid_device_descriptor) );
    hid_descriptor_init( descriptor );
    hid_parse_report_descriptor( descr_buf, res, descriptor );
    return descriptor;
  }
}

struct hid_dev_desc * hid_open_device(  unsigned short vendor, unsigned short product, const wchar_t *serial_number ){
  hid_device * handle = hid_open( vendor, product, serial_number );
  if (!handle){
      return NULL;
  }  
  struct hid_device_descriptor * newdesc = hid_read_descriptor( handle );
  if ( newdesc == NULL ){
    hid_close( handle );
    return NULL;
  }
  struct hid_dev_desc * newdevdesc = (struct hid_dev_desc *) malloc( sizeof( struct hid_dev_desc ) );
  struct hid_device_info * newinfo = hid_enumerate(vendor,product);
  newdevdesc->device = handle;
  //TODO: if serial_number is given, the info descriptor should also point to that one!
  newdevdesc->info = newinfo;
  newdevdesc->descriptor = newdesc;

  // Set the hid_read() function to be non-blocking.
  hid_set_nonblocking( handle, 1);

  return newdevdesc;
}

void hid_close_device( struct hid_dev_desc * devdesc ){
  hid_close( devdesc->device );
  hid_free_enumeration( devdesc->info );
  hid_free_descriptor( devdesc->descriptor );
  //TODO: more memory freeing?  
}
