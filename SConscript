# -*- coding: utf-8 -*-
#
# SCons build system
# Copyright (C) 2005, 2006, 2007, 2008, 2009 Tim Blechmann
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

import os, os.path
import stat, datetime, time

env = Environment()

SConsignFile()

env.Tool('gch', toolpath='.')
env.Tool('combiner', toolpath='.')

######################################################################
#
# general configuration

opt = Options('options.cache')
opt.AddOptions(
    BoolOption('simd', 'Build with simd instructions', True),
    PathOption('cache', 'Path to scons\'s compile cache (empty string to disable)', None, lambda key, val, env: val == '' or PathOption.PathIsDirCreate(key, val, env)),
    BoolOption('compact_build', 'combine source files in build.cpp files (don\'t do this, if you have less than 1gb of ram)', False),
    PathOption('extra_path', 'extra search path', None),
    ('python_version', 'version of Python', None),
    BoolOption('optimize_testsuite', 'Optimize testsuite', False),
    BoolOption('native_jack', 'Use native jack backend', True),
    PathOption('prefix', 'Install prefix', '/usr/local'),
    )

opt.Update(env)

opt.Save('options.cache',env)
Help(opt.GenerateHelpText(env))

env['CPPPATH'] = list()



######################################################################
#
# general system specific configuration

if env.has_key('extra_path'):
    env.Append(LIBPATH=[env['extra_path']+"/lib"])
    env.Append(CPPPATH=[env['extra_path']+"/include"])

custom_checks = SConscript("SConscript.custom_checks")
conf = env.Configure(custom_tests = custom_checks)

check = True

check &= conf.CheckLib('pthread')
check &= conf.CheckLib('sndfile', language="C++")

## if not conf.CheckEndiannessDefined():
##     conf.DefineEndianness()

if not check:
    print "Configure checks failed"
    Exit(1)

if conf.CheckLib('tcmalloc'):
    print "Using google's tcmalloc"

if conf.CheckLibWithHeader('jack', 'jack/jack.h', language = 'C'):
    env.Append(CPPDEFINES="JACK_BACKEND")
elif (conf.CheckLibWithHeader('portaudio', 'portaudio.h', language='C') and
    conf.CheckLibWithHeader('portaudiocpp', 'portaudiocpp/PortAudioCpp.hxx', language='C++') ):
    env.Append(CPPDEFINES="PORTAUDIO_BACKEND")
    env.Append(CPPDEFINES="HAVE_PORTAUDIO")
    if conf.CheckHeader("portaudio/portaudio_config.h"):
        env.Append(CPPDEFINES=["HAVE_PORTAUDIO_CONFIG_H"])

else:
    print "portaudio missing"
    Exit(1)

conf.Finish()

######################################################################
#
# system specific configuration

if env['PLATFORM'] == 'posix':
    SConscript('SConscript.linux', exports={'env': env, 'env_specific': False})

SConscript("libs/SConscript.boost", exports={'env': env, 'env_specific': False})

debug_env = env.Clone()
release_env = env.Clone()

release_env['debug'] = False
debug_env['debug'] = True

environments = (release_env, debug_env)

env_specific = True
for env in environments:
    if env['PLATFORM'] == 'posix':
        SConscript('SConscript.linux', "env env_specific")
    elif env['PLATFORM'] == 'win32':
        env = SConscript('SConscript.win32', "env")
    elif env['PLATFORM'] == 'darwin':
        env = SConscript('SConscript.osx', "env")
    else:
        print "platform not supported"
        Exit(1)

    ######################################################################
    #
    # some preprocessor defines
    #
    if env.get('debug', False):
        env.Append(CPPDEFINES=Split("DEBUG"))
        if env.get('debug_stl', False):
            env.Append(CPPDEFINES=Split("_GLIBCXX_DEBUG"))
    else:
        env.Append(CPPDEFINES=Split("NDEBUG"))


######################################################################
#
# build nova
#
BuildDir('release_server', 'source', duplicate=0)
BuildDir('debug_server', 'source', duplicate=0)

release_server = \
         SConscript("source/SConscript", exports={'env':release_env, 'prog':'server'}, build_dir="release_server")

debug_server = \
         SConscript("source/SConscript", exports={'env':debug_env, 'prog':'server'}, build_dir="debug_server")


server = [release_server, debug_server]

######################################################################
#
# build sc plugins
#
BuildDir('debug_plugins', 'source/sc')
BuildDir('release_plugins', 'source/sc')
debug_plugins = env.SConscript("source/sc/SConscript", exports={'env':debug_env}, build_dir="debug_plugins")
release_plugins = env.SConscript("source/sc/SConscript", exports={'env':release_env}, build_dir="release_plugins")

Alias("plugins", debug_plugins + release_plugins)


######################################################################
#
# build testsuite
#

BuildDir('test', 'testsuite', duplicate=0)
testsuite = env.SConscript("./testsuite/SConscript", "env", build_dir="test")


######################################################################
#
# install
#

installs = [
    Install(Dir("bin", env['prefix']), release_server),
    Install(Dir("lib/supernova/plugins", env['prefix']), [x for x in release_plugins]),
    Install(Dir("share/SuperCollider/SCClassLibrary/Common/Control/", env['prefix']),
            "source/sc/supercollider/build/SCClassLibrary/Common/Control/PGroup.sc")
    ]

Alias('install', installs)

ret = dict()

ret['CPPPATH'] = env['CPPPATH']
ret['CPPDEFINES'] = env['CPPDEFINES']
ret['CCFLAGS'] = env['CCFLAGS']
ret['LINKFLAGS'] = env['LINKFLAGS']
ret['LIBS'] = env['LIBS']
ret['env'] = env
ret['testsuite'] = testsuite
ret['server'] = server

Return("ret")
