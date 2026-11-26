"""Pipes text framed as an editor sends it into sclang, and checks the output.

The header is SOH file RS line RS column [RS version] STX, see SC_TerminalClient::interpretInput.
"""

import argparse
import subprocess
import sys

SOH, STX, RS, PRINT, SILENT = "\x01", "\x02", "\x1e", "\x0c", "\x1b"

# A declaration after an expression is an error in Legacy, and allowed in 3.16.
LATE_VAR = "var p = 1; p; var q = 2; "

# Each case: the header's version records, the text, what the output must contain and must not, and optionally the
# header's file (empty for none) and sclang's extra arguments. The header's line is 3, counted from 0, so an error on
# the text's first line is at line 4.
CASES = {
    "three_records": dict(records=[], text='"three records " ++ 1', expected=["-> three records 1"]),
    "legacy": dict(
        records=["legacy"],
        text=LATE_VAR + '"legacy " ++ q',
        expected=["Variable declared after an expression", "a.scd:4:"],
        forbidden=["-> legacy 2"],
    ),
    "v3_16": dict(records=["3.16"], text=LATE_VAR + '"3.16 " ++ q', expected=["-> 3.16 2"]),
    "more_records": dict(
        records=["3.16", "later"], text=LATE_VAR + '"more records " ++ q', expected=["-> more records 2"]
    ),
    "bad_version": dict(
        records=["meow"],
        text='"bad version " ++ 1',
        expected=["ERROR: version: 'meow' isn't a release", "-> nil"],
        forbidden=["-> bad version 1"],
    ),
    # Without a version, text is Legacy.
    "no_file": dict(
        file="",
        records=[],
        text=LATE_VAR + '"no file " ++ q',
        expected=["Variable declared after an expression"],
        forbidden=["-> no file 2"],
    ),
    # The IDE's help browser and untitled documents send a version with no file.
    "no_file_with_version": dict(
        file="",
        records=["3.16"],
        text=LATE_VAR + '"no file with version " ++ q',
        expected=["-> no file with version 2"],
    ),
    # --interpreted-version sets the version of text with no file, and nothing else.
    "flag_no_file": dict(
        file="",
        records=[],
        text=LATE_VAR + '"flag no file " ++ q',
        args=["--interpreted-version", "3.16"],
        expected=["-> flag no file 2"],
    ),
    "flag_with_file": dict(
        records=[],
        text=LATE_VAR + '"flag with file " ++ q',
        args=["--interpreted-version", "3.16"],
        expected=["Variable declared after an expression"],
        forbidden=["-> flag with file 2"],
    ),
    "flag_and_line_1": dict(
        file="",
        records=[],
        text="//# VERSION legacy\n" + LATE_VAR + '"flag and line 1 " ++ q',
        args=["--interpreted-version", "3.16"],
        expected=["Variable declared after an expression"],
        forbidden=["-> flag and line 1 2"],
    ),
    "flag_and_record": dict(
        file="",
        records=["legacy"],
        text=LATE_VAR + '"flag and record " ++ q',
        args=["--interpreted-version", "3.16"],
        expected=["Variable declared after an expression"],
        forbidden=["-> flag and record 2"],
    ),
}


def run_case(sclang, config, name):
    case = CASES[name]
    header = SOH + RS.join([case.get("file", "/piped/a.scd"), "3", "0"] + case["records"]) + STX
    text, expected, forbidden = case["text"], case["expected"], case.get("forbidden", [])
    framed = header + text + PRINT + "0.exit" + SILENT

    result = subprocess.run(
        [sclang, "-i", "piped_header", "-l", config] + case.get("args", []),
        input=framed.encode(),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        timeout=120,
    )
    output = result.stdout.decode(errors="replace")

    failures = [f"missing {p!r}" for p in expected if p not in output]
    failures += [f"unexpected {p!r}" for p in forbidden if p in output]
    if result.returncode != 0:
        failures.append(f"sclang exited with {result.returncode}")
    if failures:
        print(output)
        print("\n".join(f"{name}: {f}" for f in failures))
    return not failures


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--sclang", required=True)
    parser.add_argument("--config", required=True)
    args = parser.parse_args()

    # Each case runs its own sclang.
    passed = [run_case(args.sclang, args.config, name) for name in CASES]
    print(f"{sum(passed)} of {len(passed)} cases passed")
    return 0 if all(passed) else 1


if __name__ == "__main__":
    sys.exit(main())
