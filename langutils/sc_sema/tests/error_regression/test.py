#!/usr/bin/env python3
r"""
Compiles each test's sc file with ir_dump and compares the output with its expected output.

ir_dump is given by --cmd, or else by the SC_IR_DUMP environment variable, and find_assertions by
--find-assertions, or else SC_FIND_ASSERTIONS, e.g.,
    export SC_IR_DUMP=build/langutils/sc_sema/utils/ir_dump/ir_dump
    export SC_FIND_ASSERTIONS=build/langutils/sc_sema/tests/error_regression/find_assertions/find_assertions

A test can also assert what a method should compile to, with comments on the lines directly above it, each alone on
its line:
    // expect: -> 1
    constant { var a = 1; ^a }
    // expect: no result
    // expect-error: Variable already declared
    duplicate { var a, a; ^a }
The assertions are
    expect: -> VALUE                calling the method returns VALUE, as ir_dump writes it
    expect: no result               what calling the method returns isn't known
    expect: poisoned                the method is poisoned
    expect: not poisoned
    expect: no diagnostics          the method has no errors or warnings
    expect-error: TITLE             the method has an error, e.g., `expect-error: Variable not defined`
    expect-warning: TITLE
    expect-reads: NAME -> DECLARATION
                                    every read of NAME in the method refers to the declaration with this source, e.g.,
                                    `expect-reads: x -> x <- [1, 2]` in a generator, or `environment` for `~x`
    expect-reads: NAME -> DECLARATION; DECLARATION...
                                    the reads of NAME refer to these, in the order the reads are written
An scd file is a test of interpreted code, compiled as if written inside Interpreter:functionCompileContext. Its
assertions apply to all of it, wherever they are, as long as each is alone on its line, e.g.,
    // expect: -> 2
    a = 1; a + 1
It is compiled as the legacy version, unless its first lines say which release, as interpreted code has no version
comment, e.g.,
    // interpreted-version: 3.16
The first lines can also be `// no-prelude`, to compile it without the shared prelude.sc.
A test fails when an assertion doesn't hold, even when its output is overridden, so overriding can't hide a change
the test was written to catch.

To run the tests, use ctest, which builds ir_dump first:
    ctest -R sc_sema_regression                     every test
    ctest -R sc_sema_regression__literals            a topic

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
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path


# The classes the compiler requires. Tests that define their own, e.g., to test the class hierarchy, start with this.
PRELUDE = Path(__file__).parent / 'prelude.sc'
NO_PRELUDE = '// no-prelude'
# The release interpreted code is compiled as, e.g., `// interpreted-version: 3.16`.
INTERPRETED_VERSION = re.compile(r'^// interpreted-version: (?P<release>\S+)$')

# Tests are grouped by topic in directories under input, which are mirrored in expected and the output directory.
INPUT_DIR = Path(__file__).parent / 'input'
EXPECTED_DIR = Path(__file__).parent / 'expected'

PASSED, FAILED, CREATED, UPDATED, UNCHANGED = 'passed', 'failed', 'created', 'updated', 'unchanged'


@dataclass(frozen=True)
class Tools:
    ir_dump: str
    find_assertions: str


@dataclass
class Result:
    status: str
    # When the output didn't match the expected output.
    diff: Path | None = None
    # The assertions that didn't hold.
    failed_assertions: list[str] = field(default_factory=list)

    @property
    def failed(self):
        return self.status == FAILED or bool(self.failed_assertions)


# A method in ir_dump's output, e.g., `|  |  SCMethod (5:5): poisoned: foo { ^1 } -> 1`.
METHOD_LINE = re.compile(r'^(?P<indent>(?:\|  )*)SCMethod \((?P<line>\d+):\d+\): (?P<poisoned>poisoned: )?(?P<rest>.*)$')
# Interpreted code, e.g., `SCInterpreted (1:1): poisoned: interpreted/region.scd`, which is checked like a method.
INTERPRETED_LINE = re.compile(r'^(?P<indent>)SCInterpreted \((?P<line>\d+):\d+\): (?P<poisoned>poisoned: )?(?P<rest>.*)$')
# What interpreted code is compiled into, a function for a file as a function, otherwise a list of regions, which says
# what the code returns, e.g., `|  SCRegionList (1:1) -> 2`.
INTERPRETED_FUNCTION_LINE = re.compile(r'^\|  SC(?:Function|RegionList) \(\d+:\d+\)(?: -> (?P<result>.*))?$')
# Any node, which ends the block of a method at the same depth or shallower.
NODE_LINE = re.compile(r'^(?P<indent>(?:\|  )*)SC\w+ \(\d+:\d+\)')
# A read, e.g., `SCRead (5:48): local a = 1, declared (5:25): a = 1`, environment variables have no declaration.
READ_LINE = re.compile(r'^(?:\|  )*SCRead \((?P<line>\d+):(?P<column>\d+)\): \w+ (?P<name>\w+)'
                       r'(?: = .*?)?(?:, declared \(\d+:\d+\): (?P<declaration>.*))?$')
# A diagnostic printed under a method.
DIAGNOSTIC_LINE = re.compile(r'^(?:\|  )+(?P<severity>Error|Warning): (?P<title>.*)$')


@dataclass
class Method:
    # Where its name is.
    line: int
    poisoned: bool
    # What calling it returns, as ir_dump writes it, if it is known.
    result: str | None
    diagnostics: list[tuple[str, str]]
    # (line, column, name, the declaration's source or None), in the order they are written.
    reads: list[tuple[int, int, str, str | None]]
    # Interpreted code, rather than a method.
    interpreted: bool = False


def methods_in(output):
    """The methods in ir_dump's output."""
    lines = output.split('\n')
    methods = []
    for i, line in enumerate(lines):
        match = METHOD_LINE.match(line) or INTERPRETED_LINE.match(line)
        if not match:
            continue
        interpreted = line.startswith('SCInterpreted')
        depth = len(match['indent'])
        diagnostics = []
        reads = []
        for inner in lines[i + 1:]:
            node = NODE_LINE.match(inner)
            if node and len(node['indent']) <= depth:
                break
            if diagnostic := DIAGNOSTIC_LINE.match(inner):
                diagnostics.append((diagnostic['severity'], diagnostic['title']))
            if read := READ_LINE.match(inner):
                reads.append((int(read['line']), int(read['column']), read['name'], read['declaration']))
        reads.sort()
        if interpreted:
            function = next(f for l in lines[i + 1:] if (f := INTERPRETED_FUNCTION_LINE.match(l)))
            result = function['result']
        else:
            # The source is written before the result, and always ends with the method's `}`.
            source_and_result = re.match(r'^(.*\}) -> (.*)$', match['rest'])
            result = source_and_result[2] if source_and_result else None
        methods.append(Method(int(match['line']), bool(match['poisoned']), result, diagnostics, reads, interpreted))
    return methods


