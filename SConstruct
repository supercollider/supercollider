# -*- python -*- =======================================================
# FILE:         SConstruct
# CONTENTS:     scons build script for SuperCollider
# AUTHOR:       sk AT k-hornz DOT de
# modifications: nescivi AT gmail DOT com
# ======================================================================

# ======================================================================
# NOTE: Please use an indentation level of 4 spaces, i.e. no mixing of
# tabs and spaces.
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
import subprocess
import re
import types
import tarfile
import platform   # Seems to have more portable uname() than os

# ======================================================================
# constants
# ======================================================================

PACKAGE = 'SuperCollider'



f = open('VERSION')
VERSION = f.readline()
f.close()

def short_cpu_name(cpu):
    if cpu == 'Power Macintosh':
        cpu = 'ppc'
    return cpu.lower()

PLATFORM = platform.uname()[0].lower()
CPU = short_cpu_name(platform.uname()[4])

ANY_FILE_RE = re.compile('.*')
HELP_FILE_RE = re.compile('.*\.(rtf(d)?|scd|html)$')
SC_FILE_RE = re.compile('.*\.sc$')
SRC_FILE_RE = re.compile('.*\.(c(pp)|h)$')

if PLATFORM == 'darwin':
    PLATFORM_SYMBOL = 'SC_DARWIN'
    PLUGIN_EXT = '.scx'
    DEFAULT_AUDIO_API = 'coreaudio'
    DEFAULT_PREFIX = '/usr/local'
elif PLATFORM == 'freebsd':
    PLATFORM_SYMBOL = 'SC_FREEBSD'
    PLUGIN_EXT = '.so'
    DEFAULT_AUDIO_API = 'jack'
    DEFAULT_PREFIX = '/usr/local'
elif PLATFORM == 'linux':
    PLATFORM_SYMBOL = 'SC_LINUX'
    PLUGIN_EXT = '.so'
    DEFAULT_AUDIO_API = 'jack'
    DEFAULT_PREFIX = '/usr/local'
elif PLATFORM == 'windows':
    PLATFORM_SYMBOL = 'SC_WIN32'
    PLUGIN_EXT = '.scx'
    DEFAULT_AUDIO_API = 'portaudio'
    DEFAULT_PREFIX = '/'
else:
    print 'Unknown platform: %s' % PLATFORM
    Exit(1)

if CPU in [ 'ppc' , 'ppc64' ]:
    DEFAULT_OPT_ARCH = '7450'
elif CPU in [ 'i586', 'i686' ]:
    # FIXME: better detection
    DEFAULT_OPT_ARCH = CPU
else:
    DEFAULT_OPT_ARCH = None

if PLATFORM != 'windows':
	subprocess.call(['sh', 'setMainVersion.sh'])


# ======================================================================
# util
# ======================================================================


def make_os_env(*keys):
    env = os.environ
    res = {}
    for key in keys:
        if env.has_key(key):
            res[key] = env[key]
    return res

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
        res = Environment(ENV = make_os_env('PATH', 'PKG_CONFIG_PATH'))
        res.ParseConfig('pkg-config --cflags --libs \'%s\'' % name)
        res['PKGCONFIG'] = name
    context.Result(ret)
    return (ret, res)

def get_new_pkg_env():
    return Environment(ENV = make_os_env('PATH', 'PKG_CONFIG_PATH'))

def merge_lib_info(env, *others):
    for other in others:
        env.AppendUnique(CCFLAGS = other.get('CCFLAGS', []))
        env.AppendUnique(CPPDEFINES = other.get('CPPDEFINES', []))
        env.AppendUnique(CPPPATH = other.get('CPPPATH', []))
        env.AppendUnique(CXXFLAGS = other.get('CXXFLAGS', []))
        env.AppendUnique(LIBS = other.get('LIBS', []))
        env.AppendUnique(LIBPATH = other.get('LIBPATH', []))
        env['LINKFLAGS'] = env['LINKFLAGS'] + other.get('LINKFLAGS', "")

def make_pkgconfig_requires(*envs):
    res = []
    for env in envs:
        if env and env.has_key('PKGCONFIG'):
            res.append(env['PKGCONFIG'])
    return res

def build_pkgconfig_file(target, source, env):
    def write_field(file, name, prefix, separator=None):
        if env.has_key(name):
            if separator:
                content = separator.join(env[name])
            else:
                content = env[name]
            file.write('%s%s\n' % (prefix, content))
    out = file(str(target[0]), 'w')
    out.writelines([
        'prefix=%s\n' % env['PREFIX'],
        'exec_prefix=${prefix}\n',
        'libdir=${exec_prefix}/lib\n',
        'includedir=${prefix}/include/%s\n' % env['PACKAGE'],
        '\n'])
    write_field(out, 'PKGCONFIG_NAME', 'Name: ')
    write_field(out, 'PKGCONFIG_DESC', 'Description: ')
    write_field(out, 'URL', 'URL: ')
    write_field(out, 'VERSION', 'Version: ')
    write_field(out, 'PKGCONFIG_REQUIRES', 'Requires: ', ', ')
    write_field(out, 'PKGCONFIG_REQUIRES_PRIVATE', 'Requires.private: ', ', ')
    write_field(out, 'PKGCONFIG_LIBS', 'Libs: -L${libdir} ', ' ')
    write_field(out, 'PKGCONFIG_LIBS_PRIVATE', 'Libs.private: ', ' ')
    write_field(out, 'PKGCONFIG_CFLAGS', 'Cflags: ', ' ')
    out.close()

def flatten_dir(dir):
    res = []
    for root, dirs, files in os.walk(dir):
        if 'CVS' in dirs: dirs.remove('CVS')
        if '.svn' in dirs: dirs.remove('.svn')
        for f in files:
            res.append(os.path.join(root, f))
    return res

def install_dir(env, src_dir, dst_dir, filter_re, strip_levels=0):
    nodes = []
    for root, dirs, files in os.walk(src_dir):
        src_paths = []
        dst_paths = []
        if 'CVS' in dirs: dirs.remove('CVS')
        if '.svn' in dirs: dirs.remove('.svn')
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

def is_home_directory(dir):
    return os.path.normpath(dir) == os.path.normpath(os.environ.get('HOME', '/'))

def bin_dir(prefix):
    return os.path.join(prefix, 'bin')
def lib_dir(prefix):
    return os.path.join(prefix, 'lib')

def pkg_data_dir(prefix, *args):
    if PLATFORM == 'darwin':
        base = '/Library/Application Support'
        if is_home_directory(prefix):
            base = os.path.join(prefix, base)
    else:
        base = os.path.join(prefix, 'share')
    return os.path.join(base, PACKAGE, *args)
