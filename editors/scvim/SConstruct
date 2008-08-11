# -*- python -*- =======================================================
# FILE:         SConstruct
# CONTENTS:     scons build script for scvim
# AUTHOR:       alex AT x37v DOT info
# AUTHOR:       sk AT k-hornz DOT de (some of this SConstruct taken from sk's SConstruct for SuperCollider)
# ======================================================================

import os
import re
import pwd

PLATFORM = os.uname()[0].lower()

SC_FILE_RE = re.compile('.*\.sc$')
VIM_FILE_RE = re.compile('.*\.vim$')
ANY_FILE_RE = re.compile('.*')
HOME_DIR_RE = re.compile(os.environ.get('HOME') + '.*')

DEFAULT_PREFIX = '/usr/local/'
DEFAULT_SC_HELP = os.path.join(DEFAULT_PREFIX, 'share/SuperCollider/Help/')

def pkg_data_dir(prefix, *args):
    if PLATFORM == 'darwin':
        base = '/Library/Application Support'
        if is_home_directory(prefix):
            base = os.path.join(prefix, base)
    else:
        base = os.path.join(prefix, 'share')
    return os.path.join(base, 'SuperCollider', *args)

def pkg_extension_dir(prefix, *args):
    return pkg_data_dir(prefix, 'Extensions', *args)

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

def bin_dir(prefix):
    return os.path.join(prefix, 'bin')

def share_dir(prefix):
    return os.path.join(prefix, 'share')

def in_home_directory(dir):
	return HOME_DIR_RE.match(dir)

#build help, the help depends on having the home directory enviroment variable.. that could change
build_help = Builder(action = 
		'export HOME=' + os.environ.get('HOME') + ' && ruby bin/scvim_make_help.rb -f -s $SOURCE -d $TARGET')

#create our options
opts = Options()
opts.Add(PathOption('PREFIX', 'Specify the prefix for the bin and share files', 
	DEFAULT_PREFIX, PathOption.PathIsDir))
opts.Add(PathOption('CACHE_DIR', 
		'Specify the dir for scvim help docs, syntax highlighting, etc', 
		os.path.join(os.environ.get('HOME', '/'), '.scvim/'), PathOption.PathAccept))
opts.Add(PathOption('VIMFILE_DIR', 
		'Specify the dir for vim files, these need to be in vim\'s runtime path',
		os.path.join(os.environ.get('HOME', '/'), '.vim/'), PathOption.PathAccept))
opts.Add(PathOption('SUPERCOLLIDER_HELP_DIR', 
	'Specify the location of SuperCollider\'s help', 
	DEFAULT_SC_HELP, PathOption.PathIsDir))

#create our enviroment, with our options and custom builders
env = Environment(options = opts, BUILDERS = {'BuildHelp' : build_help})

#generate the help options
Help(opts.GenerateHelpText(env))

#XXX how do we test for vim?
if not env.GetOption('clean'):
	#test for the ruby
	if os.system("ruby -e exit") != 0:
		print "error: scvim depends on ruby and the ruby executable is not in your path"
		Exit(1)

#get our variables from the options
PREFIX = env.get('PREFIX')
VIMFILE_DIR = env.get('VIMFILE_DIR')
CACHE_DIR = env.get('CACHE_DIR')
SUPERCOLLIDER_HELP_DIR = env.get('SUPERCOLLIDER_HELP_DIR')
DOC_DIR = os.path.join(CACHE_DIR, 'doc/')

install_vimfiles = env.Alias('install-vimfiles', [
	install_dir( env, 'ftplugin', VIMFILE_DIR, VIM_FILE_RE, 0),
	install_dir( env, 'syntax', VIMFILE_DIR, VIM_FILE_RE, 0),
	install_dir( env, 'indent', VIMFILE_DIR, VIM_FILE_RE, 0)],
	#could chown here, if in user's homedir?
	)

install_doc = env.Alias('install-doc', env.Install(DOC_DIR, ['SCVim.scd']))
build_help = env.Alias('build-help', env.BuildHelp(target = Dir(DOC_DIR), 
	source = Dir(SUPERCOLLIDER_HELP_DIR)))
AlwaysBuild(Dir(DOC_DIR))
Depends(build_help, install_doc)

install_bin = env.Alias('install-bin', 
		install_dir(env, 'bin', bin_dir(PREFIX), ANY_FILE_RE, 1))

install_rc = env.Alias('install-rc', env.Install(os.path.join(share_dir(PREFIX), 'scvim/'), ['scvimrc']))

install_classes = env.Alias('install-classes', install_dir(
   env, 'scclasses',
   pkg_extension_dir(PREFIX, 'scvim'),
   SC_FILE_RE, 1))

env.Alias('install-all', [
	install_bin,
	install_vimfiles,
	install_doc,
	install_rc,
	install_classes])

#this should be commonly run by the user who wishes to use it if the CACHE_DIR
# and VIMFILE_DIR is in the user's homedir
env.Alias('install-user', [install_vimfiles, install_doc])
#this should normally be run with sudo privileges
env.Alias('install-system', [install_bin, install_rc, install_classes])

#for x in env.Glob('*', ondisk = False, strings = False, source = False):
#	print x.get_abspath()
