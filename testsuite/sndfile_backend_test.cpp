#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include "../source/audio_backend/sndfile_backend.hpp"

using namespace nova;

namespace
{
void tick(void);

sndfile_backend<tick> be;
aligned_storage_ptr<float> data(nova::calloc_aligned<float>(64));

void tick(void)
{
    be.deliver_dac_output(data.get(), 0, 64);
}
}

BOOST_AUTO_TEST_CASE( sndfile_backend_test_1 )
{
    BOOST_REQUIRE(!be.audio_is_opened());
    BOOST_REQUIRE(!be.audio_is_active());

    be.open_client("_", "./output.wav", 44100, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 2);
    BOOST_REQUIRE(be.audio_is_opened());

    be.activate_audio();
    BOOST_REQUIRE(be.audio_is_active());


    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += 1;
    boost::thread::sleep(xt);

    be.deactivate_audio();
    BOOST_REQUIRE(!be.audio_is_active());

    be.close_client();
    BOOST_REQUIRE(!be.audio_is_opened());
}
