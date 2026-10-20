#!/usr/bin/env python3

import argparse;
import re
import subprocess;
import sys
from pathlib import Path


def process_file(cmd, sc_file, output_dir, override):

    cmd_result = subprocess.run([cmd, sc_file], capture_output=True, text=True, check=True)

    # remove file path info
    cmd_result = re.sub(r'file:.*', 'file:', cmd_result.stdout)

    expected_dir = sc_file.parent / "../expected"
    expected_dir.mkdir(parents=True, exist_ok=True)

    expected_output_path = sc_file.parent / "../expected" / sc_file.with_suffix('.txt').name
    output_file_path = output_dir / Path(sc_file.name).with_suffix('.txt')
    diff_file_path = output_dir / Path(sc_file.name).with_suffix('.diff')

    if not expected_output_path.exists():
        print('expected output file does not exist, there is nothing to compare again, therefore, we will write to it')
        expected_output_path.write_text(cmd_result)
        return True
    else:
        expected_output = expected_output_path.read_text()
        matching = expected_output == cmd_result

        if override:
            print('overriding output')
            expected_output_path.write_text(cmd_result)
        if not matching:
            print(f"FAILED: {sc_file.name}. To update the output file, just delete {expected_output_path} and it will be regenerated.")
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
    parser.add_argument('--override', help='override exisiting files if present', type=bool, default=False)

    args = parser.parse_args()
    
    success = process_file(args.cmd, Path(args.inputSCFile), Path(args.outputDirectory), args.override)

    if success:
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()


