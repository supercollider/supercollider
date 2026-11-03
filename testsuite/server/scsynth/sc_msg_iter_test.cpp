#include <boost/test/unit_test.hpp>

#include "sc_msg_iter.h"

#include "osc/OscOutboundPacketStream.h"

#include <array>
#include <cstring>


BOOST_AUTO_TEST_CASE(sc_msg_iter_test) {
    std::array<char, 8192> buffer;
    osc::OutboundPacketStream msg(buffer.data(), buffer.size());

    const size_t numArgs = 7;

    const char* pattern = "/test";
    osc::int32 int32Arg = 0x3E850B6B;
    osc::int64 int64Arg = 0x5BE9EF51A69A5F8E;
    float32 float32Arg = 3.141592;
    float64 float64Arg = 3.141592653589793;
    osc::TimeTag timeTagArg(0xEF3402F355A432E9);
    const char* stringArg = "this is a string";

    // clang-format off
    std::array<unsigned char, 51> blobData = {
        0x3A, 0xF7, 0x92, 0x1C, 0xB5, 0x08, 0xE4, 0x6D,
        0xA1, 0x5F, 0xCC, 0x37, 0x80, 0x2B, 0xD9, 0x74,
        0x4E, 0xA3, 0x19, 0xF0, 0x66, 0xBB, 0x0D, 0x95,
        0xC8, 0x52, 0x7E, 0xE1, 0x2A, 0x9C, 0x43, 0xD6,
        0x8B, 0x01, 0xFA, 0x35, 0x6C, 0xAE, 0x59, 0xC2,
        0x17, 0xE8, 0x90, 0x4B, 0xDD, 0x23, 0x7A, 0xB4,
        0x6F, 0x0E, 0xC5
    };
    // clang-format on
    osc::Blob blobArg(blobData.data(), blobData.size());

    msg << osc::BeginMessage(pattern) << int32Arg << int64Arg << float32Arg << float64Arg << stringArg << blobArg
        << timeTagArg << osc::EndMessage;

    auto argData = OSCstrskip(msg.Data());
    auto patternSize = OSCstrlen(msg.Data());
    BOOST_TEST(argData == msg.Data() + patternSize);
    auto argSize = msg.Data() + msg.Size() - argData;

    sc_msg_iter args(argSize, argData);

    BOOST_TEST(strlen(args.tags) == numArgs);

    BOOST_TEST(args.geti() == int32Arg);

    // sc_msg_iter currently has no member function for reading a 64-bit integer
    BOOST_TEST(args.tags[args.count] == 'h');
    BOOST_TEST(OSCint64(args.rdpos) == int64Arg);
    args.count++;
    args.rdpos += 8;

    BOOST_TEST(args.getf() == float32Arg);

    BOOST_TEST(args.getd() == float64Arg);

    BOOST_TEST(args.gets() == stringArg);

    auto blobSize = args.getbsize();
    BOOST_TEST(blobSize == blobData.size());

    std::array<char, 64> blobBuffer;
    args.getb(blobBuffer.data(), blobBuffer.size());
    BOOST_TEST(memcmp(blobData.data(), blobBuffer.data(), blobSize) == 0);

    BOOST_TEST(args.gett() == timeTagArg);

    BOOST_TEST(args.remain() == 0);
    BOOST_TEST(args.nextTag() == '\0');
}
