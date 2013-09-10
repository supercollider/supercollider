AM_CFLAGS = -I$(top_srcdir)/hidapi/
AM_CPPFLAGS = -I$(top_srcdir)/hidapi/
## Linux
if OS_LINUX
noinst_PROGRAMS = hidapi_parser-libusb hidapi_parser-hidraw

hidapi_parser_hidraw_SOURCES = hidapi_parser.c main.c
hidapi_parser_hidraw_LDADD = $(top_builddir)/linux/libhidapi-hidraw.la

hidapi_parser_libusb_SOURCES = hidapi_parser.c main.c
hidapi_parser_libusb_LDADD = $(top_builddir)/libusb/libhidapi-libusb.la
else

noinst_PROGRAMS = hidapi_parser

hidapi_parser_SOURCES = hidapi_parser.c main.c
# hidapi_parser_HEADERS = hidapi_parser.h
hidapi_parser_LDADD = $(top_builddir)/$(backend)/libhidapi.la

endif
