#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/array.hpp>


#include "../source/dsp/sample_extractor.hpp"

const int size = 4410;

/* workaround for gcc bug on x86_64 */
namespace nova
{
inline float * get_samples(boost::array<float, size> & arg)
{
    return arg.begin();
}

inline const float * get_samples(boost::array<float, size> const & arg)
{
    return arg.begin();
}

inline double * get_samples(boost::array<double, size> & arg)
{
    return arg.begin();
}

inline const double * get_samples(boost::array<double, size> const & arg)
{
    return arg.begin();
}
}

#include "../source/dsp/noise_utils.hpp"
#include "../source/dsp/noise.hpp"


#include <sndfile.hh>




using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( generate_white_noise_test )
{
    boost::array<float, size> out;

    for (int i = 0; i != size; ++i)
        out[i] = generate_white_noise<float>();

#if 0
    SndfileHandle sf("white_noise.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, 1, 44100);

    assert(sf);

    sf.write(out.begin(), size);
#endif
}

namespace
{
template <typename float_type>
void run_white_noise_test(void)
{
    boost::array<float_type, size> out;
    white_noise<float_type> gen;

    gen.perform(out, size);
    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE_LE(abs(out[i]), 1.0);
}

template <typename float_type>
void run_white_noise_external_engine_test(void)
{
    boost::array<float_type, size> out;

    detail::cheap_noise_engine e;
    white_noise_external_engine<float_type, detail::cheap_noise_engine> gen;

    gen.perform(out, size, e);
    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE_LE(abs(out[i]), 1.0);
}

template <typename float_type>
void run_clipped_noise_test(void)
{
    boost::array<float_type, size> out;
    clipped_noise<float_type> gen;

    gen.perform(out, size);
    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE(out[i] == 1.0 or out[i] == -1.0);
}

template <typename float_type>
void run_clipped_noise_external_engine_test(void)
{
    boost::array<float_type, size> out;

    detail::cheap_noise_engine e;
    clipped_noise_external_engine<float_type, detail::cheap_noise_engine> gen;

    gen.perform(out, size, e);
    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE(out[i] == 1.0 or out[i] == -1.0);
}

template <typename float_type, typename generator>
void pink_noise_engine_test(float_type max, int index)
{
    typename generator::state_type state;
    state.assign(0);

    detail::cheap_noise_engine e;

    boost::array<float_type, size> out;

    generator gen(e, state);

    float_type f = 0;
    for (int i = 0; i != size; ++i)
    {
        float_type pink = gen();
        BOOST_REQUIRE_LE(abs(pink), max);
        f += std::sqrt(pink*pink);
        out[i] = pink;
    }

    f /= size;

    BOOST_REQUIRE_CLOSE(f, float_type(0.151), float_type(1.0));

#if 0
    cout << f << endl;
#endif

#if 0
    string name = "pink_noise" + boost::lexical_cast<string>(index) +".wav";
    SndfileHandle sf(name.c_str(),
                     SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, 1, 44100);
    assert(sf);

    sf.write(out.begin(), size);
#endif
}

} /* namespace */


BOOST_AUTO_TEST_CASE( white_noise_test )
{
    run_white_noise_test<float>();
    run_white_noise_test<double>();
    run_white_noise_external_engine_test<float>();
    run_white_noise_external_engine_test<double>();
}

BOOST_AUTO_TEST_CASE( clipped_noise_test )
{
    run_clipped_noise_test<float>();
    run_clipped_noise_test<double>();
    run_clipped_noise_external_engine_test<float>();
    run_clipped_noise_external_engine_test<double>();
}

BOOST_AUTO_TEST_CASE( pink_generator_test_voss_mccartney)
{
/*     cout << "stochastic voss_mccartney" << endl; */
    pink_noise_engine_test<float, detail::pink_stochastic_voss_mccartney<float, detail::cheap_noise_engine> >(1, 1);
    pink_noise_engine_test<double, detail::pink_stochastic_voss_mccartney<double, detail::cheap_noise_engine> >(1, 2);

/*     cout << "stochastic voss_mccartney (old)" << endl; */
    pink_noise_engine_test<float, detail::pink_stochastic_voss_mccartney_old<float, detail::cheap_noise_engine> >(1, 3);
    pink_noise_engine_test<double, detail::pink_stochastic_voss_mccartney_old<double, detail::cheap_noise_engine> >(1, 4);
}


BOOST_AUTO_TEST_CASE( pink_generator_test_kellett)
{
/*     cout << "kellett" << endl; */
    pink_noise_engine_test<float, detail::pink_kellett<float, detail::cheap_noise_engine> >(1, 5);
    pink_noise_engine_test<double, detail::pink_kellett<double, detail::cheap_noise_engine> >(1, 6);

/*     cout << "kellett economy" << endl; */
    pink_noise_engine_test<float, detail::pink_kellett_economy<float, detail::cheap_noise_engine> >(1, 7);
    pink_noise_engine_test<double, detail::pink_kellett_economy<double, detail::cheap_noise_engine> >(1, 8);
}


#define RUN_SR_GENERATOR(NAME, ENGINE, LIMIT)                       \
namespace {                                                         \
template <typename float_type>                                      \
void run_##NAME##_noise_test(void)                                  \
{                                                                   \
    boost::array<float_type, size> out;                             \
    NAME##_noise<float_type> gen;                                   \
                                                                    \
    gen.perform(out, size);                                         \
                                                                    \
    for (int i = 0; i != size; ++i)                                 \
        BOOST_REQUIRE_LE(abs(out[i]), LIMIT);                       \
                                                                    \
    float_type f;                                                   \
    for (int i = 0; i != size; ++i)                                 \
    {                                                               \
        gen.tick(f);                                                \
        BOOST_REQUIRE_LE(abs(f), LIMIT);                            \
    }                                                               \
                                                                    \
}                                                                   \
                                                                    \
template <typename float_type>                                      \
void run_##NAME##_noise_external_engine_test(void)                  \
{                                                                   \
    boost::array<float_type, size> out;                             \
                                                                    \
    ENGINE e;                                                       \
    NAME##_noise_external_engine<float_type, ENGINE> gen;           \
                                                                    \
    gen.perform(out, size, e);                                      \
    for (int i = 0; i != size; ++i)                                 \
        BOOST_REQUIRE_LE(abs(out[i]), 1.0);                         \
                                                                    \
    float_type f;                                                   \
    for (int i = 0; i != size; ++i)                                 \
    {                                                               \
        gen.tick(f, e);                                             \
        BOOST_REQUIRE_LE(abs(f), LIMIT);                            \
    }                                                               \
}                                                                   \
                                                                    \
}                                                                   \
                                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_test_float )                     \
{                                                                   \
    run_##NAME##_noise_test<float>();                               \
}                                                                   \
                                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_test_double )                    \
{                                                                   \
    run_##NAME##_noise_test<double>();                              \
}                                                                   \
                                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_external_engine_test_float )     \
{                                                                   \
    run_##NAME##_noise_external_engine_test<float>();               \
}                                                                   \
                                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_external_engine_test_double )    \
{                                                                   \
    run_##NAME##_noise_external_engine_test<double>();              \
}

