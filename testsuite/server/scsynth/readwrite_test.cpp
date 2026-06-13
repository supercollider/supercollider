#include <boost/test/unit_test.hpp>

#include "ReadWriteMacros.h"

#include <iostream>

BOOST_AUTO_TEST_CASE(readInt8_test) {
    int8 a = -120;

    char buffer[1] = { (char)a };
    const char* ptr = buffer;
    const char* end = buffer + 1;

    int8 b = readInt8(ptr, end);
    BOOST_TEST(a == b);

    int8 c = 0;
    try {
        // too little data, so readInt8 should throw
        c = readInt8(ptr, ptr);
    } catch (...) {}
    BOOST_TEST(c == 0);
}

BOOST_AUTO_TEST_CASE(readUInt8_test) {
    uint8 a = -120;

    char buffer[1] = { (char)a };
    const char* ptr = buffer;
    const char* end = buffer + 1;

    uint8 b = readUInt8(ptr, end);
    BOOST_TEST(a == b);

    uint8 c = 0;
    try {
        // too little data, so readUInt8 should throw
        c = readUInt8(ptr, ptr);
    } catch (...) {}
    BOOST_TEST(c == 0);
}

BOOST_AUTO_TEST_CASE(readInt16_be_test) {
    int16 a = 0x91C7;

    char temp[2];
    std::memcpy(&temp, &a, 2);

    char buffer[2];
    buffer[0] = temp[1];
    buffer[1] = temp[0];
    const char* ptr = buffer;
    const char* end = buffer + 2;

    int16 b = readInt16_be(ptr, end);
    BOOST_TEST(a == b);

    int16 c = 0;
    try {
        // too little data, so readInt16_be should throw
        c = readInt16_be(ptr, ptr + 1);
    } catch (...) {}
    BOOST_TEST(c == 0);
}

BOOST_AUTO_TEST_CASE(readInt32_be_test) {
    int32 a = 0xA35F91C7;

    char temp[4];
    std::memcpy(&temp, &a, 4);

    char buffer[4];
    buffer[0] = temp[3];
    buffer[1] = temp[2];
    buffer[2] = temp[1];
    buffer[3] = temp[0];
    const char* ptr = buffer;
    const char* end = buffer + 4;

    int32 b = readInt32_be(ptr, end);
    BOOST_TEST(a == b);

    int32 c = 0;
    try {
        // too little data, so readInt32_be should throw
        c = readInt32_be(ptr, ptr + 3);
    } catch (...) {}
    BOOST_TEST(c == 0);
}

BOOST_AUTO_TEST_CASE(readFloat_be_test) {
    float a = 3.14159265359;

    char temp[4];
    std::memcpy(&temp, &a, 4);

    char buffer[4];
    buffer[0] = temp[3];
    buffer[1] = temp[2];
    buffer[2] = temp[1];
    buffer[3] = temp[0];
    const char* ptr = buffer;
    const char* end = buffer + 4;

    float b = readFloat_be(ptr, end);
    BOOST_TEST(a == b);

    float c = 0;
    try {
        // too little data, so readFloat_be should throw
        c = readFloat_be(ptr, ptr + 3);
    } catch (...) {}
    BOOST_TEST(c == 0);
}
