# -*- python -*- =======================================================
# FILE:         SConstruct
# CONTENTS:     scons build script for SuperCollider
# AUTHOR:       steve AT k-hornz DOT de
# ======================================================================

# ======================================================================
# setup
# ======================================================================

EnsureSConsVersion(0,96)
EnsurePythonVersion(2,3)
SConsignFile()

# ======================================================================
# imports
# ======================================================================

import glob
import os
import re
import types
import tarfile

# ======================================================================
# constants
# ======================================================================

PACKAGE = 'SuperCollider'
VERSION = '3'

PLATFORM = os.uname()[0].lower()
CPU = os.uname()[4].lower()

ANY_FILE_RE = re.compile('.*')
HELP_FILE_RE = re.compile('.*\.(rtf(d)?|sc)$')
SC_FILE_RE = re.compile('.*\.sc$')
SRC_FILE_RE = re.compile('.*\.(c(pp)|h)$')

if PLATFORM == 'darwin':
    PLATFORM_SYMBOL = 'SC_DARWIN'
    PLUGIN_EXT = '.scx'
    DEFAULT_AUDIO_API = 'coreaudio'
elif PLATFORM == 'linux':
    PLATFORM_SYMBOL = 'SC_LINUX'
    PLUGIN_EXT = '.so'
    DEFAULT_AUDIO_API = 'jack'
elif PLATFORM == 'windows':
    PLATFORM_SYMBOL = 'SC_WIN32'
    PLUGIN_EXT = '.scx'
    DEFAULT_AUDIO_API = 'portaudio'
else:
    print 'Unknown platform: %s' % PLATFORM
    Exit(1)

# ======================================================================
# util
# ======================================================================

def CheckPKGConfig(context, version):
    context.Message( 'Checking for pkg-config... ' )
    ret = context.TryAction('pkg-config --atleast-pkgconfig-version=%s' % version)[0]
    context.Result( ret )
    return ret

def CheckPKG(context, name):
    context.Message('Checking for %s... ' % name)
    ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
    res = None
    if ret:
        res = Environment()
        res.ParseConfig('pkg-config --cflags --libs \'%s\'' % name)
    context.Result(ret)
    return (ret, res)

def merge_lib_info(env, *others):
    for other in others:
        env.Append(LIBS = other.get('LIBS', []))
        env.Append(LIBPATH = other.get('LIBPATH', []))
        env.Append(CPPPATH = other.get('CPPPATH', []))
        env.Append(CPPDEFINES = other.get('CPPDEFINES', []))
        env.Append(LINKFLAGS = other.get('LINKFLAGS', []))

def build_pkgconfig_file(target, source, env):
    out = file(str(target[0]), 'w')
    out.writelines([
        'prefix=%s\n' % env['PREFIX'],
        'exec_prefix=${prefix}\n',
        'libdir=${exec_prefix}/lib\n',
        'includedir=${prefix}/include/%s\n' % env['PACKAGE'],
        '\n',
        'Name: %s\n' % env['PKGCONFIG_NAME'],
        'Version: %s\n' % env['VERSION'],
        'Description: %s\n' % env['PKGCONFIG_DESC'],
        'Requires: %s\n' % ', '.join(env['PKGCONFIG_REQUIRES']),
        'Libs: -L${libdir} %s\n' % ' '.join(env['PKGCONFIG_LIBS']),
        'Cflags: %s\n' % ' '.join(env['PKGCONFIG_CFLAGS'])
        ])
    out.close()

def flatten_dir(dir):
    res = []
    for root, dirs, files in os.walk(dir):
        if 'CVS' in dirs: dirs.remove('CVS')
        for f in files:
            res.append(os.path.join(root, f))
    return res

def install_dir(env, src_dir, dst_dir, filter_re, strip_levels=0):
    nodes = []
    for root, dirs, files in os.walk(src_dir):
        src_paths = []
        dst_paths = []
        if 'CVS' in dirs: dirs.remove('CVS')
        for d in dirs[:]:
            if filter_re.match(d):
                src_paths += flatten_dir(os.path.join(root, d))
                dirs.remove(d)
        for f in files:
            if filter_re.match(f):
                src_paths.append(os.path.join(root, f))
        dst_paths += map(
            lambda f:
            os.path.join(
            dst_dir,
            *f.split(os.path.sep)[strip_levels:]),
            src_paths)
        nodes += env.InstallAs(dst_paths, src_paths)
    return nodes

