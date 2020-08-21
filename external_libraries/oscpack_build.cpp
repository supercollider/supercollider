#include <boost/predef.h>

#if BOOST_ENDIAN_LITTLE_BYTE
#define OSC_HOST_LITTLE_ENDIAN
#elif BOOST_ENDIAN_BIG_BYTE
#define OSC_HOST_BIG_ENDIAN
#else
#error please define endianness
#endif

#include "oscpack_1_1_0/osc/OscOutboundPacketStream.cpp"
#include "oscpack_1_1_0/osc/OscPrintReceivedElements.cpp"
#include "oscpack_1_1_0/osc/OscReceivedElements.cpp"
#include "oscpack_1_1_0/osc/OscTypes.cpp"