def pkg_doc_dir(prefix, *args):
    if PLATFORM == 'darwin':
        base = '/Library/Documentation'
        if is_home_directory(prefix):
            base = os.path.join(prefix, base)
    else:
        base = os.path.join(prefix, 'share', 'doc')
    return os.path.join(base, PACKAGE, *args)
def pkg_include_dir(prefix, *args):
    return os.path.join(prefix, 'include', PACKAGE, *args)
def pkg_lib_dir(prefix, *args):
    return os.path.join(lib_dir(prefix), PACKAGE, *args)

def pkg_classlib_dir(prefix, *args):
    return pkg_data_dir(prefix, 'SCClassLibrary', *args)
def pkg_extension_dir(prefix, *args):
    return pkg_data_dir(prefix, 'Extensions', *args)

def make_opt_flags(env):
    flags = [
        "-O3",
        ## "-fomit-frame-pointer", # can behave strangely for sclang
        "-ffast-math",
        "-fno-finite-math-only",
        "-fstrength-reduce"
        ]
    arch = env.get('OPT_ARCH')
    if arch:
        if CPU in [ 'ppc' , 'ppc64' ]:
            flags.extend([ "-mcpu=%s" % (arch,) ])
        else:
            flags.extend([ "-march=%s" % (arch,) ])
    if CPU in [ 'ppc' , 'ppc64' ]:
        flags.extend([ "-fsigned-char", "-mhard-float",
                       ## "-mpowerpc-gpopt", # crashes sqrt
                       "-mpowerpc-gfxopt"
                       ])
    return flags

def make_static_object(env, source, postfix="_a"):
    obj = os.path.splitext(source)[0] + postfix
    return env.StaticObject(obj, source)

def make_static_objects(env, sources, postfix="_a"):
    return map(lambda x: make_static_object(env, x, postfix), sources)

# ======================================================================
# command line options
# ======================================================================

opts = Options('scache.conf', ARGUMENTS)
opts.AddOptions(
    BoolOption('ALSA',
               'Build with ALSA sequencer support', 1),
    BoolOption('ALTIVEC',
               'Build with Altivec support', 1),
    ('OPT_ARCH', 'Architecture to optimize for', DEFAULT_OPT_ARCH),
    EnumOption('AUDIOAPI',
               'Build with specified audio API support',
               DEFAULT_AUDIO_API, ('jack', 'coreaudio', 'portaudio')),
    ('CC', 'C compiler executable'),
    ('CCFLAGS', 'C compiler flags'),
    ('CXX', 'C++ compiler executable'),
    ('CXXFLAGS', 'C++ compiler flags'),
    BoolOption('DEBUG',
               'Build with debugging information', 0),
    PathOption('DESTDIR',
               'Intermediate installation prefix for packaging', '/'),
    BoolOption('DEVELOPMENT',
               'Build and install the development files', 0),
    BoolOption('FFTW',
               'Use the FFTW libraries', PLATFORM != 'darwin'),
    BoolOption('JACK_DLL',
               'Build with delay locked loop support', 0),
    BoolOption('JACK_DEBUG_DLL',
               'Build with delay locked loop debugging support', 0),
    BoolOption('LANG',
               'Build the language application', 1),
    BoolOption('LID',
               'Build with Linux Input Device support [linux]', PLATFORM == 'linux'),
    BoolOption('NO_LIBSNDFILE',
               'Disable libsndfile (audio file reading/writing)', 0),
    BoolOption('WII',
               'Build with Linux WII support [linux]', PLATFORM == 'linux'),
    PathOption('PREFIX',
               'Installation prefix', DEFAULT_PREFIX),
    BoolOption('RENDEZVOUS',
               'Enable Zeroconf/Rendezvous.', 1),
    BoolOption('SCEL',
               'Enable the SCEL user interface; NOTE for the HTML help system you need emacs-w3m', 1),
    BoolOption('SCVIM',
               'Enable the SCVIM user interface; NOTE see the README in /editors/scvim for setting variables', 1),
    BoolOption('SCED',
               'Enable the SCED (based on gedit) user interface; NOTE see the README in /editors/sced for setting variables', 0),
    BoolOption('SSE',
               'Build with SSE support', 1),
    BoolOption('SSE2',
               'Build with SSE2 support', 1),
    BoolOption('STRIP',
               'Strip symbols from binaries', 0),
    BoolOption('CROSSCOMPILE',
               'Crosscompile for another platform (does not do SSE support check)', 0),
    BoolOption('TERMINAL_CLIENT',
               'Build with terminal client interface', 1),
    BoolOption('CURL',
               'Build with libcurl - allows server to load files from remote servers by URL', 0),
    BoolOption('READLINE',
               'Build with GNU readline for nice command-line sclang interface', PLATFORM == 'linux'),
    BoolOption('GPL3',
               'Allow the inclusion of gpl-3 licensed code. Makes the license of the whole package gpl-3', 0),
    PackageOption('X11',
                'Build with X11 support', 1),
    BoolOption('SCLANG64',
               'Build 64bit sclang (only affects compilation on 64bit systems)', 0),
    )

if PLATFORM == 'darwin':
    opts.AddOptions(
        BoolOption('UNIVERSAL',
                   'Build universal binaries (see UNIVERSAL_ARCHS)', 1),
        BoolOption('INTERNAL_LIBSNDFILE',
                   'Use internal version of libsndfile', 1),
#       ListOption('UNIVERSAL_ARCHS',
#                  'Architectures to build for',
#                  'all', ['ppc', 'i386'])
        )

# ======================================================================
# basic environment
# ======================================================================

env = Environment(options = opts,
                  ENV = make_os_env('PATH', 'PKG_CONFIG_PATH'),
                  PACKAGE = PACKAGE,
                  VERSION = VERSION,
                  URL = 'http://supercollider.sourceforge.net',
                  TARBALL = PACKAGE + VERSION + '.tbz2')
env.Append(PATH = ['/usr/local/bin', '/usr/bin', '/bin'])

# for nova-simd
env.Append(CPPPATH = ["xtralibs/include/nova-simd"])
env.Append(CPPDEFINES = ["NOVA_SIMD"])

if not env['GPL3']:
    env.Append(CPPDEFINES = ["NO_GPL3_CODE"])
    if env['READLINE']:
        print 'WARNING: option READLINE is on and option GPL3 is off, yet readline is GPL3+ licensed, so it will NOT be activated.'

# checks for DISTCC and CCACHE as used in modern linux-distros:

if os.path.exists('/usr/lib/distcc/bin'):
    os.environ['PATH']         = '/usr/lib/distcc/bin:' + os.environ['PATH']
    #env['ENV']['DISTCC_HOSTS'] = os.environ['DISTCC_HOSTS']
    env['ENV']['DISTCC_HOSTS'] = os.environ.get('DISTCC_HOSTS')

