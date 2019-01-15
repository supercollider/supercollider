# Brian Heim
# 2018-08-26
#
# See accompanying README for info.

import argparse
import os
import sys
import shutil
import getpass
import re
import datetime
from functools import reduce

# Argument parsing
parser = argparse.ArgumentParser()
parser.add_argument('root', nargs = '?', help = "Root project directory, where CMakeLists.txt will be written. Defaults to current working directory", default = '.')
parser.add_argument('-p', '--plugins', nargs = '+', help = "Plugin subproject directories, for example -p Sin Saw Square")
parser.add_argument('-P', '--project-name', help = "Name of the project. Defaults to the name of the root project directory")
parser.add_argument('-c', '--min-cmake-version', help = "Mininum CMake version to require.  Defaults to 3.5, must be >= 3.5", default = '3.5')
parser.add_argument('-s', '--cpp-standard', help = "C++ standard to use. Defaults to C++11.  Valid arguments are 11, 14, 17.", type = int, default = 11)
parser.add_argument('-v', '--verbose', help = "Verbose output. Can be specified up to 2 times for more verbosity", action = 'count', default = 0)
parser.add_argument('-a', '--author', help = "Project author name.")
parser.add_argument('-D', '--date', help = "Project date. Defaults to now.")
parser.add_argument('-f', '--flat-dir', help = "Use flat-directory configuration. See README for more info.")
parser.add_argument('-C', '--sc-files-dir', help = "For use with flat-directory configuration.  Directory where SC files can be found. Defaults to --flat-dir argument.")
parser.add_argument('-H', '--schelp-files-dir', help = "For use with flat-directory configuration.  Directory where schelp files can be found. Defaults to --flat-dir argument.")
parser.add_argument('-I', '--install-cmake', action = 'store_true',
        help = "Install CMake modules after running. root/cmake_modules is created if necessary.")
parser.add_argument('--install-cmake-only', action = 'store_true',
        help = "Install CMake modules without running generation script.")

MODULES = [
    'SuperColliderCompilerConfig.cmake',
    'SuperColliderServerPlugin.cmake'
];
HEADER_TEMPLATE = 'CMakeLists_header.template'
FOOTER_TEMPLATE = 'CMakeLists_footer.template'
TARGET_TEMPLATE_TEXT = '''
####################################################################################################
# Begin target {name}

set({name}_cpp_files
{cpp})
set({name}_sc_files
{sc})
set({name}_schelp_files
{schelp})

sc_add_server_plugin(
    "{project}/{name}" # desination directory
    "{name}" # target name
    "${{{name}_cpp_files}}"
    "${{{name}_sc_files}}"
    "${{{name}_schelp_files}}"
)

# End target {name}
####################################################################################################
'''

# script output
DEBUG = 2
VERBOSE = 1
NORMAL = 0
WARNING = -1
ERROR = -2

verbosity_ = 0

def out(s, level):
    if verbosity_ >= level:
        if level == WARNING:
            print('WARNING: ' + s)
        elif level == ERROR:
            print('ERROR: ' + s)
        else:
            print(s)

# convenience FP utils
def apply(f, l):
    for x in l:
        f(x)

def mapnow(f, l):
    return [f(x) for x in l]

def filternow(f, l):
    return list(filter(f, l))

def zip2now(a, b):
    return list(zip(a, b))

