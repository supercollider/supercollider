#ifndef HIDAPI_PARSER_H__
#define HIDAPI_PARSER_H__


#define HIDAPI_MAX_DESCRIPTOR_SIZE  4096


struct hid_device_descriptor {
	int num_elements;
// 	int usage_page;
// 	int usage;
	/** Pointer to the first element */
	struct hid_device_element *first;
};

struct hid_device_element {
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
	
	int report_size;
	int report_id;
	int report_index; // index into the report

	int value;

	/** Pointer to the next element */
	struct hid_device_element *next;
};

int hid_parse_report_descriptor( char* descr_buf, int size, struct hid_device_descriptor * descriptor );

int hid_parse_input_report( char* buf, int size, struct hid_device_descriptor * descriptor );

float hid_element_map_logical( struct hid_device_element * element );
float hid_element_map_physical( struct hid_device_element * element );

// int hid_parse_feature_report( char* buf, int size, hid_device_descriptor * descriptor );

#endif
