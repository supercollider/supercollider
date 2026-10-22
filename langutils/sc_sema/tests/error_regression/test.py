#!/usr/bin/env python3
r"""
Compiles each test's sc file with ir_dump and compares the output with its expected output.

ir_dump is given by --cmd, or else by the SC_IR_DUMP environment variable, e.g.,
    export SC_IR_DUMP=build/langutils/sc_sema/utils/ir_dump/ir_dump

To run the tests, use ctest, which builds ir_dump first:
    ctest -R sc_sema_regression                     every test
    ctest -R 'sc_sema_regression\.literals'         a topic

To accept new output, which ctest can't do:
    test.py --override methods/poisoning            that test
    test.py --override literals                     every test in a topic

This can also run the suite, or some of it, with a summary, in parallel:
    test.py                                         every test
    test.py literals messages/perform               a topic, and one test
    test.py -j 1                                    one test at a time

ctest runs one test at a time:
    test.py --cmd ir_dump --inputSCFile input/literals/escapes.sc --outputDirectory out
"""

import argparse
import concurrent.futures
import os
import subprocess
import sys
import tempfile
from pathlib import Path


# The classes the compiler requires. Tests that define their own, e.g., to test the class hierarchy, start with this.
PRELUDE = Path(__file__).parent / 'prelude.sc'
NO_PRELUDE = '// no-prelude'

# Tests are grouped by topic in directories under input, which are mirrored in expected and the output directory.
INPUT_DIR = Path(__file__).parent / 'input'
EXPECTED_DIR = Path(__file__).parent / 'expected'

PASSED, FAILED, CREATED, UPDATED, UNCHANGED = 'passed', 'failed', 'created', 'updated', 'unchanged'


def test_name(sc_file):
    """The test's name, its path under input without the extension, e.g., literals/escapes."""
    return sc_file.resolve().relative_to(INPUT_DIR.resolve()).with_suffix('')


def run_ir_dump(cmd, sc_file):
    with open(sc_file) as f:
        uses_prelude = f.readline().strip() != NO_PRELUDE
    # Paths in diagnostics are relative to the input directory, so the output doesn't depend on where it is run.
    args = [cmd, '--relative-to', INPUT_DIR]
    args += ['--prelude', PRELUDE, sc_file] if uses_prelude else [sc_file]
    return subprocess.run(args, capture_output=True, text=True, check=True).stdout


def process_file(cmd, sc_file, output_dir, override):
    """Runs one test, returning its status and, when it failed, the path of its diff."""
    name = test_name(sc_file)
    output = run_ir_dump(cmd, sc_file)

    expected_output_path = EXPECTED_DIR / name.with_suffix('.txt')
    if not expected_output_path.exists():
        # Nothing to compare against, so this becomes the expected output.
        expected_output_path.parent.mkdir(parents=True, exist_ok=True)
        expected_output_path.write_text(output)
        return CREATED, None

    matching = expected_output_path.read_text() == output
    if override:
        if matching:
            return UNCHANGED, None
        expected_output_path.write_text(output)
        return UPDATED, None
    if matching:
        return PASSED, None

    output_file_path = output_dir / name.with_suffix('.txt')
    diff_file_path = output_dir / name.with_suffix('.diff')
    output_file_path.parent.mkdir(parents=True, exist_ok=True)
    output_file_path.write_text(output)
    diff = subprocess.run(['diff', '--unified', expected_output_path, output_file_path], capture_output=True, text=True)
    diff_file_path.write_text(diff.stdout)
    return FAILED, diff_file_path


def select_tests(selections):
    """The sc files named by selections, which are test names, topics, or paths. All of them when there are none."""
    if not selections:
        return sorted(INPUT_DIR.rglob('*.sc'))
    files = []
    for selection in selections:
        candidates = [Path(selection), INPUT_DIR / selection, (INPUT_DIR / selection).with_suffix('.sc')]
        path = next((c for c in candidates if c.exists()), None)
        if path is None:
            sys.exit(f"no test or topic named '{selection}'")
        found = sorted(path.rglob('*.sc')) if path.is_dir() else [path]
        files += [f for f in found if f not in files]
    return files