def is_installing():
    pat = re.compile('^install.*$')
    for x in COMMAND_LINE_TARGETS:
        if pat.match(x): return True
    return False

def bin_dir(prefix):
    return os.path.join(prefix, 'bin')
def lib_dir(prefix):
    return os.path.join(prefix, 'lib')
def data_dir(prefix):
    return os.path.join(prefix, 'share')

def pkg_data_dir(prefix, *args):
    return os.path.join(data_dir(prefix), PACKAGE, *args)
def pkg_doc_dir(prefix, *args):
    return os.path.join(data_dir(prefix), 'doc', PACKAGE, *args)
def pkg_include_dir(prefix, *args):
    return os.path.join(prefix, 'include', PACKAGE, *args)
def pkg_lib_dir(prefix, *args):
    return os.path.join(lib_dir(prefix), PACKAGE, *args)

def pkg_classlib_dir(prefix, *args):
    return pkg_data_dir(prefix, 'SCClassLibrary', *args)
def pkg_extension_dir(prefix, *args):
    return pkg_data_dir(prefix, 'Extensions', *args)

# ======================================================================
# command line options
# ======================================================================

opts = Options('scache.conf', ARGUMENTS)
opts.AddOptions(
    BoolOption('ALSA',
               'Build with ALSA sequencer support', 1),
    BoolOption('ALTIVEC',
               'Build with Altivec support', 1),
    EnumOption('AUDIOAPI',
               'Build with specified audio API support',
               DEFAULT_AUDIO_API, ('jack', 'coreaudio', 'portaudio')),
    ('CUSTOMCCFLAGS', 'Custom c compiler flags', ''),
    ('CUSTOMCXXFLAGS', 'Custom c++ compiler flags', ''),
    BoolOption('DEBUG',
               'Build with debugging information', 0),
    PathOption('DESTDIR',
               'Intermediate installation prefix for packaging', '/'),
    BoolOption('DEVELOPMENT',
               'Build and install the development files', 0),
    BoolOption('LANG',
               'Build the language application', 1),
    BoolOption('LID',
               'Build with Linux Input Device support', 0),
    PathOption('PREFIX',
               'Installation prefix', '/usr/local'),
    BoolOption('RENDEZVOUS',
               'Enable Zeroconf/Rendezvous.', 1),
    BoolOption('SCEL',
               'Enable the SCEL user interface', 1),
    PackageOption('X11',
                  'Build with X11 support', 1)
    )

# ======================================================================
# basic environment
# ======================================================================

env = Environment(options = opts,
                  PACKAGE = PACKAGE,
                  VERSION = VERSION,
                  TARBALL = PACKAGE + VERSION + '.tbz2')
env.Append(
    CCFLAGS = env['CUSTOMCCFLAGS'],
    CXXFLAGS = env['CUSTOMCXXFLAGS'])

# ======================================================================
# installation directories
# ======================================================================

FINAL_PREFIX = '$PREFIX'
INSTALL_PREFIX = os.path.join('$DESTDIR', '$PREFIX')

if env['PREFIX'] == '/usr':
    FINAL_CONFIG_PREFIX = '/etc'
else:
    FINAL_CONFIG_PREFIX = os.path.join(env['PREFIX'], 'etc')
CONFIG_PREFIX = '$DESTDIR' + FINAL_CONFIG_PREFIX

env.Append(
    CPPDEFINES = [('SC_DATA_DIR', '\\"' + pkg_data_dir(FINAL_PREFIX) + '\\"')])

# ======================================================================
# configuration
# ======================================================================

conf = Configure(env, custom_tests = { 'CheckPKGConfig' : CheckPKGConfig,
                                       'CheckPKG' : CheckPKG })

if not conf.CheckPKGConfig('0'):
    print 'pkg-config not found.'
    Exit(1)

# libraries
libraries = { }
features = { }

# sndfile
success, libraries['sndfile'] = conf.CheckPKG('sndfile >= 1.0.0')
if not success: Exit(1)