RUN_SR_GENERATOR(brown, detail::cheap_noise_engine, 1.0)
RUN_SR_GENERATOR(pink, detail::cheap_noise_engine, 1.0)
RUN_SR_GENERATOR(grey, detail::taus88_engine, 1.3)


#define RUN_LF_GENERATOR(NAME, LIMIT)               \
namespace {                                         \
template <typename float_type>                      \
void run_##NAME##_noise_test(void)                  \
{                                                   \
    boost::array<float_type, size> out;             \
    NAME##_noise<float_type> gen;                   \
                                                    \
    gen.set_frequency(2000, 44100);                 \
                                                    \
    gen.perform(out, size/2);                       \
    gen.set_frequency(1000, 44100);                 \
                                                    \
    float_type * ptr = out.begin()+size/2;          \
    gen.perform(ptr, size/2);                       \
                                                    \
    for (int i = 0; i != size; ++i)                 \
        BOOST_REQUIRE_LE(abs(out[i]), LIMIT);       \
                                                    \
    float_type f;                                   \
    for (int i = 0; i != size; ++i)                 \
    {                                               \
        gen.tick(f);                                \
        BOOST_REQUIRE_LE(abs(f), LIMIT);            \
    }                                               \
                                                    \
}                                                   \
}                                                   \
                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_test_float )     \
{                                                   \
    run_##NAME##_noise_test<float>();               \
}                                                   \
                                                    \
BOOST_AUTO_TEST_CASE( NAME##_noise_test_double )    \
{                                                   \
    run_##NAME##_noise_test<double>();              \
}


RUN_LF_GENERATOR(lf0_white, 1.0);
RUN_LF_GENERATOR(lf1_white, 1.03);
RUN_LF_GENERATOR(lf2_white, 1.1);
RUN_LF_GENERATOR(lf2a_white, 2);

RUN_LF_GENERATOR(lf0_clipped, 1.0);
RUN_LF_GENERATOR(lf1_clipped, 1.1);
RUN_LF_GENERATOR(lf2_clipped, 1.8);
RUN_LF_GENERATOR(lf2a_clipped, 3.5);
