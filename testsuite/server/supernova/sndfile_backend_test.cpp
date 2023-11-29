#include <boost/test/unit_test.hpp>
#include <chrono>
#include <thread>

#include "audio_backend/sndfile_backend.hpp"

using namespace nova;

namespace {

aligned_storage_ptr<float> data(nova::calloc_aligned<float>(64));

struct engine_functor;

struct engine_functor {
    void init_tick(void) {}

    void run_tick(void);
};

sndfile_backend<engine_functor> be;

void engine_functor::run_tick(void) {
    float* data_ptr = data.get();
    be.output_mapping(&data_ptr, &data_ptr + 1);
}

}

BOOST_AUTO_TEST_CASE(sndfile_backend_test_1) {
    BOOST_REQUIRE(!be.audio_is_opened());
    BOOST_REQUIRE(!be.audio_is_active());

    be.open_client("", "./output.wav", 44100, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 64);
    BOOST_REQUIRE(be.audio_is_opened());

    be.activate_audio();
    BOOST_REQUIRE(be.audio_is_active());

    std::this_thread::sleep_for(std::chrono::seconds(1));

    be.deactivate_audio();
    BOOST_REQUIRE(!be.audio_is_active());

    be.close_client();
    BOOST_REQUIRE(!be.audio_is_opened());
}