# audio api
if env['AUDIOAPI'] == 'jack':
    success, libraries['audioapi'] = conf.CheckPKG('jack')
    if not success: Exit(1)
    libraries['audioapi'].Append(
        CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_JACK')],
        ADDITIONAL_SOURCES = ['source/server/SC_Jack.cpp']
        )
elif env['AUDIOAPI'] == 'coreaudio':
    libraries['audioapi'] = Environment(
        CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_COREAUDIO')],
        ADDITIONAL_SOURCES = []
        )
elif env['AUDIOAPI'] == 'portaudio':
    libraries['audioapi'] = Environment(
        CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_PORTAUDIO')],
        LIBS = ['portaudio'],
        ADDITIONAL_SOURCES = []
        )

# rendezvous
if env['RENDEZVOUS']:
    features['rendezvous'], libraries['rendezvous'] = conf.CheckPKG('howl')
    if features['rendezvous']:
        libraries['rendezvous'].Append(CPPDEFINES = ['HAVE_HOWL'])
else:
    features['rendezvous'] = False

# asound
features['alsa'] = env['ALSA'] \
                   and conf.CheckCHeader('alsa/asoundlib.h') \
                   and conf.CheckLib('asound', autoadd = False)
if features['alsa']: libraries['alsa'] = Environment(LIBS = 'asound')

# lid
features['lid'] = env['LID'] and conf.CheckCHeader('linux/input.h')

# altivec
features['altivec'] = env['ALTIVEC'] \
                      and CPU == 'ppc' \
                      and conf.CheckCHeader('altivec.h')

env = conf.Finish()

# x11
if env['X11']:
    if type(env['X11']) != types.StringType:
        env['X11'] = '/usr/X11R6'
    x11Env = Environment(
        CPPPATH = [os.path.join(env['X11'], 'include')],
        LIBPATH = [os.path.join(env['X11'], 'lib')])
    x11Conf = Configure(x11Env)
    features['x11'] = x11Conf.CheckCHeader('X11/Intrinsic.h') \
                      and x11Conf.CheckLib('X11', 'XQueryPointer')
    if features['x11']:
        libraries['x11'] = x11Conf.Finish()
else:
    features['x11'] = False

opts.Save('scache.conf', env)
Help(opts.GenerateHelpText(env))

# defines and compiler flags

env.Append(
    CPPDEFINES = ['_REENTRANT', PLATFORM_SYMBOL],
    CCFLAGS = ['-Wno-unknown-pragmas'],
    CXXFLAGS = ['-Wno-deprecated']
    )

# debugging flags
if env['DEBUG']:
    env.Append(CCFLAGS = '-g')
else:
    env.Append(CPPDEFINES = ['NDEBUG'])

# platform specific
if CPU == 'ppc':
    env.Append(CCFLAGS = '-fsigned-char')

# vectorization
if features['altivec']:
    env.Append(
        CCFLAGS = ['-maltivec', '-mabi=altivec'],
        CPPDEFINES = [('SC_MEMORY_ALIGNMENT', 16)]
        )
else:
    env.Append(CPPDEFINES = [('SC_MEMORY_ALIGNMENT', 1)])

# ======================================================================
# configuration summary
# ======================================================================

def yesorno(p):
    if p: return 'yes'
    else: return 'no'

print '------------------------------------------------------------------------'
print ' ALSA:                    %s' % yesorno(env['ALSA'])
print ' ALTIVEC:                 %s' % yesorno(features['altivec'])
print ' AUDIOAPI:                %s' % env['AUDIOAPI']
print ' DEBUG:                   %s' % yesorno(env['DEBUG'])
# print ' DESTDIR:                 %s' % env['DESTDIR']
print ' DEVELOPMENT:             %s' % yesorno(env['DEVELOPMENT'])
print ' LANG:                    %s' % yesorno(env['LANG'])
print ' LID:                     %s' % yesorno(env['LID'])
print ' PREFIX:                  %s' % env['PREFIX']
print ' RENDEZVOUS:              %s' % yesorno(env['RENDEZVOUS'])
print ' SCEL:                    %s' % yesorno(env['SCEL'])
print ' X11:                     %s' % yesorno(features['x11'])
print '------------------------------------------------------------------------'

# ======================================================================
# source/common
# ======================================================================