def check_assertion(assertion, method):
    """Why the assertion doesn't hold for method, or None when it does."""
    kind, _, value = assertion.partition(':')
    value = value.strip()
    if kind == 'expect' and value.startswith('->'):
        expected = value[2:].strip()
        if method.result != expected:
            return f"the method returns {method.result}" if method.result else "the method has no result"
    elif kind == 'expect' and value == 'no result':
        if method.result is not None:
            return f"the method returns {method.result}"
    elif kind == 'expect' and value in ('poisoned', 'not poisoned'):
        if method.poisoned != (value == 'poisoned'):
            return "the method is poisoned" if method.poisoned else "the method isn't poisoned"
    elif kind == 'expect' and value == 'no diagnostics':
        if method.diagnostics:
            return "the method has " + ', '.join(f"{s.lower()} '{t}'" for s, t in method.diagnostics)
    elif kind == 'expect-reads':
        name, arrow, declarations = value.partition('->')
        name = name.strip()
        expected = [d.strip() for d in declarations.split(';')]
        if not arrow or not name or not all(expected):
            return "it should be `expect-reads: NAME -> DECLARATION`"
        found = [declaration or 'environment' for _, _, n, declaration in method.reads if n == name]
        if not found:
            return f"the method doesn't read {name}"
        # One declaration is for all the reads, otherwise there is one for each.
        if found != (expected * len(found) if len(expected) == 1 else expected):
            return f"its reads of {name} refer to " + '; '.join(found)
    elif kind in ('expect-error', 'expect-warning'):
        severity = 'Error' if kind == 'expect-error' else 'Warning'
        if (severity, value) not in method.diagnostics:
            found = ', '.join(f"{s.lower()} '{t}'" for s, t in method.diagnostics) or 'no diagnostics'
            return f"the method has {found}"
    else:
        return "this isn't an assertion, see test.py --help"
    return None