if os.path.exists('/usr/lib/ccache/bin'):
    os.environ['PATH']         = '/usr/lib/ccache/bin:' + os.environ['PATH']
    #env['ENV']['CCACHE_DIR']   = os.environ['CCACHE_DIR']
    env['ENV']['CCACHE_DIR']   = os.environ.get('CCACHE_DIR')

env['ENV']['PATH'] = os.environ['PATH']
if PLATFORM == 'windows':
	env['ENV']['HOME'] = os.environ['HOMEPATH']
else:
	env['ENV']['HOME'] = os.environ['HOME']

if PLATFORM == 'linux':
	env['amd64'] = 'x86_64' in platform.uname()
	if env['amd64']:
		print "we are on amd64 linux"


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

def make_conf(env):
    return Configure(env, custom_tests = { 'CheckPKGConfig' : CheckPKGConfig,
                                           'CheckPKG' : CheckPKG })

def isDefaultBuild():
    return not env.GetOption('clean') and not 'debian' in COMMAND_LINE_TARGETS

conf = make_conf(env)

# libraries
libraries = { }
features = { }

if isDefaultBuild():
    if not conf.CheckPKGConfig('0'):
        print 'pkg-config not found.'
        Exit(1)

    # sndfile
    if env['NO_LIBSNDFILE']:
        libraries['sndfile'] = Environment(CPPDEFINES = ['NO_LIBSNDFILE'])
    else:
        if PLATFORM == 'darwin' and env['INTERNAL_LIBSNDFILE']:
            libraries['sndfile'] = Environment(LINKFLAGS = ['xtralibs/mac/libsndfile.a'],
                                               CPPPATH = ['#xtralibs/include/libsndfile'])
        else:
            success, libraries['sndfile'] = conf.CheckPKG('sndfile >= 1.0.16')
            if not success: Exit(1)
            succes2, libraries['sndfile18'] = conf.CheckPKG('sndfile >= 1.0.18')
            if succes2:
                libraries['sndfile'].Append(CPPDEFINES = ['LIBSNDFILE_1018'])

    # libcurl
    success, libraries['libcurl'] = conf.CheckPKG('libcurl >= 7')
    if env['CURL'] and not success:
        print 'CURL option was set, but libcurl not found.'
        Exit(1)

    # FFTW
    libraries['fftwf'] = Environment()
    if env['FFTW']:
        success, libraries['fftwf'] = conf.CheckPKG('fftw3f')
        if success:
            libraries['fftwf'].Append(CPPDEFINES = ['SC_FFT_FFTW'])
        else:
            libraries['fftwf'].Append(CPPDEFINES = ['SC_FFT_NONE'])
    else:
        libraries['fftwf'].Append(CPPDEFINES = ['SC_FFT_NONE'])
    if PLATFORM == 'darwin':
        # needed for vector multiplication
        libraries['fftwf'] = Environment(LINKFLAGS = '-framework vecLib')
else:
    libraries['sndfile'] = get_new_pkg_env()
    libraries['fftwf'] = get_new_pkg_env()
    libraries['libcurl'] = get_new_pkg_env()

# audio api
if env['AUDIOAPI'] == 'jack':
    features['audioapi'] = 'Jack'
    success, libraries['audioapi'] = conf.CheckPKG('jack >= 0.100')
    if success:
        libraries['audioapi'].Append(
            CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_JACK')],
            ADDITIONAL_SOURCES = ['Source/server/SC_Jack.cpp']
            )
    else:
        success, libraries['audioapi'] = conf.CheckPKG('jack')
        if isDefaultBuild():
            if not success: Exit(1)
        else:
            libraries['audioapi'] = get_new_pkg_env()
        libraries['audioapi'].Append(
            CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_JACK'),
                          'SC_USE_JACK_CLIENT_NEW'],
            ADDITIONAL_SOURCES = ['Source/server/SC_Jack.cpp']
            )
    libraries['audioapi'].Append(
        CPPDEFINES = [('SC_JACK_USE_DLL', env['JACK_DLL']),
                      ('SC_JACK_DEBUG_DLL', env['JACK_DEBUG_DLL'])]
        )
elif env['AUDIOAPI'] == 'coreaudio':
    features['audioapi'] = 'CoreAudio'
    libraries['audioapi'] = Environment(
        CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_COREAUDIO')],
        LINKFLAGS = '-framework CoreAudio',
        ADDITIONAL_SOURCES = []
        )
elif env['AUDIOAPI'] == 'portaudio':
    features['audioapi'] = 'Portaudio'
    libraries['audioapi'] = Environment(
        CPPDEFINES = [('SC_AUDIO_API', 'SC_AUDIO_API_PORTAUDIO')],
        LIBS = ['portaudio'],
        ADDITIONAL_SOURCES = []
        )

# rendezvous
if env['RENDEZVOUS']:
    features['rendezvous'], libraries['rendezvous'] = conf.CheckPKG('avahi-client')
    if features['rendezvous']:
        libraries['rendezvous'].Append(CPPDEFINES = ['HAVE_AVAHI'])
    else:
        features['rendezvous'], libraries['rendezvous'] = conf.CheckPKG('howl')
        if features['rendezvous']:
            libraries['rendezvous'].Append(CPPDEFINES = ['HAVE_HOWL'])
else:
    features['rendezvous'] = False
    libraries['rendezvous'] = False

# alsa
if env['ALSA']:
    features['alsa'], libraries['alsa'] = conf.CheckPKG('alsa')
else:
    features['alsa'] = False

if features['alsa']:
    libraries['alsa'].Append(CPPDEFINES = ['HAVE_ALSA'])

# midi
if conf.CheckCHeader('/System/Library/Frameworks/CoreMIDI.framework/Headers/CoreMIDI.h'):
    features['midiapi'] = 'CoreMIDI'
    libraries['midiapi'] = Environment(
        LINKFLAGS = '-framework CoreMIDI',
        )
elif features['alsa']:
    features['midiapi'] = 'ALSA'
    libraries['midiapi'] = libraries['alsa'].Clone()
else:
    features['midiapi'] = None

# lid
features['lid'] = env['LID'] and conf.CheckCHeader('linux/input.h')

# wii on linux
if PLATFORM == 'linux':
    features['wii'] = env['WII'] and conf.CheckCHeader('cwiid.h')
else:
    features['wii'] = env['WII']

# libicu
if env['LANG']:
    if PLATFORM == 'darwin' or conf.CheckCHeader('unicode/uregex.h'):
        libraries['libicu'] = Environment(
             LINKFLAGS = '-licui18n -licuuc -licudata',
        )
    else:
        print "libicu not found"
        Exit(1)

