AM_CFLAGS =  $(LIBLO_CFLAGS) -I$(top_srcdir)/hidapi/ -I$(top_srcdir)/hidapi_parser/
AM_CPPFLAGS =  $(LIBLO_CFLAGS) -I$(top_srcdir)/hidapi/ -I$(top_srcdir)/hidapi_parser/
## Linux
if OS_LINUX
noinst_PROGRAMS = hidapi2osc-libusb hidapi2osc-hidraw

hidapi2osc_hidraw_SOURCES = $(top_srcdir)/hidapi_parser/hidapi_parser.c hidapi2osc.cpp
hidapi2osc_hidraw_LDADD = $(top_builddir)/linux/libhidapi-hidraw.la $(LIBLO_LIBS)

hidapi2osc_libusb_SOURCES = $(top_srcdir)/hidapi_parser/hidapi_parser.c hidapi2osc.cpp
hidapi2osc_libusb_LDADD = $(top_builddir)/libusb/libhidapi-libusb.la $(LIBLO_LIBS)
else

noinst_PROGRAMS = hidapi2osc

hidapi2osc_SOURCES = $(top_srcdir)/hidapi_parser/hidapi_parser.c hidapi2osc.cpp
# hidapi_parser_HEADERS = hidapi_parser.h
hidapi2osc_LDADD = $(top_builddir)/$(backend)/libhidapi.la $(LIBLO_LIBS)

endif
