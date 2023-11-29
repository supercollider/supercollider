#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#define protected public

#include "audio_backend/audio_backend.hpp"
#include "audio_backend/portaudio.hpp"
#include "audio_backend/audio_frontend.hpp"

using namespace nova;

namespace {
void tick(void) {}
}

BOOST_AUTO_TEST_CASE(frontend_test_1) {
    audio_frontend<&tick> af;

    BOOST_REQUIRE(!af.audio_is_opened());

    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(af.audio_is_active());

    BOOST_REQUIRE(!af.audio_is_opened());
}

BOOST_AUTO_TEST_CASE(frontend_test_2) {
    audio_frontend<&tick> af;

    BOOST_REQUIRE(!af.audio_is_opened());
    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(af.audio_is_active());

    af.open_portaudio_backend();

    BOOST_REQUIRE(!af.audio_is_ready());
    BOOST_REQUIRE(!af.audio_is_active());

    device_list devs = af.list_devices();
    af.open_audio_stream(devs[0], devs[0].inchannels, devs[0], devs[0].outchannels, devs[0].defaultSampleRate);

    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(!af.audio_is_active());

    af.activate_audio();

    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(af.audio_is_active());

    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += 1;
    boost::thread::sleep(xt);

    af.deactivate_audio();

    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(!af.audio_is_active());

    af.close_audio_stream();

    BOOST_REQUIRE(!af.audio_is_ready());
    BOOST_REQUIRE(!af.audio_is_active());

    af.close_audio_backend();

    BOOST_REQUIRE(af.audio_is_ready());
    BOOST_REQUIRE(af.audio_is_active());
    BOOST_REQUIRE(!af.audio_is_opened());
}