# main generation utility class
class Generator:
    def __init__(self, args):
        global verbosity_
        verbosity_ = args.verbose
        self.args = args
        self.script_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
        self.root_dir = os.path.realpath(args.root)
        self.project_name_spaceless = self.args.project_name.replace(' ', '')
        if not args_.install_cmake_only:
            self.validate()

    def validate(self):
        flatmode = self.args.flat_dir or self.args.sc_files_dir or self.args.schelp_files_dir
        if flatmode and self.args.plugins:
            out('Cannot specify both flat-dir mode and individual plugin directories', ERROR)
            exit(1)

        if not flatmode:
            if not self.args.plugins:
                out('Must specify one of -p or -f', ERROR)
                parser.print_help()
                exit(1)
            else:
                for plugin in self.args.plugins:
                    plugin_path = os.path.join(self.root_dir, plugin)
                    if not os.path.exists(plugin_path):
                        out('Config: Plugin directory does not exist: {}'.format(plugin_path), ERROR)
                        exit(1)

        else:
            self.validate_flatmode()

        out('Config: Directory of this script: ' + self.script_dir, DEBUG)
        out('Config: Absolute path of root directory: ' + self.root_dir, DEBUG)

        if not self.args.cpp_standard in [11, 14, 17]:
            out('Invalid C++ standard given.', ERROR)
            exit(1)
        if not os.path.exists(self.root_dir):
            out('Root project directory does not exist: {}'.format(self.root_dir), ERROR)
            exit(1)

        if not int(self.args.min_cmake_version[0:1]) == 3 or not float(self.args.min_cmake_version[2:]) >= 5.0:
            out('CMake version must be >= 3.5, got {}'.format(self.args.min_cmake_version))
            exit(1)
        if not self.args.author:
            user = Generator.get_username()
            out('Config: No author given, using user profile name: {}'.format(user), NORMAL)
            self.args.author = user
        if not self.args.date:
            date = '{:%Y-%m-%d}'.format(datetime.datetime.now())
            out('Config: No date given, using now: {}'.format(date), VERBOSE)
            self.args.date = date

        if not self.args.project_name:
            self.args.project_name = os.path.split(self.root_dir)[1]
            out('Config: No project name given, using parent of root dirctory: {}'.format(self.args.project_name), VERBOSE)

    def validate_flatmode(self):
        if not self.args.sc_files_dir:
            self.args.sc_files_dir = self.args.flat_dir
        if not self.args.schelp_files_dir:
            self.args.schelp_files_dir = self.args.flat_dir

        self.full_flat_dir = os.path.join(self.root_dir, self.args.flat_dir)
        self.full_sc_files_dir = os.path.join(self.root_dir, self.args.sc_files_dir)
        self.full_schelp_files_dir = os.path.join(self.root_dir, self.args.schelp_files_dir)
        out('Config: sc_files_dir is {}'.format(self.full_sc_files_dir), VERBOSE)
        out('Config: schelp_files_dir is {}'.format(self.full_schelp_files_dir), VERBOSE)
        def check_exists(name, dir):
            if not os.path.exists(dir):
                out('{} does not exist: {}'.format(name, dir), ERROR)
                exit(1)

        check_exists('--flat-dir', self.full_flat_dir)
        check_exists('--sc-files-dir', self.full_sc_files_dir)
        check_exists('--schelp-files-dir', self.full_schelp_files_dir)


    # main generation routine
    def generate_cmake(self):
        if self.args.flat_dir:
            targets, project_files = self.find_project_files_flat(self.full_flat_dir)
        else:
            targets, project_files = self.find_project_files(self.args.plugins)
        out('Targets: {}; project_files: {}'.format(targets, project_files), DEBUG)
        result = self.gen_section('header', HEADER_TEMPLATE)
        result += self.gen_targets(targets, project_files)
        result += self.gen_section('footer', FOOTER_TEMPLATE)
        self.write_cmake_file(result)

    def gen_section(self, name, filename):
        out('Generating {}'.format(name), VERBOSE)
        path = os.path.join(self.script_dir, filename)
        out('Reading from {} file {}'.format(name, path), DEBUG)
        with open(path, mode='r') as f:
            section = ''.join(f.readlines())
        out('Configuring {}'.format(name), DEBUG)
        return self.configure_text(section)

    def write_cmake_file(self, text):
        cmake_file = os.path.join(self.root_dir, 'CMakeLists.txt')
        out('Writing CMake file'.format(cmake_file), DEBUG)
        with open(cmake_file, mode='w') as f:
            f.write(text)
        out('Wrote project file to {}'.format(cmake_file), NORMAL)

    def gen_targets(self, plugins, project_files):
        out('Generating targets text', VERBOSE)
        return reduce(lambda x, y: x + y,
            map(lambda p: self.gen_target_cmake(p,
                project_files[p]['cpp'],
                project_files[p]['sc'],
                project_files[p]['schelp']
                ),
                plugins
            )
        )

    # replace '@xyz' variables in template text with script arguments
    def configure_text(self, text):
        envir = [
            ('project_name', self.args.project_name),
            ('author', self.args.author),
            ('minimum_cmake_version', self.args.min_cmake_version),
            ('cpp_standard', str(self.args.cpp_standard)),
            ('date', self.args.date)
        ]

        out('Configure environment: {}'.format(envir), DEBUG)

        def op(text, pair):
            k, v = pair
            k = '@' + k
            out('\tReplacement: {} -> {}'.format(k, v, text), DEBUG)
            return re.sub(k, v, text)

        return reduce(op, envir, text)

    def gen_target_cmake(self, name, cpp_files, sc_files, schelp_files):
        out('\tGenerating CMake for plugin {}'.format(name), VERBOSE)
        result = TARGET_TEMPLATE_TEXT.format(
            project=self.project_name_spaceless,
            name=name,
            cpp=self.join_file_list(cpp_files),
            sc=self.join_file_list(sc_files),
            schelp=self.join_file_list(schelp_files)
        )

        return result

    # helper
    def join_file_list(self, files):
        if files:
            # CMake generally likes / more than \
            return ''.join(map(lambda x: '    ' + x.replace('\\', '/') + '\n', files))
        else:
            return ''

    # return type is a tuple of plugin names and a map from plugin name to a map from 'sc', 'cpp',
    # 'schelp' to the relevant files for that target. 'cpp' will map to both header and
    # implementation files.
    def find_project_files(self, dirs):
        out('Scanning for project files.', VERBOSE)
        out('Directories: {}'.format(dirs), DEBUG)

        # split will return (path, '') if path ends in dir separator
        def get_real_parent(path):
            head, tail = os.path.split(path)
            if tail == '':
                return os.path.split(head)[1]
            else:
                return tail

        plugin_names = mapnow(get_real_parent, dirs)
        result = dict()
        for d, p in zip(dirs, plugin_names):
            result[p] = self.find_project_files_impl(d)

        return (plugin_names, result)

    # Helper for find_project_files
    def find_project_files_impl(self, dir):
        dirmap = dict(cpp=[], sc=[], schelp=[])
        d = os.path.join(self.root_dir, dir)
        out('Current directory: {}'.format(d), DEBUG)

        def classify(f):
            ty = Generator.classify_file_type(f)
            out('\tClassified {} as {}'.format(f, ty), DEBUG)
            if ty:
                fullpath = os.path.join(root, f)
                relpath = os.path.relpath(fullpath, self.root_dir)
                out('\tIncluding {} as a {} file'.format(relpath, ty), VERBOSE)
                dirmap[ty].append(relpath)

        for root, _, files in os.walk(d):
            apply(classify, files)

        return dirmap

    # Gather all .cpp files in a directory, absolute paths
    def get_cpp_files(self, dir):
        entries = os.listdir(dir)
        cppfiles = zip2now(entries, map(lambda f: os.path.join(dir, f), entries))
        cppfiles = filternow(lambda f: os.path.isfile(f[1]), cppfiles)
        cppfiles = filternow(lambda f: Generator.is_cpp_file(f[1]), cppfiles)
        cppfiles = sorted(cppfiles)
        return cppfiles

    # Same as find_project_files, but with flat directories
    def find_project_files_flat(self, fulldir):
        out('Scanning for project files in dir: {}'.format(fulldir), VERBOSE)
        result = dict()
        plugins = []
        cppfiles = self.get_cpp_files(fulldir)

        def scan_for_filetype(dir, pluginname, ext, test):
            out('Scanning for matching .{} files in dir: {}'.format(ext, dir), DEBUG)
            for root, _, files in os.walk(dir):
                for file in files:
                    if test(file) and os.path.splitext(file)[0] == pluginname:
                        relpath = os.path.relpath(os.path.join(root, file), self.root_dir)
                        out('\tIncluding {} in target'.format(relpath), VERBOSE)
                        result[pluginname][ext].append(relpath)

        for fname, fpath in cppfiles:
            pluginname, _ = os.path.splitext(fname)
            out('Found plugin {}'.format(pluginname), NORMAL)
            plugins.append(pluginname)

            result[pluginname] = dict(cpp=[], sc=[], schelp=[])
            relpath = os.path.relpath(fpath, self.root_dir)
            out('\tIncluding {} as a cpp file'.format(relpath), DEBUG)
            result[pluginname]['cpp'].append(relpath)

            scan_for_filetype(self.full_flat_dir, pluginname, 'cpp', Generator.is_hpp_file)

        # Find all [sc, schelp] files in dir. Match and remove files that share a name with known
        # targets.  Generate warnings for the remaining files; as this signals some plugins may not
        # have been found.
        def find_matching_files(dir, ext, test):
            extfiles = []
            for root, _, files in os.walk(dir):
                extfiles.extend(mapnow(lambda f: os.path.join(root, f), filternow(test, files)))

            for plugin in plugins:
                extname = plugin + '.' + ext
                for extfile in extfiles:
                    if os.path.split(extfile)[1] == extname:
                        relpath = os.path.relpath(extfile, self.root_dir)
                        out('\tIncluding {} in target'.format(relpath), VERBOSE)
                        result[plugin][ext].append(relpath)
                        extfiles.remove(extfile)
                        break

            for leftover in extfiles:
                out('File {} was not included in any target'.format(leftover), WARNING)

        find_matching_files(self.full_sc_files_dir, 'sc', Generator.is_sc_file)
        find_matching_files(self.full_schelp_files_dir, 'schelp', Generator.is_schelp_file)

        return (plugins, result)

    # filetype tests
    @staticmethod
    def is_cpp_file(name):
        return any(map(lambda x: name.endswith(x), ['.cpp', '.cxx', '.c']))

    @staticmethod
    def is_hpp_file(name):
        return any(map(lambda x: name.endswith(x), ['.hpp', '.hxx', '.h']))

    @staticmethod
    def is_sc_file(name):
        return name.endswith('.sc')

    @staticmethod
    def is_schelp_file(name):
        return name.endswith('.schelp')

    @staticmethod
    def classify_file_type(name):
        if Generator.is_sc_file(name):
            return 'sc'
        if Generator.is_schelp_file(name):
            return 'schelp'
        if Generator.is_cpp_file(name) or Generator.is_hpp_file(name):
            return 'cpp'

        return None

    @staticmethod
    def get_username():
        return getpass.getuser()

    def install_modules(self):
        out('Installing CMake modules', DEBUG)
        modules_dir = os.path.join(g.root_dir, 'cmake_modules')

        try:
            os.mkdir(modules_dir)
            out('Created cmake_modules directory {}.'.format(modules_dir), DEBUG)
        except FileExistsError:
            out('cmake_modules directory {} already existed'.format(modules_dir), DEBUG)

        for module in MODULES:
            pfrom = os.path.join(self.script_dir, module)
            if not os.path.exists(pfrom):
                out('Module does not exist: {}'.format(pfrom), ERROR)
                continue

            out('Installing {} to {}'.format(pfrom, modules_dir), VERBOSE)
            shutil.copy(pfrom, modules_dir)

        out('Installed {} CMake modules'.format(len(MODULES)), NORMAL)

if __name__ == "__main__":
    args_ = parser.parse_args()
    g = Generator(args_)
    if not args_.install_cmake_only:
        g.generate_cmake()

    if args_.install_cmake or args_.install_cmake_only:
        g.install_modules()