def check_assertions(tools, sc_file, output):
    """The assertions in sc_file that don't hold for ir_dump's output, as messages."""
    found = subprocess.run([tools.find_assertions, sc_file], capture_output=True, text=True, check=True).stdout
    assertions = []
    for entry in found.splitlines():
        position, _, assertion = entry.partition(' ')
        line, column = map(int, position.split(':'))
        assertions.append((line, column, assertion))
    assertion_lines = {line for line, _, _ in assertions}
    source_lines = sc_file.read_text().split('\n')
    found_methods = methods_in(output)
    methods = {m.line: m for m in reversed(found_methods)}
    interpreted = next((m for m in found_methods if m.interpreted), None) if sc_file.suffix == '.scd' else None

    failures = []
    for line, column, assertion in assertions:
        where = f"{test_name(sc_file)}{sc_file.suffix}:{line}: '{assertion}'"
        # Applies to the method below it, after any other assertions stacked between them.
        below = line + 1
        while below in assertion_lines:
            below += 1
        method = interpreted if sc_file.suffix == '.scd' else methods.get(below)
        # Otherwise, `a { ^1 } // expect: -> 1` would apply to the method after `a`.
        if source_lines[line - 1][:column - 1].strip():
            failures.append(f"{where}, it must be alone on its line, above the method")
        elif method is None and sc_file.suffix == '.scd':
            failures.append(f"{where}, but the interpreted code wasn't compiled")
        elif method is None:
            failures.append(f"{where}, it must be on the line above a method, or above another assertion that is")
        elif reason := check_assertion(assertion, method):
            failures.append(f"{where}, but {reason}")
    return failures


def test_name(sc_file):
    """The test's name, its path under input without the extension, e.g., literals/escapes."""
    return sc_file.resolve().relative_to(INPUT_DIR.resolve()).with_suffix('')


def headers(sc_file):
    """Whether the test uses the prelude, and the release its interpreted code is compiled as, from its first lines."""
    uses_prelude = True
    release = None
    with open(sc_file) as f:
        for line in f:
            line = line.strip()
            if line == NO_PRELUDE:
                uses_prelude = False
            elif version := INTERPRETED_VERSION.match(line):
                release = version['release']
            else:
                break
    return uses_prelude, release


def run_ir_dump(cmd, sc_file):
    uses_prelude, release = headers(sc_file)
    # Paths in diagnostics are relative to the input directory, so the output doesn't depend on where it is run.
    # The output is the same however many threads compile it, two checks that, while leaving cpus for other tests.
    args = [cmd, '--threads', '2', '--relative-to', INPUT_DIR]
    if release:
        args += ['--interpreted-version', release]
    args += ['--prelude', PRELUDE, sc_file] if uses_prelude else [sc_file]
    return subprocess.run(args, capture_output=True, text=True, check=True).stdout


def process_file(tools, sc_file, output_dir, override):
    """Runs one test."""
    output = run_ir_dump(tools.ir_dump, sc_file)
    result = compare_output(sc_file, output, output_dir, override)
    result.failed_assertions = check_assertions(tools, sc_file, output)
    return result


def compare_output(sc_file, output, output_dir, override):
    """Compares output with the expected output, or writes it as the expected output."""
    name = test_name(sc_file)
    expected_output_path = EXPECTED_DIR / name.with_suffix('.txt')
    if not expected_output_path.exists():
        # Nothing to compare against, so this becomes the expected output.
        expected_output_path.parent.mkdir(parents=True, exist_ok=True)
        expected_output_path.write_text(output)
        return Result(CREATED)

    matching = expected_output_path.read_text() == output
    if override:
        if matching:
            return Result(UNCHANGED)
        expected_output_path.write_text(output)
        return Result(UPDATED)
    if matching:
        return Result(PASSED)

    output_file_path = output_dir / name.with_suffix('.txt')
    diff_file_path = output_dir / name.with_suffix('.diff')
    output_file_path.parent.mkdir(parents=True, exist_ok=True)
    output_file_path.write_text(output)
    diff = subprocess.run(['diff', '--unified', expected_output_path, output_file_path], capture_output=True, text=True)
    diff_file_path.write_text(diff.stdout)
    return Result(FAILED, diff_file_path)


