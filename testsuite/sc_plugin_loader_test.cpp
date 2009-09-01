#include <boost/test/unit_test.hpp>

#include <iostream>

#include <boost/filesystem.hpp>

#include "../source/sc/sc_ugen_factory.hpp"
#include "../source/server/memory_pool.hpp"

using namespace nova;
using namespace std;

boost::filesystem::path base_path ("/home/tim/workspace/nova-server/plugins/");

BOOST_AUTO_TEST_CASE( ugen_factory_test_1 )
{
    ugen_factory.load_plugin(base_path / "BeatTrack2.so");
    ugen_factory.load_plugin(base_path / "BeatTrack.so");
    ugen_factory.load_plugin(base_path / "BinaryOpUGens.so");
    ugen_factory.load_plugin(base_path / "ChaosUGens.so");
    ugen_factory.load_plugin(base_path / "Convolution.so");
    ugen_factory.load_plugin(base_path / "DelayUGens.so");
    ugen_factory.load_plugin(base_path / "DemandUGens.so");
    ugen_factory.load_plugin(base_path / "DiskIO_UGens.so");
    ugen_factory.load_plugin(base_path / "DynNoiseUGens.so");
    ugen_factory.load_plugin(base_path / "FeatureDetection.so");
    ugen_factory.load_plugin(base_path / "FFT2InterfaceTable.so");
    ugen_factory.load_plugin(base_path / "FFTInterfaceTable.so");
    ugen_factory.load_plugin(base_path / "FFT_UGens.so");
    ugen_factory.load_plugin(base_path / "FilterUGens.so");
    ugen_factory.load_plugin(base_path / "GendynUGens.so");
    ugen_factory.load_plugin(base_path / "GrainUGens.so");
    ugen_factory.load_plugin(base_path / "IOUGens.so");
    ugen_factory.load_plugin(base_path / "KeyboardUGens.so");
    ugen_factory.load_plugin(base_path / "KeyTrack.so");
    ugen_factory.load_plugin(base_path / "LFUGens.so");
    ugen_factory.load_plugin(base_path / "Loudness.so");
    ugen_factory.load_plugin(base_path / "MFCC.so");
    ugen_factory.load_plugin(base_path / "ML.so");
    ugen_factory.load_plugin(base_path / "ML_SpecStats.so");
    ugen_factory.load_plugin(base_path / "MouseUGens.so");
    ugen_factory.load_plugin(base_path / "MulAddUGens.so");
    ugen_factory.load_plugin(base_path / "NoiseUGens.so");
    ugen_factory.load_plugin(base_path / "Onsets.so");
    ugen_factory.load_plugin(base_path / "OscUGens.so");
    ugen_factory.load_plugin(base_path / "PanUGens.so");
    ugen_factory.load_plugin(base_path / "PartitionedConvolution.so");
    ugen_factory.load_plugin(base_path / "PhysicalModelingUGens.so");
    ugen_factory.load_plugin(base_path / "PV_ThirdParty.so");
    ugen_factory.load_plugin(base_path / "PV_UGens.so");
    ugen_factory.load_plugin(base_path / "ReverbUGens.so");
    ugen_factory.load_plugin(base_path / "SCComplex.so");
    ugen_factory.load_plugin(base_path / "TestUGens.so");
    ugen_factory.load_plugin(base_path / "TriggerUGens.so");
    ugen_factory.load_plugin(base_path / "UnaryOpUGens.so");
    ugen_factory.load_plugin(base_path / "UnpackFFTUGens.so");

    rt_pool.init(1024*1024*16, true);
}


const char * test_synthdefs[] =
{
    "default.scsyndef",
    "help-In.scsyndef",
    "help_out.scsyndef",
    "help_out2.scsyndef",
    "help_InFeedback.scsyndef",
    "help_LocalIn.scsyndef",
    "help_PlayBuf.scsyndef",
    "help_RecordBuf.scsyndef",
    "help_RecordBuf_overdub.scsyndef",
    "help_LocalBuf.scsyndef",
};


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
            s->run(context);
            delete s;
        }
        catch(std::runtime_error const & e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}
