#ifndef HIDAPI_PARSER_H__
#define HIDAPI_PARSER_H__


#define HIDAPI_MAX_DESCRIPTOR_SIZE  4096

#include <hidapi.h>

typedef struct _hid_device_element hid_device_element;
typedef struct _hid_device_descriptor hid_device_descriptor;
typedef struct _hid_dev_desc hid_dev_desc;

// struct hid_element_cb;
// struct hid_descriptor_cb;

typedef void (*hid_element_callback) ( hid_device_element *element, void *user_data);
typedef void (*hid_descriptor_callback) ( hid_device_descriptor *descriptor, void *user_data);

// typedef struct _hid_element_cb {
//     hid_element_callback cb;    
//     void *data;
// } hid_element_cb;
// 
// typedef struct _hid_descriptor_cb {
//     hid_descriptor_callback cb;    
//     void *data;
// } hid_descriptor_cb;

struct _hid_dev_desc {
    int index;
    hid_device *device;
    hid_device_descriptor *descriptor;
    struct hid_device_info *info;    
};

struct _hid_device_element {
	int index;
	
	int io_type; // input(1), output(2), feature(3)
	int type;    // flags from the input/output report
// 	int vartype; // abs/relative
	int usage_page; // usage page
	int usage;   // some kind of index (as from descriptor)

	int logical_min;
	int logical_max;
	
	int phys_min;
	int phys_max;
	
	int unit_exponent;
	int unit;
	
	int report_size;
	int report_id;
	int report_index; // index into the report

	int value;

	/** Pointer to the next element */
	hid_device_element *next;
};

struct _hid_device_descriptor {
	int num_elements;

	/** Pointer to the first element */
	hid_device_element *first;

	/** pointers to callback function */
	hid_element_callback _element_callback;
	void *_element_data;
	hid_descriptor_callback _descriptor_callback;
	void *_descriptor_data;
};

// typedef void (*event_cb_t)(const struct hid_device_element *element, void *user_data);

void hid_descriptor_init( hid_device_descriptor * devd);
void hid_free_descriptor( hid_device_descriptor * devd);

void hid_set_descriptor_callback(  hid_device_descriptor * devd, hid_descriptor_callback cb, void *user_data );
void hid_set_element_callback(  hid_device_descriptor * devd, hid_element_callback cb, void *user_data );

int hid_parse_report_descriptor( char* descr_buf, int size, hid_device_descriptor * descriptor );

hid_device_element * hid_get_next_input_element( hid_device_element * curel );

int hid_parse_input_report( unsigned char* buf, int size, hid_device_descriptor * descriptor );

float hid_element_resolution( hid_device_element * element );
float hid_element_map_logical( hid_device_element * element );
float hid_element_map_physical( hid_device_element * element );

// int hid_parse_feature_report( char* buf, int size, hid_device_descriptor * descriptor );

// higher level functions:
hid_device_descriptor * hid_read_descriptor( hid_device * devd );
hid_dev_desc * hid_open_device(  unsigned short vendor, unsigned short product, const wchar_t *serial_number );
void hid_close_device( hid_dev_desc * devdesc );

#endif
