#include <boost/test/unit_test.hpp>

#include <memory>
#include "../source/server/ugen.hpp"
#include "../libs/nova-dsp/nova-dsp/sine_oscillator.hpp"

using namespace nova;
using namespace boost;
using namespace std;

namespace
{

class sine_ugen_s:
    public ugen
{
    void prepare(inblock_array_t const & ins, outblock_array_t const & outs)
    {
        in = ins[0];
        out = outs[0];
    }

    void prepare(const dsp_context * context)
    {
        generator.set_inv_samplerate(context->inv_samplerate());
    }

public:
    void run(uint32 samples, const dsp_context * context)
    {
        generator.perform(in, out, samples);
    }

private:
    const_aligned_sample_ptr in;
    aligned_sample_ptr out;

    nova::sine_oscillator<sample> generator;
};

class sine_ugen_c:
    public ugen
{
    void prepare(inblock_array_t const &, outblock_array_t const & outs)
    {
        out = outs[0];
    }

    void prepare(const dsp_context * context)
    {
        generator.set_inv_samplerate(context->inv_samplerate());
    }

    sample get_control(int index)
    {
        /* dummy: implement me */
        return 0;
    }

public:
    void run(uint32 samples, const dsp_context * context)
    {
        generator.set_frequency(get_control(0));
        generator.perform(out, samples);
    }

private:
    aligned_sample_ptr out;
    nova::sine_oscillator<sample> generator;
};

class sine_ugen_e:
    public ugen
{
    void prepare(inblock_array_t const &, outblock_array_t const & outs)
    {
        out = outs[0];
    }

    void prepare(const dsp_context * context)
    {
        generator.set_inv_samplerate(context->inv_samplerate());
    }

    void set_control(int index, sample value)
    {
        if (index == 0)
            generator.set_frequency(value);
    }

public:
    void run(uint32 samples, const dsp_context * context)
    {
        generator.perform(out, samples);
    }

    void run_unrolled(uint32 samples, const dsp_context * context)
    {
        generator.perform(out, samples);
    }

private:
    aligned_sample_ptr out;
    nova::sine_oscillator<sample> generator;
};

class sine_ugen_factory:
    public ugen_factory
{
public:
    ugen_runner create_instance (ugen_arg_signature const & sig, uint32 samples)
    {
        switch(sig[0])
        {
        case signal:
            if (sig.size() == 1)
                return ugen_runner(new sine_ugen_s());

        case control:
            if (sig.size() == 1)
                return ugen_runner(new sine_ugen_c());

        case event:
            if (sig.size() == 1)
                return ugen_runner(new sine_ugen_e());

        default:
            ;
        }
        assert(false);
    }
};


}


BOOST_AUTO_TEST_CASE( ugen_factory_test_1 )
{
    sine_ugen_factory sf;

    {
        ugen_arg_signature uas;
        uas.push_back(signal);

        ugen_runner ugen_a (sf.create_instance(uas, 64));
    }

    {
        ugen_arg_signature uac;
        uac.push_back(control);

        ugen_runner ugen_c (sf.create_instance(uac, 64));
    }

    {
        ugen_arg_signature uac;
        uac.push_back(control);

        ugen_runner ugen_c (sf.create_instance(uac, 64));
    }

    {
        ugen_arg_signature uae;
        uae.push_back(control);

        ugen_runner ugen_e (sf.create_instance(uae, 64));
    }

    {
        ugen_arg_signature uacnst;
        uacnst.push_back(control);

        ugen_runner ugen_cnst (sf.create_instance(uacnst, 64));
    }

}

namespace
{

void runner (ugen * obj, uint32 samples, dsp_context* c)
{
    static_cast<sine_ugen_s*>(obj)->run(samples, c);
}

} /* namespace */




BOOST_AUTO_TEST_CASE( ugen_runner_test_1 )
{
    ugen_runner tst(new sine_ugen_s(), (ugen_runner::runfn)runner);
    ugen_runner tst2(new sine_ugen_s(), (ugen_runner::runfn)runner, (ugen_runner::runfn)runner);
}

BOOST_AUTO_TEST_CASE( ugen_runner_test_2 )
{
    ugen_runner tst(new sine_ugen_s());
}

BOOST_AUTO_TEST_CASE( ugen_runner_test_3 )
{
    ugen_runner tst(new sine_ugen_e());
    tst.set_unrolled<sine_ugen_e>();
}