# readline
if env['LANG'] and env['READLINE'] and env['GPL3']:
    if conf.CheckCHeader('readline/readline.h'):
        libraries['readline'] = Environment(
             LINKFLAGS = '-lreadline',
        )
    else:
        print "libreadline not found"
        Exit(1)

# only _one_ Configure context can be alive at a time
env = conf.Finish()

# altivec
if env['ALTIVEC']:
    if PLATFORM == 'darwin':
        altivec_flags = [ '-faltivec' ]
    else:
        altivec_flags = [ '-maltivec', '-mabi=altivec' ]
    libraries['altivec'] = env.Clone()
    libraries['altivec'].Append(
        CCFLAGS = altivec_flags,
        CPPDEFINES = [('SC_MEMORY_ALIGNMENT', 16)])
    altiConf = Configure(libraries['altivec'])
    features['altivec'] = altiConf.CheckCHeader('altivec.h')
    altiConf.Finish()
else:
    features['altivec'] = False

# sse
if env['SSE']:
    libraries['sse'] = env.Clone()
    libraries['sse'].Append(
        CCFLAGS = ['-msse', '-mfpmath=sse'],
        CPPDEFINES = [('SC_MEMORY_ALIGNMENT', 16)])
    if env['SSE2']:
        libraries['sse'].Append(
            CCFLAGS = ['-msse2'])

    sseConf = Configure(libraries['sse'])
    hasSSEHeader = sseConf.CheckCHeader('xmmintrin.h')
    if env['CROSSCOMPILE']:
        build_host_supports_sse = True
        print 'NOTICE: cross compiling for another platform: assuming SSE support'
    else:
        build_host_supports_sse = False
        #if CPU != 'ppc':
        if CPU not in [ 'ppc' , 'ppc64' ]:
           if PLATFORM == 'freebsd':
                machine_info = os.popen ("sysctl -a  hw.instruction_sse").read()[:-1]
                x86_flags = 'no'
                if "1" in [x for x in machine_info]:
                        build_host_supports_sse = True
                        x86_flags = 'sse'
           elif PLATFORM != 'darwin':
                flag_line = os.popen ("cat /proc/cpuinfo | grep '^flags'").read()[:-1]
                x86_flags = flag_line.split (": ")[1:][0].split ()
           else:
              machine_info = os.popen ("sysctl -a machdep.cpu").read()[:-1]
              x86_flags = machine_info.split()
           if "sse" in [x.lower() for x in x86_flags]:
              build_host_supports_sse = True
              print 'NOTICE: CPU has SSE support'
        else:
           print 'NOTICE: CPU does not have SSE support'
    features['sse'] = hasSSEHeader and build_host_supports_sse
    sseConf.Finish()
else:
    features['sse'] = False

if env['X11']:
#    features['x11'], libraries['x11'] = conf.CheckPKG('xt')
    if type(env['X11']) != types.StringType:
        if os.path.exists('/usr/X11R6'):
            env['X11'] = '/usr/X11R6'
        elif os.path.exists('/usr/lib/X11'):
            env['X11'] = '/usr/lib/X11'
        else: env['X11'] = '/usr'
    x11Env = Environment(
        CPPPATH = [os.path.join(env['X11'], 'include')],
        LIBPATH = [os.path.join(env['X11'], 'lib')])
    x11Conf = Configure(x11Env)
    features['x11'] = x11Conf.CheckCHeader('X11/Intrinsic.h') \
                      and x11Conf.CheckLib('X11', 'XQueryPointer')
    libraries['x11'] = x11Conf.Finish()
else:
    features['x11'] = False


opts.Save('scache.conf', env)
Help(opts.GenerateHelpText(env))

# defines and compiler flags

env.Append(
    CPPDEFINES = [ '_REENTRANT', PLATFORM_SYMBOL ],
    CCFLAGS = [ '-Wno-unknown-pragmas' ],
    CXXFLAGS = [ '-Wno-deprecated' ]
    )

# debugging flags
if env['DEBUG']:
    env.Append(CCFLAGS = '-g')
else:
    env.Append(
        CCFLAGS = make_opt_flags(env),
        CPPDEFINES = ['NDEBUG'])
    if env['STRIP']:
        env.Append(LINKFLAGS = "-Wl,-s")

# platform specific
if False: #PLATFORM == 'darwin':
    # universal binary support
    if env['UNIVERSAL']:
        archs = map(lambda x: ['-arch', x], ['ppc', 'i386'])
        env.Append(
            CCFLAGS = archs,
            LINKFLAGS = archs
        )

# vectorization
if features['altivec']:
    merge_lib_info(env, libraries['altivec'])
elif features['sse']:
    merge_lib_info(env, libraries['sse'])
else:
    env.AppendUnique(CPPDEFINES = [('SC_MEMORY_ALIGNMENT', 1)])

# ======================================================================
# Source/common
# ======================================================================

commonEnv = env.Clone()
commonEnv.Append(
    CPPPATH = ['#Headers/common',
               '#Headers/plugin_interface',
               '#Headers/server'],
    CCFLAGS = ['-fPIC'],
    CXXFLAGS = ['-fPIC']
    )

# strtod
conf = Configure(commonEnv)
if conf.CheckFunc('strtod'):
    commonEnv.Append(
        CPPDEFINES = 'HAVE_STRTOD'
        )
commonEnv = conf.Finish()


# ======================================================================
# back to common
# ======================================================================

commonSources = Split('''
Source/common/SC_AllocPool.cpp
Source/common/SC_DirUtils.cpp
Source/common/SC_Sem.cpp
Source/common/SC_StringBuffer.cpp
Source/common/SC_StringParser.cpp
Source/common/scsynthsend.cpp
Source/common/sc_popen.cpp
''')
if PLATFORM == 'darwin':
    commonSources += [
        'Source/common/SC_StandAloneInfo_Darwin.cpp'
        ]
if env['CURL']:
    commonEnv.Append(CPPDEFINES = ['HAVE_LIBCURL'])
    merge_lib_info(commonEnv, libraries['libcurl'])
libcommon = commonEnv.Library('build/common', commonSources)

#if env['amd64']:
    #commonSources32 = list(commonSources)
    #commonEnv32 = commonEnv.Clone()
    #commonEnv32.Append(
        #CCFLAGS = ['-m32'],
    #)
    #libcommon32 = commonEnv32.Library('build/common32', commonSources32)


# ======================================================================
# Source/server
# ======================================================================

