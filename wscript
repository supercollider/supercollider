#! /usr/bin/env python
# encoding: utf-8

# =====================================================================
# Experimental waf build script for SuperCollider, currently Linux-only
# use with caution!!
# --artfwo

import os
import sys

VERSION='3.3.1'
APPNAME='supercollider'
srcdir = '.'
blddir = 'build/waf' # FIXME

SC3_API_VERSION=VERSION

def set_options(opt):
    opt.tool_options('gnu_dirs')
    opt.tool_options('compiler_cc')
    opt.tool_options('compiler_cxx')

def configure(conf):
    import Utils
    platform = Utils.detect_platform()

    conf.env['IS_LINUX'] = platform == 'linux'
    conf.env['IS_WIN32'] = platform == 'win32'
    conf.env['IS_MACOSX'] = platform == 'darwin'
    conf.env['IS_FREEBSD'] = platform == 'freebsd'

    if not conf.env['IS_LINUX']:
        Utils.pprint('RED', 'Sorry, waf builder currently does not work on %s platform!' % platform)
        sys.exit(1)

    conf.check_tool('gnu_dirs')
    conf.check_tool('compiler_cc')
    conf.check_tool('compiler_cxx')
    #conf.check_tool('misc')

    conf.check_cfg(atleast_pkgconfig_version='0.0.0')

    conf.check_cfg(package='sndfile', atleast_version='1.0.16',
        args='--cflags --libs', mandatory=True)
    conf.check_cfg(package='jack', atleast_version='0.100',
        args='--cflags --libs', mandatory=True)
    conf.check_cfg(package='avahi-client',
        args='--cflags --libs', mandatory=True)
    conf.check_cfg(package='alsa',
        args='--cflags --libs', mandatory=True)
    conf.check_cfg(package='xt',
        args='--cflags --libs', mandatory=True)
    conf.check_cfg(package='fftw3f',
        args='--cflags --libs', mandatory=True)

    conf.check_cc(header_name='cwiid.h', lib='cwiid', uselib_store='CWIID')
    conf.check_cc(header_name='linux/input.h')
    conf.check_cc(header_name='xmmintrin.h', ccflags=['-msse', '-mfpmath=sse'], define_name='HAVE_SSE')

    if conf.env['HAVE_SSE']:
        conf.env.append_unique('CCFLAGS', ['-msse', '-mfpmath=sse'])
        conf.env.append_unique('CXXFLAGS', ['-msse', '-mfpmath=sse'])
        conf.define('SC_MEMORY_ALIGNMENT', 16)
    else:
        conf.define('SC_MEMORY_ALIGNMENT', 1)

    #conf.env['LIBDIR'] = os.path.join(conf.env['PREFIX'], 'lib')
    #conf.env['DATADIR'] = os.path.join(conf.env['PREFIX'], 'share')

    # global defines
    conf.define('HAVE_ALSA', 1)
    conf.define('HAVE_AVAHI', 1)
    conf.define('HAVE_LID', 1)
    conf.define('HAVE_WII', 1)
    #conf.define('SC_AUDIO_API=SC_AUDIO_API_JACK')
    conf.define('SC_FFT_FFTW', 1)
    conf.define('SC_LINUX', 1)
    #conf.define('SC_MEMORY_ALIGNMENT', 16)
    conf.define('SC_USE_JACK_CLIENT_NEW', 1)
    conf.define('USE_RENDEZVOUS', 1)
    conf.define('USE_SC_TERMINAL_CLIENT', 1)

    conf.define('SC_DATA_DIR', os.path.join(conf.env['DATADIR'], 'SuperCollider'))
    conf.define('SC_PLUGIN_DIR', os.path.join(conf.env['LIBDIR'], 'SuperCollider', 'plugins'))
    conf.define('SC_PLUGIN_EXT', '.so')
    conf.define('SC_PLUGIN_LOAD_SYM', 'load')

    # other defines from SConstruct
    conf.env.append_unique('CCFLAGS', ['-Wno-unknown-pragmas'])
    conf.env.append_unique('CXXFLAGS', ['-Wno-unknown-pragmas', '-Wno-deprecated'])

    # FIXME: ugly hack but SC3 does not use config.h now
    conf.env.append_value('CPPFLAGS', ['-include','default/waf_config.h'])
    conf.write_config_header('waf_config.h')