def test_files_in(directory):
    """The tests in directory, class files and interpreted code."""
    return [*directory.rglob('*.sc'), *directory.rglob('*.scd')]


def select_tests(selections):
    """The sc files named by selections, which are test names, topics, or paths. All of them when there are none."""
    if not selections:
        return sorted(test_files_in(INPUT_DIR))
    files = []
    for selection in selections:
        candidates = [Path(selection), INPUT_DIR / selection, (INPUT_DIR / selection).with_suffix('.sc'),
                      (INPUT_DIR / selection).with_suffix('.scd')]
        path = next((c for c in candidates if c.exists()), None)
        if path is None:
            sys.exit(f"no test or topic named '{selection}'")
        found = sorted(test_files_in(path)) if path.is_dir() else [path]
        files += [f for f in found if f not in files]
    return files


def run_test(tools, sc_file, output_dir, override):
    """Runs one test of the suite, a tool crashing fails it."""
    try:
        return process_file(tools, sc_file, output_dir, override)
    except subprocess.CalledProcessError as e:
        return Result(FAILED, failed_assertions=[f"{Path(e.cmd[0]).name} exited with {e.returncode}\n{e.stderr}"])


def print_result(name, result):
    if result.status == FAILED and result.diff:
        print(f"FAILED   {name}: the output changed, see {result.diff}")
    elif result.status in (CREATED, UPDATED):
        print(f"{result.status.upper():8} {name}")
    for failure in result.failed_assertions:
        print(f"FAILED   {failure}")


def run_suite(tools, selections, output_dir, override, jobs):
    files = select_tests(selections)
    # Each test only writes its own files, so they can run in parallel. The results are printed in test order.
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        results = list(pool.map(lambda f: run_test(tools, f, output_dir, override), files))

    counts = {}
    for sc_file, result in zip(files, results):
        print_result(test_name(sc_file), result)
        status = FAILED if result.failed else result.status
        counts[status] = counts.get(status, 0) + 1

    summary = ', '.join(f'{counts[s]} {s}' for s in (PASSED, FAILED, CREATED, UPDATED, UNCHANGED) if s in counts)
    print(f"\n{len(files)} tests: {summary}")
    if any(r.status == FAILED for r in results):
        print("To accept a test's new output, run with --override and its name.")
    return not counts.get(FAILED)


def main():
    parser = argparse.ArgumentParser(
        prog='test.py',
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('--cmd', help='path to ir_dump, SC_IR_DUMP by default', default=os.environ.get('SC_IR_DUMP'))
    parser.add_argument('--find-assertions', help='path to find_assertions, SC_FIND_ASSERTIONS by default',
                        default=os.environ.get('SC_FIND_ASSERTIONS'))
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
    if not args.find_assertions:
        parser.error('give the path to find_assertions with --find-assertions, or set SC_FIND_ASSERTIONS')
    if not Path(args.find_assertions).is_file():
        parser.error(f"find_assertions not found at '{args.find_assertions}', build it, or check --find-assertions "
                     "or SC_FIND_ASSERTIONS")

    # Failed tests write their output here. A temporary directory is removed again when nothing failed.
    output_dir = Path(args.outputDirectory) if args.outputDirectory else Path(tempfile.mkdtemp(prefix='sc_sema_regression_'))
    try:
        sys.exit(0 if run(args, output_dir) else 1)
    finally:
        if not args.outputDirectory and not any(output_dir.iterdir()):
            output_dir.rmdir()


def run(args, output_dir):
    """Runs the tests args selects, returning whether they passed."""
    tools = Tools(args.cmd, args.find_assertions)
    if args.inputSCFile:
        # One test, for ctest.
        sc_file = Path(args.inputSCFile)
        result = process_file(tools, sc_file, output_dir, args.override)
        name = test_name(sc_file)
        if result.status == FAILED:
            print(f"FAILED: {name}. To update the output file, run with --override.")
            print(f"For the diff of the results see {result.diff}\n")
        elif result.status == CREATED:
            print(f"created the expected output of {name}, as it didn't exist")
        elif result.status in (UPDATED, UNCHANGED):
            print(f"overriding output: {name} {result.status}")
        for failure in result.failed_assertions:
            print(f"FAILED: {failure}")
        return not result.failed

    return run_suite(tools, args.tests, output_dir, args.override, args.jobs)


if __name__ == '__main__':
    main()
