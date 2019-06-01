#include <boost/test/unit_test.hpp>

#include <iostream>

#include <boost/filesystem.hpp>

#include "sc/sc_ugen_factory.hpp"
#include "server/memory_pool.hpp"
#include "server/server_args.hpp"

using namespace nova;
using namespace std;

boost::filesystem::path base_path("/home/tim/workspace/nova-server/debug_plugins/");

BOOST_AUTO_TEST_CASE(ugen_factory_test_1) {
    server_arguments::initialize(0, 0);
    rt_pool.init(1024 * 1024);
    sc_factory.initialize();

    sc_factory.load_plugin(base_path / "BinaryOpUGens.so");
    sc_factory.load_plugin(base_path / "ChaosUGens.so");
    sc_factory.load_plugin(base_path / "DelayUGens.so");
    sc_factory.load_plugin(base_path / "DemandUGens.so");
    sc_factory.load_plugin(base_path / "DiskIO_UGens.so");
    sc_factory.load_plugin(base_path / "DynNoiseUGens.so");
    sc_factory.load_plugin(base_path / "FFT_UGens.so");
    sc_factory.load_plugin(base_path / "FilterUGens.so");
    sc_factory.load_plugin(base_path / "GendynUGens.so");
    sc_factory.load_plugin(base_path / "GrainUGens.so");
    sc_factory.load_plugin(base_path / "IOUGens.so");
    sc_factory.load_plugin(base_path / "KeyboardUGens.so");
    sc_factory.load_plugin(base_path / "LFUGens.so");
    sc_factory.load_plugin(base_path / "ML_UGens.so");
    sc_factory.load_plugin(base_path / "MouseUGens.so");
    sc_factory.load_plugin(base_path / "MulAddUGens.so");
    sc_factory.load_plugin(base_path / "NoiseUGens.so");
    sc_factory.load_plugin(base_path / "OscUGens.so");
    sc_factory.load_plugin(base_path / "PanUGens.so");
    sc_factory.load_plugin(base_path / "PhysicalModelingUGens.so");
    sc_factory.load_plugin(base_path / "PV_ThirdParty.so");
    sc_factory.load_plugin(base_path / "ReverbUGens.so");
    sc_factory.load_plugin(base_path / "TestUGens.so");
    sc_factory.load_plugin(base_path / "TriggerUGens.so");
    sc_factory.load_plugin(base_path / "UnaryOpUGens.so");
    sc_factory.load_plugin(base_path / "UnpackFFTUGens.so");

    rt_pool.init(1024 * 1024 * 16, true);
}


const char* test_synthdefs[] = {
    "default.scsyndef",       "help-In.scsyndef",         "help_out.scsyndef",
    "help_out2.scsyndef",     "help_InFeedback.scsyndef", "help_LocalIn.scsyndef",
    "help_PlayBuf.scsyndef",  "help_RecordBuf.scsyndef",  "help_RecordBuf_overdub.scsyndef",
    "help_LocalBuf.scsyndef", "help_Demand.scsyndef",
};


#if 0 /* doesn't work anymore because of increased sanity checks */
BOOST_AUTO_TEST_CASE( ugen_construct_test_1 )
{
    for (int i = 0; i != sizeof(test_synthdefs)/sizeof(const char*); ++i) {
        const char * synthdef = test_synthdefs[i];

        try {
            std::vector<nova::sc_synthdef> defs =
                nova::read_synthdef_file(base_path / ".." / "testsuite" / synthdef);
            sc_synth_prototype_ptr prtype(new sc_synth_prototype(defs[0]));

            sc_synth * s = new sc_synth(1000, prtype);

            dsp_context context(44100, 64, 0);
            for (int i = 0; i != 1000; ++i)
                s->run(context);
            delete s;
        }
        catch(std::runtime_error const & e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}
#endif
