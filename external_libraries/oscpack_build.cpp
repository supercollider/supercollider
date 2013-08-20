#include <boost/detail/endian.hpp>

#ifdef BOOST_LITTLE_ENDIAN
#define OSC_HOST_LITTLE_ENDIAN
#elif defined(BOOST_BIG_ENDIAN)
#define OSC_HOST_BIG_ENDIAN
#else
#error please define endianness
#endif

#include "oscpack_1_1_0/osc/OscOutboundPacketStream.cpp"
#include "oscpack_1_1_0/osc/OscPrintReceivedElements.cpp"
#include "oscpack_1_1_0/osc/OscReceivedElements.cpp"
#include "oscpack_1_1_0/osc/OscTypes.cpp"
