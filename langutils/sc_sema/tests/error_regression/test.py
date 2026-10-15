#!/usr/bin/env python3

import subprocess;
from pathlib import Path
import argparse;
import sys
import re

def process_file(cmd, file, override):

    requires_intrinsics = "true" if "intrinsic" in file.absolute().as_posix() else "false"

    cmd_result = subprocess.run([cmd, file, requires_intrinsics], capture_output=True, text=True, check=True)
    # remove file path info
    cmd_result = re.sub(r'file:.*', 'file:', cmd_result.stdout)

    expected_file = file.with_suffix('.txt')
    got_file = file.with_suffix('.got')
    diff_file = file.with_suffix('.diff')

    if got_file.exists():
        got_file.unlink()

    if diff_file.exists():
        diff_file.unlink()

    if expected_file.exists():
        expected = expected_file.read_text(encoding="utf-8")
        result = expected == cmd_result

        if override:
            expected_file.write_text(cmd_result, encoding="utf-8")

        if not result:
            got_file.write_text(cmd_result, encoding="utf-8")
            diff = subprocess.run(['diff', expected_file, got_file, '--unified', '--color'], capture_output=True, text=True)
            got_file.unlink()
            diff_file.write_text(diff.stdout, encoding='utf-8')

        return result
    else:
        expected_file.write_text(cmd_result, encoding="utf-8")
        return True

def main():
    parser = argparse.ArgumentParser(
        prog='sc_sema error_regression test',
        description='Compiles each sc file in a directory, writes the output to a file, runs diff to see if there is any change.'
    )
    parser.add_argument('--cmd', help='path to compile cmd', required=True)
    parser.add_argument('--dir', help='directory', required=True)
    parser.add_argument('--override', help='override exisiting files if present', type=bool, default=False)

    args = parser.parse_args()

    results = {}
    
    got_error = False;
    for file in Path(args.dir).glob('*.sc'):
        r = process_file(args.cmd, file, args.override)
        got_error = got_error or r
        results[file] = r

    if got_error:
        sys.exit(1)
    else:
        sys.exit(0)


if __name__ == '__main__':
    main()


