#!/usr/bin/env python3

import argparse;
import subprocess;
import sys
from pathlib import Path


# The classes the compiler requires. Tests that define their own, e.g., to test the class hierarchy, start with this.
PRELUDE = Path(__file__).parent / 'prelude.sc'
NO_PRELUDE = '// no-prelude'

# Tests are grouped by topic in directories under input, which are mirrored in expected and the output directory.
INPUT_DIR = Path(__file__).parent / 'input'
EXPECTED_DIR = Path(__file__).parent / 'expected'


def process_file(cmd, sc_file, output_dir, override):

    with open(sc_file) as f:
        uses_prelude = f.readline().strip() != NO_PRELUDE
    # Paths in diagnostics are relative to the input directory, so the output doesn't depend on where it is run.
    args = [cmd, '--relative-to', INPUT_DIR]
    args += ['--prelude', PRELUDE, sc_file] if uses_prelude else [sc_file]
    cmd_result = subprocess.run(args, capture_output=True, text=True, check=True).stdout

    name = sc_file.resolve().relative_to(INPUT_DIR.resolve())

    expected_output_path = EXPECTED_DIR / name.with_suffix('.txt')
    expected_output_path.parent.mkdir(parents=True, exist_ok=True)
    output_file_path = output_dir / name.with_suffix('.txt')
    diff_file_path = output_dir / name.with_suffix('.diff')
    output_file_path.parent.mkdir(parents=True, exist_ok=True)

    if not expected_output_path.exists():
        print('expected output file does not exist, there is nothing to compare again, therefore, we will write to it')
        expected_output_path.write_text(cmd_result)
        return True
    else:
        expected_output = expected_output_path.read_text()
        matching = expected_output == cmd_result

        if override:
            # Nothing to compare against, as the output becomes the expected output.
            print(f"overriding output: {name} {'unchanged' if matching else 'updated'}")
            if not matching:
                expected_output_path.write_text(cmd_result)
            return True

        if not matching:
            print(f"FAILED: {name}. To update the output file, run with --override.")
            output_file_path.write_text(cmd_result)
            diff = subprocess.run(['diff', expected_output_path, output_file_path, '--unified', '--color'], capture_output=True, text=True)
            diff_file_path.write_text(diff.stdout)

            print(f"For the diff of the results see {diff_file_path}\n")

        return matching

def main():
    parser = argparse.ArgumentParser(
        prog='sc_sema error_regression test',
        description='Compiles each sc file in a directory, writes the output to a file, runs diff to see if there is any change.'
    )
    parser.add_argument('--cmd', help='path to compile cmd', required=True)
    parser.add_argument('--inputSCFile', help='input sc file', required=True)
    parser.add_argument('--outputDirectory', help='output directory used to store the new output and any dif.', required=True)
    parser.add_argument('--override', help='override existing files if present', action='store_true')

    args = parser.parse_args()
    
    success = process_file(args.cmd, Path(args.inputSCFile), Path(args.outputDirectory), args.override)

    if success:
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()


