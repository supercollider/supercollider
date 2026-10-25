#!/usr/bin/env python3
"""The differential test (B7): compiles the class library and sc_sema's Legacy bytecode test classes with old sclang
and with the backend, and compares every method's bytes, tables and properties, see compare_bytecode.py.

    differential_test.py [--sclang PATH] [--sc-inspect PATH] [--keep DIR]

Paths default to the repository's build/. Fails on any difference, method only one compiler compiled, or old sclang
error that differential_allowlist.txt doesn't name, and on an allowlisted method that now matches. --keep writes the
dumps and the library copy to DIR rather than a temporary directory.
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile

import compare_bytecode

TOOLS = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(TOOLS, '..', '..', '..'))
TEST_CLASSES = os.path.join(REPO, 'langutils', 'sc_sema', 'tests', 'error_regression', 'input', 'versions')
# bytecode_loops.sc redefines Integer and Float, old sclang can't compile bytecode_unary_overflow.sc.
SKIPPED = {'bytecode_loops.sc', 'bytecode_unary_overflow.sc'}


def skipped_directory(name):
    """SC_Filesystem::shouldNotCompileDirectory, on Linux, without an IDE."""
    name = name.lower()
    return (name in ('help', 'ignore', '.svn', '.git', '_darcs', 'osx', 'windows', 'iphone', 'wasm')
            or name.startswith('scide_'))


def copy_compiled(src, dst):
    """What sclang compiles of src, as sc_inspect skips no directories and doesn't follow symlinked ones."""
    shutil.copytree(src, dst, symlinks=False,
                    ignore=lambda d, names: [n for n in names if os.path.isdir(os.path.join(d, n)) and skipped_directory(n)])


def read_allowlist(path):
    out = {}
    for line in open(path, encoding='utf-8'):
        line = line.strip()
        if line and not line.startswith('#'):
            key, _, reason = line.partition(' ')
            cls, _, method = key.partition(':')
            out[(cls, method)] = reason.strip()
    return out


def run_old(sclang, work, tests):
    conf = os.path.join(work, 'conf.yaml')
    with open(os.path.join(TOOLS, 'sclang_conf.yaml.in'), encoding='utf-8') as f:
        text = f.read().replace('REPO', REPO).replace('TESTS', tests)
    with open(conf, 'w', encoding='utf-8') as f:
        f.write(text)
    out = os.path.join(work, 'old.txt')
    log = os.path.join(work, 'sclang.log')
    env = dict(os.environ, QT_QPA_PLATFORM='offscreen')
    with open(log, 'w', encoding='utf-8') as f:
        subprocess.run([sclang, '-l', conf, os.path.join(TOOLS, 'dump_old_bytecode.scd'), out], cwd=work, env=env,
                       stdout=f, stderr=subprocess.STDOUT, timeout=600)
    text = open(out, encoding='utf-8', errors='replace').read() if os.path.exists(out) else ''
    if not text.endswith('END\n'):
        sys.exit('old sclang did not finish its dump, see %s' % log)
    errors = [line for line in text.splitlines() if line.startswith('ERROR ')]
    return out, errors


def run_new(sc_inspect, work, library):
    out = os.path.join(work, 'new.txt')
    with open(out, 'w', encoding='utf-8') as f:
        result = subprocess.run([sc_inspect, '--bytecode', library], stdout=f, stderr=subprocess.PIPE, text=True)
    if result.returncode != 0:
        sys.exit('sc_inspect failed (%d):\n%s' % (result.returncode, result.stderr))
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--sclang', default=os.path.join(REPO, 'build', 'lang', 'sclang'))
    parser.add_argument('--sc-inspect', default=os.path.join(REPO, 'build', 'langutils', 'sc_inspect', 'sc_inspect'))
    parser.add_argument('--keep')
    args = parser.parse_args()

    work = os.path.abspath(args.keep) if args.keep else tempfile.mkdtemp(prefix='sc_differential_')
    for name in ('tests', 'library'):
        shutil.rmtree(os.path.join(work, name), ignore_errors=True)
    tests = os.path.join(work, 'tests')
    os.makedirs(tests)
    for name in sorted(os.listdir(TEST_CLASSES)):
        if name.startswith('bytecode_') and name.endswith('.sc') and name not in SKIPPED:
            shutil.copy(os.path.join(TEST_CLASSES, name), tests)
    # One module, as the test classes are compiled with the library by old sclang.
    library = os.path.join(work, 'library')
    copy_compiled(os.path.join(REPO, 'SCClassLibrary'), library)
    shutil.copytree(tests, os.path.join(library, 'DifferentialTests'))

    old, errors = run_old(args.sclang, work, tests)
    new = run_new(args.sc_inspect, work, library)
    stats, differences, missing, extra = compare_bytecode.compare_all(old, new)

    allowed = read_allowlist(os.path.join(TOOLS, 'differential_allowlist.txt'))
    seen, failures = set(), 0
    for key, problems in differences:
        if key in allowed:
            seen.add(key)
        else:
            compare_bytecode.print_difference(key, problems)
            failures += 1
    for label, keys in (('MISSING', missing), ('EXTRA', extra)):
        for key in keys:
            if key in allowed:
                seen.add(key)
            else:
                print('%s %s:%s' % (label, key[0], key[1]))
                failures += 1
    for error in errors:
        print('OLD %s' % error)
        failures += 1
    for key in allowed:
        if key not in seen:
            print('STALE %s:%s, it no longer differs, remove it from the allowlist' % key)
            failures += 1

    print(stats)
    print('%d allowlisted, %d failures%s' % (len(seen), failures, ', files in ' + work if args.keep else ''))
    if not args.keep:
        shutil.rmtree(work)
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