serverEnv = env.Clone()
serverEnv.Append(
    CPPPATH = ['#Headers/common',
               '#Headers/plugin_interface',
               '#Headers/server'],
    CPPDEFINES = [('SC_PLUGIN_DIR', '\\"' + pkg_lib_dir(FINAL_PREFIX, 'plugins') + '\\"'), ('SC_PLUGIN_EXT', '\\"' + PLUGIN_EXT + '\\"')],
    LIBPATH = 'build')
libscsynthEnv = serverEnv.Clone(
    PKGCONFIG_NAME = 'libscsynth',
    PKGCONFIG_DESC = 'SuperCollider synthesis server library',
    PKGCONFIG_PREFIX = FINAL_PREFIX,
    PKGCONFIG_REQUIRES = make_pkgconfig_requires(libraries['sndfile'],
                                                 libraries['audioapi'],
                                                 libraries['rendezvous']),
    PKGCONFIG_LIBS = ['-lscsynth'],
#     PKGCONFIG_LIBS_PRIVATE = ['-lm', '-lpthread', '-ldl'],
    PKGCONFIG_CFLAGS = ['-D' + PLATFORM_SYMBOL, '-I${includedir}/common', '-I${includedir}/plugin_interface', '-I${includedir}/server']
    )

# platform specific

# functionality of libdl is included in libc on freebsd
if PLATFORM == 'freebsd':
    serverEnv.Append(LIBS = ['common', 'pthread'])
else:
    serverEnv.Append(LIBS = ['common', 'pthread', 'dl'])

if PLATFORM == 'darwin':
    serverEnv.Append(
                LINKFLAGS = [
                    '-framework', 'CoreServices'])
                    #'-dylib_install_name', FINAL_PREFIX + '/lib/libsclang.dylib'])
elif PLATFORM == 'linux':
    serverEnv.Append(
                CPPDEFINES = [('SC_PLUGIN_LOAD_SYM', '\\"load\\"')],
                LINKFLAGS = '-Wl,-rpath,' + FINAL_PREFIX + '/lib')

elif PLATFORM == 'freebsd':
    serverEnv.Append(CPPDEFINES = [('SC_PLUGIN_LOAD_SYM', '\\"load\\"')])

# required libraries
merge_lib_info(
    serverEnv,
    libraries['sndfile'], libraries['audioapi'])

# optional features
if features['rendezvous']:
    serverEnv.Append(CPPDEFINES = ['USE_RENDEZVOUS'])
    merge_lib_info(serverEnv, libraries['rendezvous'])

if env['CURL']:
    serverEnv.Append(CPPDEFINES = ['HAVE_LIBCURL'])
    merge_lib_info(serverEnv, libraries['libcurl'])

libscsynthSources = Split('''
Source/server/Rendezvous.cpp
Source/server/Samp.cpp
Source/server/SC_BufGen.cpp
Source/server/SC_Carbon.cpp
Source/server/SC_Complex.cpp
Source/server/SC_ComPort.cpp
Source/server/SC_CoreAudio.cpp
Source/server/SC_Dimension.cpp
Source/server/SC_Errors.cpp
Source/server/SC_Graph.cpp
Source/server/SC_GraphDef.cpp
Source/server/SC_Group.cpp
Source/server/SC_Lib_Cintf.cpp
Source/server/SC_Lib.cpp
Source/server/SC_MiscCmds.cpp
Source/server/SC_Node.cpp
Source/server/SC_Rate.cpp
Source/server/SC_SequencedCommand.cpp
Source/server/SC_Str4.cpp
Source/server/SC_SyncCondition.cpp
Source/server/SC_Unit.cpp
Source/server/SC_UnitDef.cpp
Source/server/SC_World.cpp
''') + libraries['audioapi']['ADDITIONAL_SOURCES']

scsynthSources = ['Source/server/scsynth_main.cpp']

libscsynth = serverEnv.SharedLibrary('build/scsynth', libscsynthSources)
env.Alias('install-programs', env.Install(lib_dir(INSTALL_PREFIX), [libscsynth]))

libscsynthStaticSources = libscsynthSources + make_static_objects(serverEnv, commonSources, "_libscsynthStatic");
libscsynthStatic = serverEnv.StaticLibrary('build/scsynth', libscsynthStaticSources)
env.Alias('install-programs', env.Install(lib_dir(INSTALL_PREFIX), [libscsynthStatic]))

scsynth = serverEnv.Program('build/scsynth', scsynthSources, LIBS = ['scsynth'])
env.Alias('install-programs', env.Install(bin_dir(INSTALL_PREFIX), [scsynth]))

# ======================================================================
# Source/plugins
# ======================================================================

pluginEnv = env.Clone(
    SHLIBPREFIX = '',
    SHLIBSUFFIX = PLUGIN_EXT
    )
pluginEnv.Append(
    CPPPATH = ['#Headers/common',
               '#Headers/plugin_interface',
               '#Headers/server'],
    PKGCONFIG_NAME = 'libscplugin',
    PKGCONFIG_DESC = 'SuperCollider synthesis plugin headers',
    PKGCONFIG_PREFIX = FINAL_PREFIX,
    #PKGCONFIG_REQUIRES = make_pkgconfig_requires(libraries['scsynth']),
    #PKGCONFIG_LIBS = [],
#     PKGCONFIG_LIBS_PRIVATE = ['-lm', '-lpthread', '-ldl'],
    PKGCONFIG_CFLAGS = ['-D' + PLATFORM_SYMBOL, '-I${includedir}/common', '-I${includedir}/plugin_interface', '-I${includedir}/server']
    )
if PLATFORM == 'darwin':
    pluginEnv['SHLINKFLAGS'] = '$LINKFLAGS -bundle -flat_namespace -undefined suppress'

if PLATFORM == 'freebsd':
    merge_lib_info(
        pluginEnv,
        libraries['sndfile'])


plugins = []