def sc_plugin_gen(bld, name, source, uselib='', add_objects=[]):
    obj = bld.new_task_gen('cxx', 'shlib')
    obj.add_objects = add_objects
    obj.env['shlib_PATTERN'] = '%s.so'
    obj.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    obj.install_path = '${SC_PLUGIN_DIR}/'
    obj.source = source
    obj.target = name
    obj.uselib = uselib
    return obj

def build(bld):
    # objects for other targets
    dtoa = bld.new_task_gen('cc')
    dtoa.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    dtoa.source = 'Source/common/dtoa.c'
    dtoa.target = 'dtoa'

    fftlib = bld.new_task_gen('cc')
    fftlib.includes = '#Headers/common'
    fftlib.source = 'Source/common/fftlib.c'
    fftlib.target = 'fftlib'

    # libcommon
    libcommon = bld.new_task_gen('cxx', 'staticlib')
    libcommon.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    libcommon.source = [
        'Source/common/g_fmt.c',
        'Source/common/SC_AllocPool.cpp',
        'Source/common/SC_DirUtils.cpp',
        'Source/common/sc_popen.cpp',
        'Source/common/SC_Sem.cpp',
        'Source/common/SC_StringBuffer.cpp',
        'Source/common/SC_StringParser.cpp',
        'Source/common/scsynthsend.cpp',
    ] # libcommon_source
    libcommon.add_objects = 'dtoa'
    libcommon.target = 'common'
    libcommon.install_path = None

    # libscsynth
    libscsynth = bld.new_task_gen('cxx', 'shlib')
    # FIXME: AUDIOAPI -> top
    libscsynth.defines = 'SC_AUDIO_API=SC_AUDIO_API_JACK'
    libscsynth.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    libscsynth.uselib = 'JACK SNDFILE AVAHI-CLIENT' # FIXME
    libscsynth.uselib_local = 'common'
    libscsynth.vnum = SC3_API_VERSION # FIXME
    libscsynth.source = [
        'Source/server/Rendezvous.cpp',
        'Source/server/Samp.cpp',
        'Source/server/SC_BufGen.cpp',
        'Source/server/SC_Carbon.cpp',
        'Source/server/SC_Complex.cpp',
        'Source/server/SC_ComPort.cpp',
        'Source/server/SC_CoreAudio.cpp',
        'Source/server/SC_Dimension.cpp',
        'Source/server/SC_Errors.cpp',
        'Source/server/SC_Graph.cpp',
        'Source/server/SC_GraphDef.cpp',
        'Source/server/SC_Group.cpp',
        'Source/server/SC_Lib_Cintf.cpp',
        'Source/server/SC_Lib.cpp',
        'Source/server/SC_MiscCmds.cpp',
        'Source/server/SC_Node.cpp',
        'Source/server/SC_Rate.cpp',
        'Source/server/SC_SequencedCommand.cpp',
        'Source/server/SC_Str4.cpp',
        'Source/server/SC_SyncCondition.cpp',
        'Source/server/SC_Unit.cpp',
        'Source/server/SC_UnitDef.cpp',
        'Source/server/SC_World.cpp',
        # FIXME:
        'Source/server/SC_Jack.cpp',
    ] # libscsynth_source
    libscsynth.target = 'scsynth'

    #uselib_local = 'my_static_lib',

    scsynth = bld.new_task_gen('cxx', 'program')
    #scsynth.defines = sc3_defines
    scsynth.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    scsynth.source = ['Source/server/scsynth_main.cpp'] #+ common_sources
    scsynth.target = 'scsynth'
    scsynth.uselib_local = 'scsynth' # FIXME
    #scsynth.uselib = ['JACK']

    # libsclang
    libsclang = bld.new_task_gen('cxx', 'shlib')
    #libsclang.defines = sc3_defines
    libsclang.includes = '#Headers/common #Headers/plugin_interface #Headers/lang #Source/lang/LangSource/Bison'
    libsclang.target = 'sclang'
    libsclang.uselib_local = 'common'
    libsclang.uselib = 'SNDFILE CWIID ALSA'
    libsclang.vnum = SC3_API_VERSION # FIXME
    libsclang.source = [
        'Source/lang/LangPrimSource/OSCData.cpp',
        'Source/lang/LangPrimSource/PyrArchiver.cpp',
        'Source/lang/LangPrimSource/PyrArrayPrimitives.cpp',
        'Source/lang/LangPrimSource/PyrBitPrim.cpp',
        'Source/lang/LangPrimSource/PyrCharPrim.cpp',
        'Source/lang/LangPrimSource/PyrFilePrim.cpp',
        'Source/lang/LangPrimSource/PyrListPrim.cpp',
        'Source/lang/LangPrimSource/PyrMathPrim.cpp',
        'Source/lang/LangPrimSource/PyrPlatformPrim.cpp',
        'Source/lang/LangPrimSource/PyrPrimitive.cpp',
        'Source/lang/LangPrimSource/PyrSched.cpp',
        'Source/lang/LangPrimSource/PyrSerialPrim.cpp',
        'Source/lang/LangPrimSource/PyrSignalPrim.cpp',
        'Source/lang/LangPrimSource/PyrStringPrim.cpp',
        'Source/lang/LangPrimSource/PyrSymbolPrim.cpp',
        'Source/lang/LangPrimSource/PyrUnixPrim.cpp',
        'Source/lang/LangPrimSource/PyrUStringPrim.cpp',
        'Source/lang/LangPrimSource/SC_ComPort.cpp',
        'Source/lang/LangPrimSource/SC_Wii.cpp',
        'Source/lang/LangSource/AdvancingAllocPool.cpp',
        'Source/lang/LangSource/alloca.cpp',
        'Source/lang/LangSource/Bison/lang11d_tab.cpp',
        'Source/lang/LangSource/ByteCodeArray.cpp',
        'Source/lang/LangSource/dumpByteCodes.cpp',
        'Source/lang/LangSource/DumpParseNode.cpp',
        'Source/lang/LangSource/GC.cpp',
        'Source/lang/LangSource/InitAlloc.cpp',
        'Source/lang/LangSource/PyrFileUtils.cpp',
        'Source/lang/LangSource/PyrInterpreter3.cpp',
        'Source/lang/LangSource/PyrLexer.cpp',
        'Source/lang/LangSource/PyrMathOps.cpp',
        'Source/lang/LangSource/PyrMathSupport.cpp',
        'Source/lang/LangSource/PyrMessage.cpp',
        'Source/lang/LangSource/PyrObject.cpp',
        'Source/lang/LangSource/PyrParseNode.cpp',
        'Source/lang/LangSource/PyrSignal.cpp',
        'Source/lang/LangSource/PyrSymbolTable.cpp',
        'Source/lang/LangSource/Samp.cpp',
        'Source/lang/LangSource/SC_LanguageClient.cpp',
        'Source/lang/LangSource/SC_LibraryConfig.cpp',
        'Source/lang/LangSource/SC_TerminalClient.cpp',
        'Source/lang/LangSource/SimpleStack.cpp',
        'Source/lang/LangSource/VMGlobals.cpp',
        # FIXME:
        #'Source/common/fftlib.c',
        'Source/lang/LangPrimSource/SC_LID.cpp',
        'Source/lang/LangPrimSource/SC_AlsaMIDI.cpp',
    ] # libsclang_source
    libsclang.add_objects = 'fftlib' # FIXME

    # sclang
    sclang = bld.new_task_gen('cxx', 'program')
    sclang.includes = '#Headers/lang #Headers/plugin_interface #Headers/common'
    sclang.source = ['Source/lang/LangSource/cmdLineFuncs.cpp'] #+ common_sources
    sclang.target = 'sclang'
    sclang.uselib_local = 'sclang scsynth'

    # plugins
    sccomplex = bld.new_task_gen('cxx')
    sccomplex.includes = '#Headers/common #Headers/plugin_interface #Headers/server'
    sccomplex.source = 'Source/plugins/SCComplex.cpp'
    sccomplex.target = 'SCComplex'

    stdugens = [
        'BinaryOpUGens',
        'ChaosUGens',
        'DelayUGens',
        'DemandUGens',
        'DynNoiseUGens',
        'FilterUGens',
        'GendynUGens',
        'IOUGens',
        'LFUGens',
        'MulAddUGens',
        'NoiseUGens',
        'OscUGens',
        'PanUGens',
        'PhysicalModelingUGens',
        'TestUGens',
        'TriggerUGens',
        'UnaryOpUGens',
        'GrainUGens',
        'ReverbUGens'
    ] # ugens
    for name in stdugens:
        sc_plugin_gen(bld, name, source=os.path.join('Source', 'plugins', name + '.cpp'))

    sc_plugin_gen(bld, 'FFTUgens', source=[
        'Source/plugins/FFTInterfaceTable.cpp',
        'Source/plugins/FFT_UGens.cpp',
        'Source/plugins/PV_UGens.cpp',
        'Source/plugins/PartitionedConvolution.cpp',
        'Source/common/SC_fftlib.cpp'], # XXX: special inline
        uselib='FFTW3F',
        add_objects='SCComplex')

    sc_plugin_gen(bld, 'PVThirdParty', source=[
        'Source/plugins/Convolution.cpp',
        'Source/plugins/FFT2InterfaceTable.cpp',
        'Source/plugins/FeatureDetection.cpp',
        'Source/plugins/PV_ThirdParty.cpp',
        'Source/common/SC_fftlib.cpp'], # XXX: special inline
        uselib='FFTW3F',
        add_objects='SCComplex')

    sc_plugin_gen(bld, 'UnpackFFTUGens', source=[
        'Source/plugins/UnpackFFTUGens.cpp'],
        add_objects='SCComplex')

    sc_plugin_gen(bld, 'MLUgens', source=[
        'Source/plugins/ML.cpp',
        'Source/plugins/Loudness.cpp',
        'Source/plugins/BeatTrack.cpp',
        'Source/plugins/Onsets.cpp',
        'Source/plugins/onsetsds.c',
        'Source/plugins/KeyTrack.cpp',
        'Source/plugins/MFCC.cpp',
        'Source/plugins/BeatTrack2.cpp',
        'Source/plugins/ML_SpecStats.cpp'],
        add_objects='SCComplex')

    sc_plugin_gen(bld, 'DiskIOUgens', source=[
        'Source/server/SC_SyncCondition.cpp',
        'Source/plugins/DiskIO_UGens.cpp'])

    sc_plugin_gen(bld, 'MouseUGens',
        source=os.path.join('Source', 'plugins', 'MouseUGens.cpp'),
        uselib='XT')
    sc_plugin_gen(bld, 'KeyboardUGens',
        source=os.path.join('Source', 'plugins', 'KeyboardUGens.cpp'),
        uselib='XT')

    # install headers
    bld.install_files('${PREFIX}/include/SuperCollider/common', 'Headers/common/*.h')
    bld.install_files('${PREFIX}/include/SuperCollider/lang', 'Headers/lang/*.h')
    bld.install_files('${PREFIX}/include/SuperCollider/plugin_interface', 'Headers/plugin_interface/*.h')
    bld.install_files('${PREFIX}/include/SuperCollider/server', 'Headers/server/*.h')

    #bld.install_files('${PREFIX}/share/SuperCollider/',
    #    bld.path.ant_glob('build/SCClassLibrary/**/*.sc'),
    #    relative_trick=True)
    #classfiles = bld.path.ant_glob('build/SCClassLibrary/**/*.sc', flat=True)
    #for f in classfiles.split('build/'):
    #    print f