commonEnv = env.Copy()
commonEnv.Append(
    CPPPATH = ['#headers/common',
               '#headers/plugin_interface',
               '#headers/server']
    )

commonSources = Split('''
source/common/SC_AllocPool.cpp
source/common/SC_DirUtils.cpp
source/common/SC_Sem.cpp
source/common/SC_StringBuffer.cpp
source/common/SC_StringParser.cpp
source/common/g_fmt.c
source/common/dtoa.c
source/common/scsynthsend.cpp
''')
if PLATFORM == 'darwin':
    commonSources += ['source/common/dlopen.c']
commonEnv.Library('common', commonSources)

# ======================================================================
# source/server
# ======================================================================

serverEnv = env.Copy()
serverEnv.Append(
    CPPPATH = ['#headers/common',
               '#headers/plugin_interface',
               '#headers/server'],
    CPPDEFINES = [('SC_PLUGIN_DIR', '\\"' + pkg_lib_dir(FINAL_PREFIX, 'plugins') + '\\"'), ('SC_PLUGIN_EXT', '\\"' + PLUGIN_EXT + '\\"')],
    LIBS = ['common', 'pthread', 'dl'],
    LIBPATH = '.')
libscsynthEnv = serverEnv.Copy(
    PKGCONFIG_NAME = 'libscsynth',
    PKGCONFIG_PREFIX = FINAL_PREFIX,
    PKGCONFIG_DESC = 'SuperCollider synthesis server library',
    PKGCONFIG_REQUIRES = ['sndfile', 'jack'],
    PKGCONFIG_LIBS = ['-lscsynth'],
    PKGCONFIG_CFLAGS = ['-D' + PLATFORM_SYMBOL, '-I${includedir}/common', '-I${includedir}/plugin_interface', '-I${includedir}/server']
    )

# platform specific
if PLATFORM == 'linux':
    serverEnv.Append(CPPDEFINES = [('SC_PLUGIN_LOAD_SYM', '\\"load\\"')])

# required libraries
merge_lib_info(
    serverEnv,
    libraries['sndfile'], libraries['audioapi'])

# optional features
if features['rendezvous']:
    serverEnv.Append(CPPDEFINES = ['USE_RENDEZVOUS'])
    merge_lib_info(serverEnv, libraries['rendezvous'])

libscsynthSources = Split('''
source/server/Rendezvous.cpp
source/server/Samp.cpp
source/server/SC_BufGen.cpp
source/server/SC_Carbon.cpp
source/server/SC_Complex.cpp
source/server/SC_ComPort.cpp
source/server/SC_CoreAudio.cpp
source/server/SC_Dimension.cpp
source/server/SC_Errors.cpp
source/server/SC_Graph.cpp
source/server/SC_GraphDef.cpp
source/server/SC_Group.cpp
source/server/SC_Lib_Cintf.cpp
source/server/SC_Lib.cpp
source/server/SC_MiscCmds.cpp
source/server/SC_Node.cpp
source/server/SC_Rate.cpp
source/server/SC_SequencedCommand.cpp
source/server/SC_Str4.cpp
source/server/SC_SyncCondition.cpp
source/server/SC_Unit.cpp
source/server/SC_UnitDef.cpp
source/server/SC_World.cpp
''') + libraries['audioapi']['ADDITIONAL_SOURCES']

scsynthSources = libscsynthSources + ['source/server/scsynth_main.cpp']

libscsynth = serverEnv.SharedLibrary('scsynth', libscsynthSources)
env.Alias('install-programs', env.Install(lib_dir(INSTALL_PREFIX), [libscsynth]))

scsynth = serverEnv.Program('scsynth', scsynthSources)
env.Alias('install-programs', env.Install(bin_dir(INSTALL_PREFIX), [scsynth]))

# ======================================================================
# source/plugins
# ======================================================================

pluginEnv = env.Copy(SHLIBPREFIX = '', SHLIBSUFFIX = PLUGIN_EXT)
pluginEnv.Append(
    CPPPATH = ['#headers/common',
               '#headers/plugin_interface',
               '#headers/server']
    )
plugins = []