def make_plugin_target(name):
    return os.path.join('build', 'plugins', name)

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
TestUGens
TriggerUGens
UnaryOpUGens
GrainUGens
ReverbUGens
'''):
    plugins.append(
        pluginEnv.SharedLibrary(
        make_plugin_target(name), os.path.join('Source', 'plugins', name + '.cpp')))

# complex
complexEnv = pluginEnv.Clone()
complexSources = Split('Source/plugins/SCComplex.cpp')
complexEnv.SharedObject('Source/plugins/SCComplex.o', complexSources)

# fft ugens
fftEnv = pluginEnv.Clone()
fftSources = Split('Source/common/SC_fftlib.cpp Source/plugins/SCComplex.o')
merge_lib_info(fftEnv, libraries['fftwf'])
plugins.append(
    fftEnv.SharedLibrary(
    make_plugin_target('FFT_UGens'),
        ['Source/plugins/FFTInterfaceTable.cpp',
         'Source/plugins/FFT_UGens.cpp',
         'Source/plugins/PV_UGens.cpp',
	 'Source/plugins/PartitionedConvolution.cpp'] + fftSources))

plugins.append(
    fftEnv.SharedLibrary(
    make_plugin_target('PV_ThirdParty'),
    ['Source/plugins/Convolution.cpp',
     'Source/plugins/FFT2InterfaceTable.cpp',
     'Source/plugins/FeatureDetection.cpp',
     'Source/plugins/PV_ThirdParty.cpp'] + fftSources))

# fft 'unpacking' ugens
plugins.append(
    pluginEnv.SharedLibrary(
    make_plugin_target('UnpackFFTUGens'), ['Source/plugins/SCComplex.o', 'Source/plugins/UnpackFFTUGens.cpp']))

# machine listening ugens
# fft ugens
mlEnv = pluginEnv.Clone()
mlSources = Split('Source/plugins/ML.cpp Source/plugins/Loudness.cpp Source/plugins/BeatTrack.cpp Source/plugins/Onsets.cpp Source/plugins/onsetsds.c Source/plugins/KeyTrack.cpp Source/plugins/MFCC.cpp Source/plugins/SCComplex.o Source/plugins/BeatTrack2.cpp Source/plugins/ML_SpecStats.cpp')
plugins.append(
    mlEnv.SharedLibrary(
    make_plugin_target('ML_UGens'), mlSources))

# diskio ugens
if not env['NO_LIBSNDFILE']:  # (libsndfile needed for all of these, so skip if unavailable)
    diskIOEnv = pluginEnv.Clone(
        LIBS = ['common', 'm'],
        LIBPATH = 'build'
        )
    if PLATFORM == 'darwin':
        diskIOEnv.Append(
            LINKFLAGS = '-framework CoreServices'
            )

    diskIOSources = [
        diskIOEnv.SharedObject('Source/plugins/SC_SyncCondition', 'Source/server/SC_SyncCondition.cpp'),
        'Source/plugins/DiskIO_UGens.cpp']
    merge_lib_info(diskIOEnv, libraries['sndfile'])
    plugins.append(
        diskIOEnv.SharedLibrary(
        make_plugin_target('DiskIO_UGens'), diskIOSources))

# ui ugens
if PLATFORM == 'darwin':
    uiUGensEnv = pluginEnv.Clone(
        LIBS = 'm',
        LINKFLAGS = '-framework CoreServices -framework Carbon'
        )
    plugins.append(
        uiUGensEnv.SharedLibrary(make_plugin_target('MouseUGens'), 'Source/plugins/MouseUGens.cpp'))
    plugins.append(
        uiUGensEnv.SharedLibrary(make_plugin_target('KeyboardUGens'), 'Source/plugins/KeyboardUGens.cpp'))
elif features['x11']:
    uiUGensEnv = pluginEnv.Clone()
    merge_lib_info(uiUGensEnv, libraries['x11'])
    plugins.append(
        uiUGensEnv.SharedLibrary(make_plugin_target('MouseUGens'), 'Source/plugins/MouseUGens.cpp'))
    plugins.append(
        uiUGensEnv.SharedLibrary(make_plugin_target('KeyboardUGens'), 'Source/plugins/KeyboardUGens.cpp'))

env.Alias('install-plugins', env.Install(
    pkg_lib_dir(INSTALL_PREFIX, 'plugins'), plugins))

# ======================================================================
# Source/lang
# ======================================================================

if env['TERMINAL_CLIENT'] == True:
        env['TERMINAL_CLIENT'] = 1
else:
        env['TERMINAL_CLIENT'] = 0

langEnv = env.Clone()
langEnv.Append(
    CPPPATH = ['#Headers/common',
               '#Headers/plugin_interface',
               '#Headers/lang',
               '#Headers/server',
               '#Source/lang/LangSource/Bison'],
    CPPDEFINES = [['USE_SC_TERMINAL_CLIENT', env['TERMINAL_CLIENT']]],
    LIBPATH = 'build'
    )

if env.get('amd64') and not env.get('SCLANG64'):
    langEnv.Append( CFLAGS = ['-m32'],
                    CXXFLAGS = ['-m32'],
                    LINKFLAGS = ['-m32'],
                    CPPDEFINES = "NO_INTERNAL_SERVER")
else:
    langEnv.Append(LIBS = ['common', 'scsynth'])
    merge_lib_info(langEnv, libraries['audioapi'])

if PLATFORM == 'windows':
    langEnv.Append(CPPDEFINES = "NO_INTERNAL_SERVER")

# functionality of libdl is included in libc on freebsd
if PLATFORM == 'freebsd':
    langEnv.Append(LIBS = ['pthread', 'm'])
else:
    langEnv.Append(LIBS = ['pthread', 'dl',  'm'])

if PLATFORM == 'darwin':
    langEnv.Append(
                LIBS = ['icucore'],
                LINKFLAGS = ['-framework', 'CoreServices'], #'-dylib_install_name', FINAL_PREFIX + '/lib/libsclang.dylib'],
                CPPPATH = ['#xtralibs/include/icu/unicode'])
elif PLATFORM == 'linux':
    langEnv.Append(
        LINKFLAGS = '-Wl,-rpath,build -Wl,-rpath,' + FINAL_PREFIX + '/lib')

    if env.get('amd64') and not env.get('SCLANG64'):
        langEnv.Append(LIBPATH="/emul/ia32-linux/usr/lib/,/usr/lib32/",
                       LINKFLAGS = '-Wl,-rpath,/emul/ia32-linux/usr/lib/')

elif PLATFORM == 'freebsd':
    langEnv.Append(
    LINKFLAGS = '-Wl,-rpath,build -Wl,-rpath,' + FINAL_PREFIX + '/lib')

if env['CURL']:
    langEnv.Append(CPPDEFINES = ['HAVE_LIBCURL'])
    merge_lib_info(langEnv, libraries['libcurl'])

if env['READLINE'] and env['GPL3']:
    langEnv.Append(CPPDEFINES = ['HAVE_READLINE'])
    merge_lib_info(langEnv, libraries['readline'])


libsclangEnv = langEnv.Clone(
    PKGCONFIG_NAME = 'libsclang',
    PKGCONFIG_DESC = 'SuperCollider synthesis language library',
    PKGCONFIG_PREFIX = FINAL_PREFIX,
    PKGCONFIG_REQUIRES = make_pkgconfig_requires(libraries['sndfile']) + ['libscsynth'],
    PKGCONFIG_LIBS = ['-lsclang'],
    PKGCONFIG_CFLAGS = ['-D' + PLATFORM_SYMBOL, '-I${includedir}/common', '-I${includedir}/plugin_interface', '-I${includedir}/lang', '-I${includedir}/server']
    )

# required libraries
merge_lib_info(langEnv, libraries['sndfile'])

libsclangSources = Split('''
Source/lang/LangSource/AdvancingAllocPool.cpp
Source/lang/LangSource/ByteCodeArray.cpp
Source/lang/LangSource/DumpParseNode.cpp
Source/lang/LangSource/GC.cpp
Source/lang/LangSource/InitAlloc.cpp
Source/lang/LangSource/PyrFileUtils.cpp
Source/lang/LangSource/PyrInterpreter3.cpp
Source/lang/LangSource/PyrLexer.cpp
Source/lang/LangSource/PyrMathOps.cpp
Source/lang/LangSource/PyrMathSupport.cpp
Source/lang/LangSource/PyrMessage.cpp
Source/lang/LangSource/PyrObject.cpp
Source/lang/LangSource/PyrParseNode.cpp
Source/lang/LangSource/PyrSignal.cpp
Source/lang/LangSource/PyrSymbolTable.cpp
Source/lang/LangSource/SC_LanguageClient.cpp
Source/lang/LangSource/SC_LibraryConfig.cpp
Source/lang/LangSource/SC_TerminalClient.cpp
Source/lang/LangSource/Samp.cpp
Source/lang/LangSource/SimpleStack.cpp
Source/lang/LangSource/VMGlobals.cpp
Source/lang/LangSource/alloca.cpp
Source/lang/LangSource/dumpByteCodes.cpp
Source/lang/LangSource/Bison/lang11d_tab.cpp
Source/lang/LangPrimSource/SC_Wii.cpp
Source/lang/LangPrimSource/PyrSignalPrim.cpp
Source/lang/LangPrimSource/PyrSched.cpp
Source/lang/LangPrimSource/PyrPrimitive.cpp
Source/lang/LangPrimSource/PyrMathPrim.cpp
Source/lang/LangPrimSource/SC_ComPort.cpp
Source/lang/LangPrimSource/OSCData.cpp
Source/lang/LangPrimSource/PyrArchiver.cpp
Source/lang/LangPrimSource/PyrArrayPrimitives.cpp
Source/lang/LangPrimSource/PyrBitPrim.cpp
Source/lang/LangPrimSource/PyrCharPrim.cpp
Source/lang/LangPrimSource/PyrFilePrim.cpp
Source/lang/LangPrimSource/PyrListPrim.cpp
Source/lang/LangPrimSource/PyrPlatformPrim.cpp
Source/lang/LangPrimSource/PyrSerialPrim.cpp
Source/lang/LangPrimSource/PyrStringPrim.cpp
Source/lang/LangPrimSource/PyrUStringPrim.cpp
Source/lang/LangPrimSource/PyrSymbolPrim.cpp
Source/lang/LangPrimSource/PyrUnixPrim.cpp
Source/common/fftlib.c
''')

if env['LANG']:
    merge_lib_info(langEnv, libraries['libicu'])

# optional features
if features['midiapi']:
    merge_lib_info(langEnv, libraries['midiapi'])
    if features['midiapi'] == 'CoreMIDI':
                libsclangSources += ['Source/lang/LangPrimSource/SC_CoreMIDI.cpp']
    else:
                libsclangSources += ['Source/lang/LangPrimSource/SC_AlsaMIDI.cpp']
else:
    # fallback implementation
    libsclangSources += ['Source/lang/LangPrimSource/SC_AlsaMIDI.cpp']

if PLATFORM == 'darwin':
    langEnv.Append(
                CPPPATH = ['#Source/lang/LangPrimSource/HID_Utilities',
                                   '#Source/lang/LangPrimSource/WiiMote_OSX'],
                LINKFLAGS = '-framework Carbon -framework IOKit -framework IOBluetooth'
        )
    libsclangSources += Split('''
