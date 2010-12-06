#include <boost/detail/endian.hpp>

#ifdef BOOST_LITTLE_ENDIAN
#define OSC_HOST_LITTLE_ENDIAN
#elif defined(BOOST_BIG_ENDIAN)
#define OSC_HOST_BIG_ENDIAN
#else
#error please define endianness
#endif

#include "oscpack/osc/OscOutboundPacketStream.cpp"
#include "oscpack/osc/OscPrintReceivedElements.cpp"
#include "oscpack/osc/OscReceivedElements.cpp"
#include "oscpack/osc/OscTypes.cpp"
