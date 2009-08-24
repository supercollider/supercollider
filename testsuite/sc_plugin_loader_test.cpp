#include <boost/test/unit_test.hpp>

#include <boost/filesystem.hpp>

#include "../source/sc/sc_ugen_factory.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( ugen_factory_test_1 )
{
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/BeatTrack2.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/BeatTrack.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/BinaryOpUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/ChaosUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/Convolution.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/DelayUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/DemandUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/DiskIO_UGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/DynNoiseUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/FeatureDetection.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/FFT2InterfaceTable.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/FFTInterfaceTable.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/FFT_UGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/FilterUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/GendynUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/GrainUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/IOUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/KeyboardUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/KeyTrack.so");
/*     ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/LFUGens.so"); */
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/Loudness.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/MFCC.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/ML.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/ML_SpecStats.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/MouseUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/MulAddUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/NoiseUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/Onsets.so");
/*     ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/OscUGens.so"); */
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/PanUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/PartitionedConvolution.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/PhysicalModelingUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/PV_ThirdParty.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/PV_UGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/ReverbUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/SCComplex.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/TestUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/TriggerUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/UnaryOpUGens.so");
    ugen_factory.load_ugen( "/home/tim/workspace/nova-server/plugins/UnpackFFTUGens.so");
}
