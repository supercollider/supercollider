# Run clang-format easily on all the relevant source files in the project
# Brian Heim
# 2018-09-11

import get_source_files
import sys
import os
from subprocess import check_call
from subprocess import DEVNULL

def parse_args():
    from argparse import ArgumentParser
    ap = ArgumentParser()
    ap.add_argument('-r', '--root', help='Project root. Defaults to current directory', default='.')
    ap.add_argument('-q', '--quiet',
        help='Do not print output of clang-format',
        action='store_true')
    ap.add_argument('-v', '--verbose',
        help='Get verbose output from clang-format. This overrides --quiet.',
        action='store_true')
    ap.add_argument('files', metavar='FILE', nargs='*',
        help=('Specific files to format. If no files are provided, all source files in the project'
              ' will be formatted. This list will be filtered so that only real source files and'
              ' non-tool-generated files will be reformatted.'))

    return ap.parse_args()

def run_clang_format(files, quiet, verbose):
    args = ['clang-format', '-i'] + list(files)
    if verbose:
        args += ['-verbose']
        stdout = sys.stdout
        stderr = sys.stderr
    elif quiet:
        stdout = stderr = DEVNULL
    else:
        stdout = stderr = None

    check_call(
        args,
        stdout=stdout,
        stderr=stderr)

if __name__ == "__main__":
    args = parse_args()
    get_source_files.check_root(args.root)

    if not os.path.exists(os.path.join(args.root, get_source_files.SOURCE_DIRS[0])):
        print('Project files not found at given root.')

    if args.files:
        good_exts = filter(get_source_files.has_good_ext, files)
        not_generated = filter(
            lambda p: get_source_files.not_generated_file(p, True), good_exts)
        files = not_generated
    else:
        files = get_source_files.get_source_files(args.root, True)

    run_clang_format(files, args.quiet, args.verbose)