# simple plugins
for name in Split('''
BinaryOpUGens
ChaosUGens
DelayUGens
DemandUGens
DynNoiseUGens
FilterUGens
GendynUGens
IOUGens
LFUGens
MulAddUGens
NoiseUGens
OscUGens
PanUGens
PhysicalModelingUGens
TriggerUGens
UnaryOpUGens
'''):
    plugins.append(
        pluginEnv.SharedLibrary(
        name, os.path.join('source', 'plugins', name + '.cpp')))

# fft ugens
fftSources = Split('source/plugins/fftlib.c source/plugins/SCComplex.cpp')
plugins.append(
    pluginEnv.SharedLibrary(
    'FFT_UGens', ['source/plugins/FFT_UGens.cpp'] + fftSources))
    
plugins.append(
    pluginEnv.SharedLibrary(
    'PV_ThirdParty',
    ['source/plugins/Convolution.cpp',
     'source/plugins/FFT2InterfaceTable.cpp',
     'source/plugins/FeatureDetection.cpp',
     'source/plugins/PV_ThirdParty.cpp'] + fftSources))

# diskio ugens
diskIOEnv = pluginEnv.Copy(
    LIBS = ['common'],
    LIBPATH = '.'
    )
# rename shared object because it was already used for libscsynth
diskIOSources = [diskIOEnv.SharedObject('source/plugins/SC_SyncCondition', 'source/server/SC_SyncCondition.cpp'), 'source/plugins/DiskIO_UGens.cpp']
merge_lib_info(diskIOEnv, libraries['sndfile'])
plugins.append(
    diskIOEnv.SharedLibrary(
    'DiskIO_UGens', diskIOSources))

# ui ugens
if features['x11']:
    macUGensEnv = pluginEnv.Copy()
    merge_lib_info(macUGensEnv, libraries['x11'])
    plugins.append(
        macUGensEnv.SharedLibrary('MacUGens', 'source/plugins/MacUGens.cpp'))

env.Alias('install-plugins', env.Install(
    pkg_lib_dir(INSTALL_PREFIX, 'plugins'), plugins))

# ======================================================================
# source/lang
# ======================================================================

langEnv = env.Copy()
langEnv.Append(
    CPPPATH = ['#headers/common',
               '#headers/plugin_interface',
               '#headers/lang',
               '#headers/server',
               '#source/lang/LangSource/erase-compiler'],
    LIBS = ['common', 'scsynth', 'pthread', 'dl', 'm'],
    LIBPATH = '.'
    )
libsclangEnv = langEnv.Copy(
    PKGCONFIG_NAME = 'libsclang',
    PKGCONFIG_PREFIX = FINAL_PREFIX,
    PKGCONFIG_DESC = 'SuperCollider synthesis language library',
    PKGCONFIG_REQUIRES = ['sndfile', 'libscsynth'],
    PKGCONFIG_LIBS = ['-lsclang'],
    PKGCONFIG_CFLAGS = ['-D' + PLATFORM_SYMBOL, '-I${includedir}/common', '-I${includedir}/plugin_interface', '-I${includedir}/lang', '-I${includedir}/server']
    )

# required libraries
merge_lib_info(langEnv, libraries['sndfile'])

# optional features
if features['alsa']:
    langEnv.Append(CPPDEFINES = ['HAVE_ALSA'])
    merge_lib_info(langEnv, libraries['alsa'])

if features['lid']:
    langEnv.Append(CPPDEFINES = ['HAVE_LID'])