Source/lang/LangPrimSource/SC_HID.cpp
Source/lang/LangPrimSource/SC_CoreAudioPrim.cpp
Source/lang/LangPrimSource/HID_Utilities/HID_Error_Handler.c
Source/lang/LangPrimSource/HID_Utilities/HID_Name_Lookup.c
Source/lang/LangPrimSource/HID_Utilities/HID_Queue_Utilities.c
Source/lang/LangPrimSource/HID_Utilities/HID_Utilities.c
Source/lang/LangPrimSource/WiiMote_OSX/wiiremote.c
''')

if env.has_key('amd64') and env['amd64']:
    libsclangSources += commonSources

if PLATFORM == 'linux':
    # HAVE_LID does the right thing in SC_LID.cpp source
    libsclangSources += ['Source/lang/LangPrimSource/SC_LID.cpp']
    if features['wii']:
                langEnv.Append(CPPDEFINES = 'HAVE_WII')
                langEnv.Append(LINKFLAGS = '-lcwiid')
        #langEnv.Append(LINKFLAGS = '-lbluetooth')
        #langEnv.Append(CPPPATH = '-I/usr/local/include/libcwiimote-0.4.0/libcwiimote/' ) #FIXME: to proper include directory
    if features['lid']:
            langEnv.Append(CPPDEFINES = 'HAVE_LID')

if PLATFORM == 'darwin':
    langEnv.Append(CPPDEFINES = 'HAVE_SPEECH')
    libsclangSources += ['Source/lang/LangPrimSource/SC_Speech.cpp']

sclangSources = ['Source/lang/LangSource/cmdLineFuncs.cpp']

if env['LANG']:
    libsclang = langEnv.SharedLibrary('build/sclang', libsclangSources)
    env.Alias('install-bin', env.Install(lib_dir(INSTALL_PREFIX), [libsclang]))
    if PLATFORM == 'darwin':
        sclangLibs = ['scsynth', 'sclang']
    else:
        sclangLibs = ['sclang']
    sclang = langEnv.Program('build/sclang', sclangSources, LIBS=sclangLibs)
    env.Alias('install-programs', env.Install(bin_dir(INSTALL_PREFIX), [sclang]))

# ======================================================================
# doc/doxygen
# ======================================================================

# doxygen = env.Command(None, 'doc/doxygen/html/index.html', 'doxygen doc/doxygen/doxygen.cfg')
# env.Alias('doxygen', doxygen)

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
    # example files
    env.Alias('install-library', install_dir(
        env, 'build/examples',
        pkg_data_dir(INSTALL_PREFIX),
        HELP_FILE_RE, 1))
    # scel
    if env['SCEL']:
        env.Alias('install-library', install_dir(
            env, 'editors/scel/sc',
            pkg_extension_dir(INSTALL_PREFIX, 'scide_scel'),
            SC_FILE_RE, 3))
    # scvim
#    if env['SCVIM']:
#        env.Alias('install-library', install_dir(
#            env, 'editors/scvim/scclasses',
#            pkg_extension_dir(INSTALL_PREFIX, 'scvim'),
#            SC_FILE_RE, 3))
#    # scvim helpfiles
#    if env['SCVIM']:
#       env.Alias('install-library', install_dir(
#            env, 'editors/scvim/cache/doc',
#            pkg_data_dir(INSTALL_PREFIX, 'scvim-help'),
#            HELP_FILE_RE, 4))

#scvim : unhtml help files
#if env['SCVIM']:
        #os.execvpe("editors/scvim/bin/scvim_make_help", [ "-c", "-s", "build/Help"],"SCVIM=editors/scvim/")
    #os.popen ("cat /proc/cpuinfo | grep '^flags'").read()[:-1]

#if env['SCVIM']:
	#vimenv = env.Clone()
	#SConscript('editors/test/SConstruct', exports=['vimenv'])

#if env['SCVIM']:
   #print 'installing scvim'
   #vimenv = env.Clone()
   ##env.Append(FROMTOP=True)
   #SConscript('editors/scvim/SConstruct', exports=['vimenv'])

#### scvim and sced scripts need to be called independently, until someone fixes setting the proper variables from the parent script

if env['SCVIM']:
    print '----------------------------------------------------'
#   SConscript('editors/scvim/SConstruct', exports=['env'])
    print 'To install SCVIM, please use scons in the directory editors/scvim/'

if env['SCED']:
#   SConscript('editors/sced/SConstruct', 'env')
    print '----------------------------------------------------'
    print 'To install SCED, please use the scons in the directory editors/sced/'

# scel
if env['SCEL']:
    env.Command('editors/scel/el/sclang-vars.el', 'editors/scel/el/sclang-vars.el.in',
                'sed \'s,@PKG_DATA_DIR@,%s,g\' < $SOURCE > $TARGET' %
                pkg_data_dir(FINAL_PREFIX))
    el_files = glob.glob('editors/scel/el/*.el') + ['editors/scel/el/sclang-vars.el']
    elc_files = map(lambda f: os.path.splitext(f)[0] + '.elc', el_files)
    elisp_dir = os.path.join(INSTALL_PREFIX, 'share', 'emacs', 'site-lisp')
    env.Command(elc_files, el_files,
                'emacs -batch --eval "(add-to-list \'load-path (expand-file-name \\"editors/scel/el/\\"))" -f batch-byte-compile $SOURCES')
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
            env, os.path.join('Headers', d),
            pkg_include_dir(INSTALL_PREFIX),
            re.compile('.*\.h(h|pp)?'), 1)
            )
    # other useful headers
    env.Alias('install-dev',
              env.Install(pkg_include_dir(INSTALL_PREFIX, 'plugin_interface'), 'Source/plugins/FFT_UGens.h'))
    pkgconfig_files = [
        libscsynthEnv.Command('linux/libscsynth.pc', 'SConstruct',
                              build_pkgconfig_file),
        libsclangEnv.Command('linux/libsclang.pc', 'SConstruct',
                             build_pkgconfig_file),
        pluginEnv.Command('linux/libscplugin.pc', 'SConstruct',
                             build_pkgconfig_file)]
    pkgconfig_dir = os.path.join(lib_dir(INSTALL_PREFIX), 'pkgconfig')
    env.Alias('install-dev', env.Install(pkgconfig_dir, pkgconfig_files))
    installEnv.Append(ALL = 'install-dev')

# documentation
if is_installing():
    # TODO: build html documentation?
    doc_dir = pkg_doc_dir(INSTALL_PREFIX)
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
clean-compile.sh
compile.sh
distro
linux/examples/onetwoonetwo.sc
linux/examples/sclang.cfg.in
linux/examples/sclang.sc
editors/scel/README
editors/scvim/README
editors/sced/README
scsynthlib_exp
''')