def run_test(cmd, sc_file, output_dir, override):
    """Runs one test of the suite, returning its status, its diff, and why ir_dump failed, if it did."""
    try:
        return *process_file(cmd, sc_file, output_dir, override), None
    except subprocess.CalledProcessError as e:
        return FAILED, None, f"ir_dump exited with {e.returncode}\n{e.stderr}"


def run_suite(cmd, selections, output_dir, override, jobs):
    files = select_tests(selections)
    # Each test only writes its own files, so they can run in parallel. The results are printed in test order.
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        results = list(pool.map(lambda f: run_test(cmd, f, output_dir, override), files))

    counts = {}
    for sc_file, (status, diff, error) in zip(files, results):
        name = test_name(sc_file)
        if error:
            print(f"FAILED   {name}: {error}")
        counts[status] = counts.get(status, 0) + 1
        if status == FAILED and diff:
            print(f"FAILED   {name}: see {diff}")
        elif status in (CREATED, UPDATED):
            print(f"{status.upper():8} {name}")

    summary = ', '.join(f'{counts[s]} {s}' for s in (PASSED, FAILED, CREATED, UPDATED, UNCHANGED) if s in counts)
    print(f"\n{len(files)} tests: {summary}")
    if counts.get(FAILED):
        print("To accept a test's new output, run with --override and its name.")
    return not counts.get(FAILED)


def main():
    parser = argparse.ArgumentParser(
        prog='test.py',
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('--cmd', help='path to ir_dump, SC_IR_DUMP by default', default=os.environ.get('SC_IR_DUMP'))
    parser.add_argument('--inputSCFile', help='run only this sc file, as ctest does')
    parser.add_argument('--outputDirectory', help='where to write the output and diff of failed tests, a temporary directory by default')
    parser.add_argument('--override', help='write the output as the expected output of the selected tests', action='store_true')
    parser.add_argument('-j', '--jobs', type=int, default=os.cpu_count(), help='how many tests to run at once, the number of cpus by default')
    parser.add_argument('tests', nargs='*', help='test names, e.g., literals/escapes, or topics, e.g., literals. All tests when none are given.')
    args = parser.parse_args()
    if not args.cmd:
        parser.error('give the path to ir_dump with --cmd, or set SC_IR_DUMP')
    if not Path(args.cmd).is_file():
        parser.error(f"ir_dump not found at '{args.cmd}', build it, or check --cmd or SC_IR_DUMP")

    # Failed tests write their output here. A temporary directory is removed again when nothing failed.
    output_dir = Path(args.outputDirectory) if args.outputDirectory else Path(tempfile.mkdtemp(prefix='sc_sema_regression_'))
    try:
        sys.exit(0 if run(args, output_dir) else 1)
    finally:
        if not args.outputDirectory and not any(output_dir.iterdir()):
            output_dir.rmdir()


def run(args, output_dir):
    """Runs the tests args selects, returning whether they passed."""
    if args.inputSCFile:
        # One test, for ctest.
        sc_file = Path(args.inputSCFile)
        status, diff = process_file(args.cmd, sc_file, output_dir, args.override)
        name = test_name(sc_file)
        if status == FAILED:
            print(f"FAILED: {name}. To update the output file, run with --override.")
            print(f"For the diff of the results see {diff}\n")
        elif status == CREATED:
            print(f"created the expected output of {name}, as it didn't exist")
        elif status in (UPDATED, UNCHANGED):
            print(f"overriding output: {name} {status}")
        return status != FAILED

    return run_suite(args.cmd, args.tests, output_dir, args.override, args.jobs)


if __name__ == '__main__':
    main()