libsclangSources = Split('''
source/lang/LangSource/AdvancingAllocPool.cpp
source/lang/LangSource/ByteCodeArray.cpp
source/lang/LangSource/DumpParseNode.cpp
source/lang/LangSource/GC.cpp
source/lang/LangSource/InitAlloc.cpp
source/lang/LangSource/PyrFileUtils.cpp
source/lang/LangSource/PyrInterpreter3.cpp
source/lang/LangSource/PyrLexer.cpp
source/lang/LangSource/PyrMathOps.cpp
source/lang/LangSource/PyrMathPrim.cpp
source/lang/LangSource/PyrMathSupport.cpp
source/lang/LangSource/PyrMessage.cpp
source/lang/LangSource/PyrObject.cpp
source/lang/LangSource/PyrParseNode.cpp
source/lang/LangSource/PyrPrimitive.cpp
source/lang/LangSource/PyrSched.cpp
source/lang/LangSource/PyrSignal.cpp
source/lang/LangSource/PyrSignalPrim.cpp
source/lang/LangSource/PyrSymbolTable.cpp
source/lang/LangSource/SC_AlsaMIDI.cpp
source/lang/LangSource/SC_ComPort.cpp
source/lang/LangSource/SC_LID.cpp
source/lang/LangSource/SC_LanguageClient.cpp
source/lang/LangSource/SC_LibraryConfig.cpp
source/lang/LangSource/SC_TerminalClient.cpp
source/lang/LangSource/Samp.cpp
source/lang/LangSource/SimpleStack.cpp
source/lang/LangSource/VMGlobals.cpp
source/lang/LangSource/alloca.cpp
source/lang/LangSource/dumpByteCodes.cpp
source/lang/LangSource/erase-compiler/lang11d_tab.cpp
source/lang/LangPrimSource/OSCData.cpp
source/lang/LangPrimSource/PyrArchiver.cpp
source/lang/LangPrimSource/PyrArrayPrimitives.cpp
source/lang/LangPrimSource/PyrBitPrim.cpp
source/lang/LangPrimSource/PyrCharPrim.cpp
source/lang/LangPrimSource/PyrFilePrim.cpp
source/lang/LangPrimSource/PyrListPrim.cpp
source/lang/LangPrimSource/PyrStringPrim.cpp
source/lang/LangPrimSource/PyrSymbolPrim.cpp
source/lang/LangPrimSource/PyrUnixPrim.cpp
source/plugins/fftlib.c
''')

sclangSources = libsclangSources + ['source/lang/LangSource/cmdLineFuncs.cpp']

if env['LANG']:
    sclang = langEnv.Program('sclang', sclangSources)
    env.Alias('install-programs', env.Install(bin_dir(INSTALL_PREFIX), [sclang]))
    if env['DEVELOPMENT']:
        libsclang = langEnv.StaticLibrary('sclang', libsclangSources)
        env.Alias('install-dev', env.Install(lib_dir(INSTALL_PREFIX), [libsclang]))

# ======================================================================
# installation
# ======================================================================

installEnv = Environment(
    ALL = ['install-bin', 'install-data'],
    BIN = ['install-plugins', 'install-programs'],
    DATA = ['install-doc']
    )
if env['LANG']:
    installEnv.Append(DATA = ['install-library'])

if is_installing():
    # class library
    env.Alias('install-library', install_dir(
        env, 'build/SCClassLibrary',
        pkg_data_dir(INSTALL_PREFIX),
        SC_FILE_RE, 1))
    # help files
    env.Alias('install-library', install_dir(
        env, 'build/Help',
        pkg_data_dir(INSTALL_PREFIX),
        HELP_FILE_RE, 1))
    # linux extensions
    if PLATFORM == 'linux':
        env.Alias('install-library', install_dir(
            env, 'linux/lib',
            pkg_extension_dir(INSTALL_PREFIX, 'linux'),
            SC_FILE_RE, 2))
    # scel
    if env['SCEL']:
        env.Alias('install-library', install_dir(
            env, 'linux/scel/sc',
            pkg_extension_dir(INSTALL_PREFIX, 'scel'),
            SC_FILE_RE, 3))

# scel
if env['SCEL']:
    env.Command('linux/scel/el/sclang-vars.el', 'linux/scel/el/sclang-vars.el.in',
                'sed \'s,@PKG_DATA_DIR@,%s,g\' < $SOURCE > $TARGET' %
                pkg_data_dir(FINAL_PREFIX))
    el_files = glob.glob('linux/scel/el/*.el') + ['linux/scel/el/sclang-vars.el']
    elc_files = map(lambda f: os.path.splitext(f)[0] + '.elc', el_files)
    elisp_dir = os.path.join(INSTALL_PREFIX, 'share', 'emacs', 'site-lisp')
    env.Command(elc_files, el_files,
                'emacs -batch --eval "(add-to-list \'load-path (expand-file-name \\"linux/scel/el/\\"))" -f batch-byte-compile $SOURCES')
    env.Alias('install-elisp', env.Install(elisp_dir, el_files + elc_files))
    installEnv.Append(DATA = 'install-elisp')