DIST_SPECS = [
    ('build', HELP_FILE_RE),
    ('Headers', SRC_FILE_RE),
    ('editors/scel/sc', SC_FILE_RE),
    ('editors/scel/el', re.compile('.*\.el$')),
    ('editors/scvim/scclasses', SC_FILE_RE),
    ('Source', SRC_FILE_RE)
    ]

def dist_paths():
    paths = DIST_FILES[:]
    specs = DIST_SPECS[:]
    while specs:
        base, re = specs.pop()
        if not re: re = ANY_FILE_RE
        for root, dirs, files in os.walk(base):
            if 'CVS' in dirs: dirs.remove('CVS')
            if '.svn' in dirs: dirs.remove('.svn')
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

if 'scrub' in COMMAND_LINE_TARGETS:
    env.Clean('scrub', Split('config.log scache.conf .sconf_temp'))

# ======================================================================
# configuration summary
# ======================================================================

def yesorno(p):
    if p: return 'yes'
    else: return 'no'

print '------------------------------------------------------------------------'
print ' ALTIVEC:                 %s' % yesorno(features['altivec'])
print ' AUDIOAPI:                %s' % features['audioapi']
print ' MIDIAPI:                 %s' % features['midiapi']
print ' DEBUG:                   %s' % yesorno(env['DEBUG'])
# print ' DESTDIR:                 %s' % env['DESTDIR']
print ' DEVELOPMENT:             %s' % yesorno(env['DEVELOPMENT'])
print ' LANG:                    %s' % yesorno(env['LANG'])
print ' LID:                     %s' % yesorno(features['lid'])
print ' NO_LIBSNDFILE:           %s' % yesorno(env['NO_LIBSNDFILE'])
print ' WII:                     %s' % yesorno(features['wii'])
print ' PREFIX:                  %s' % env['PREFIX']
print ' RENDEZVOUS:              %s' % yesorno(features['rendezvous'])
print ' SCEL:                    %s' % yesorno(env['SCEL'])
print ' SCVIM:                   %s' % yesorno(env['SCVIM'])
print ' SCED:                    %s' % yesorno(env['SCED'])
print ' SSE:                     %s' % yesorno(features['sse'])
print ' STRIP:                   %s' % yesorno(env['STRIP'])
print ' CROSSCOMPILE:            %s' % yesorno(env['CROSSCOMPILE'])
print ' TERMINAL_CLIENT:         %s' % yesorno(env['TERMINAL_CLIENT'])
print ' X11:                     %s' % yesorno(features['x11'])
print ' GPL3:                    %s' % yesorno(env['GPL3'])
print ' SCLANG64:                %s' % yesorno(env.get('SCLANG64'))
print '------------------------------------------------------------------------'

# ======================================================================
# EOF