# example library configuration file
env.Command('linux/examples/sclang.cfg', 'linux/examples/sclang.cfg.in',
            'sed \'s,@PKG_DATA_DIR@,%s,g\' < $SOURCE > $TARGET' %
            pkg_data_dir(FINAL_PREFIX))

# headers
if env['DEVELOPMENT']:
    header_dirs = Split('common plugin_interface server lang')
    if PLATFORM == 'darwin':
        header_dirs += 'app'
    for d in header_dirs:
        env.Alias('install-dev', install_dir(
            env, os.path.join('headers', d),
            pkg_include_dir(INSTALL_PREFIX),
            re.compile('.*\.h(h|pp)?'), 1))
    pkgconfig_files = [
        libscsynthEnv.Command('linux/libscsynth.pc', 'SConstruct',
                              build_pkgconfig_file),
        libsclangEnv.Command('linux/libsclang.pc', 'SConstruct',
                             build_pkgconfig_file)]
    pkgconfig_dir = os.path.join(lib_dir(INSTALL_PREFIX), 'pkgconfig')
    env.Alias('install-dev', env.Install(pkgconfig_dir, pkgconfig_files))
    installEnv.Append(ALL = 'install-dev')
    
# documentation
if is_installing():
    # TODO: build html documentation?
    doc_dir = os.path.join(data_dir(INSTALL_PREFIX), 'doc', PACKAGE)
    env.Alias('install-doc',
              install_dir(env, 'doc', doc_dir, ANY_FILE_RE, 0) +
              install_dir(env, 'build/examples', doc_dir, ANY_FILE_RE, 1) +
              install_dir(env, 'build/TestingAndToDo', doc_dir, ANY_FILE_RE, 1))

env.Alias('install-bin', installEnv['BIN'])
env.Alias('install-data', installEnv['DATA'])
env.Alias('install', installEnv['ALL'])

# ======================================================================
# distribution
# ======================================================================

DIST_FILES = Split('''
COPYING
SConstruct
changes.rtf
compile-xcode.sh
compile.sh
distro
linux/ChangeLog
linux/INSTALL
linux/NEWS
linux/README
linux/examples/onetwoonetwo.sc
linux/examples/sclang.cfg.in
linux/examples/sclang.sc
linux/scel/README
scsynthlib_exp
''')

DIST_SPECS = [
    ('English.lproj', None),
    ('Japanese.lproj', None),
    ('Psycollider', None),
    ('SuperCollider3Service', None),
    ('build', HELP_FILE_RE),
    ('doc', None),
    ('headers', SRC_FILE_RE),
    ('linux/lib', SC_FILE_RE),
    ('linux/scel/sc', SC_FILE_RE),
    ('linux/scel/el', re.compile('.*\.el$')),
    ('resources', None),
    ('source', SRC_FILE_RE),
    ('windows', None),
    ('xSC3lang.pbproj', None),
    ('xSC3plugins.pbproj', None),
    ('xSC3synth.pbproj', None)
    ]

def dist_paths():
    paths = DIST_FILES[:]
    specs = DIST_SPECS[:]
    while specs:
        base, re = specs.pop()
        if not re: re = ANY_FILE_RE
        for root, dirs, files in os.walk(base):
            if 'CVS' in dirs: dirs.remove('CVS')
            for path in dirs[:]:
                if re.match(path):
                    specs.append((os.path.join(root, path), re))
                    dirs.remove(path)
            for path in files:
                if re.match(path):
                    paths.append(os.path.join(root, path))
    paths.sort()
    return paths

def build_tar(env, target, source):
    paths = dist_paths()
    tarfile_name = str(target[0])
    tar_name = os.path.splitext(os.path.basename(tarfile_name))[0]
    tar = tarfile.open(tarfile_name, "w:bz2")
    for path in paths:
        tar.add(path, os.path.join(tar_name, path))
    tar.close()

if 'dist' in COMMAND_LINE_TARGETS:
    env.Alias('dist', env['TARBALL'])
    env.Command(env['TARBALL'], 'SConstruct', build_tar)

# ======================================================================
# cleanup
# ======================================================================

env.Clean('scrub',
          Split('config.log scache.conf .sconf_temp .sconsign.dblite'))

# ======================================================================
# EOF
