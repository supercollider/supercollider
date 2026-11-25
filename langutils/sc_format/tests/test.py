"""sc_format's golden tests.

Each file in input/ is formatted with the options of the nearest .sc-format.scd above it in input/, and must give the
file at the same path in expected/, byte for byte. A test with no file in expected/ must give its input unchanged.
Formatting must also keep its tokens and change nothing when done twice (sc_format --verify).

    test.py --cmd=path/to/sc_format                             every test
    test.py --cmd=path/to/sc_format spacing/dot.scd indent/2    those tests, and every test in a directory
    test.py --cmd=path/to/sc_format --override [tests]          writes the results to expected/, to review,
                                                                and removes those that equal their input
    test.py --html=page.html [tests]                            writes a page showing each test's options, input,
                                                                and expected output side by side
"""

import argparse
import difflib
import html
import itertools
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import unicodedata

HERE = pathlib.Path(__file__).resolve().parent
INPUT = HERE / "input"
EXPECTED = HERE / "expected"
OPTIONS = ".sc-format.scd"
PAGE = HERE / "golden_page.html"


def tests_in(names):
    paths = [INPUT / name for name in names] if names else [INPUT]
    found = []
    for path in paths:
        files = [path] if path.is_file() else path.rglob("*")
        found += [f for f in files if f.suffix in (".sc", ".scd") and f.name != OPTIONS]
    return sorted(found)


def options_above(directory):
    for d in (directory, *directory.parents):
        if d == INPUT.parent:
            break
        if (d / OPTIONS).is_file():
            return d / OPTIONS
    return None


def options_for(test):
    return options_above(test.parent)


def expected_of(test):
    return EXPECTED / test.relative_to(INPUT)


# The file a test's output must equal: its expected file, or its input if formatting doesn't change it.
def wanted_of(test):
    expected = expected_of(test)
    return expected if expected.is_file() else test


def remove_expected(expected):
    expected.unlink(missing_ok=True)
    for directory in expected.parents:
        if directory == EXPECTED or not directory.is_dir() or any(directory.iterdir()):
            break
        directory.rmdir()


# The lines of a text with their whitespace made visible, so a diff shows it.
def visible(text):
    lines = text.decode("utf-8", "replace").split("\n")
    shown = [line.replace(" ", "·").replace("\t", "→").replace("\r", "␍") for line in lines]
    # Every line but the last ended in a newline. The last is empty if the text ends in one.
    return [line + "⏎\n" for line in shown[:-1]] + ([shown[-1] + "\n"] if shown[-1] else [])


def run(cmd, test, override):
    name = test.relative_to(INPUT).as_posix()
    options = options_for(test)
    if options is None:
        return f"{name}: no {OPTIONS} above it in input/"
    with tempfile.TemporaryDirectory() as tmp:
        copy = pathlib.Path(tmp) / test.name
        shutil.copyfile(test, copy)
        for flag in ("--verify", "-i"):
            result = subprocess.run([cmd, flag, "--config", str(options), str(copy)], capture_output=True)
            if result.returncode != 0:
                return f"{name}: sc_format {flag} failed\n{result.stderr.decode('utf-8', 'replace')}"
        formatted = copy.read_bytes()
    if override:
        expected = expected_of(test)
        if formatted == test.read_bytes():
            remove_expected(expected)
        else:
            expected.parent.mkdir(parents=True, exist_ok=True)
            expected.write_bytes(formatted)
        return None
    wanted = wanted_of(test)
    if formatted == wanted.read_bytes():
        return None
    label = "expected" if wanted != test else "input"
    diff = difflib.unified_diff(visible(wanted.read_bytes()), visible(formatted), label, "formatted")
    return f"{name}: differs (· space, → tab, ⏎ newline)\n" + "".join(diff)


# The page: each test's options, then its input beside its expected output, the lines lined up and the changes marked.


# The options files a test uses, nearest first. One with `inherit: true` builds on the one above it.
def options_chain(test):
    chain = []
    found = options_for(test)
    while found:
        chain.append(found)
        if not re.search(r"\binherit\s*:\s*true\b", found.read_text(encoding="utf-8")):
            break
        found = options_above(found.parent.parent)
    return chain


# The lines of a text, each with its newline.
def lines_of(text):
    return re.findall(r"[^\n]*\n|[^\n]+\Z", text)


WHITESPACE = {
    " ": '<i class="s"> </i>',
    "\t": '<i class="t">\t</i>',
    "\n": '<i class="n"></i>',
    "\r": '<i class="r" title="carriage return"></i>',
}


def text_html(text):
    out = []
    for c in text:
        if c in WHITESPACE:
            out.append(WHITESPACE[c])
        elif unicodedata.category(c) in ("Zs", "Zl", "Zp", "Cc", "Cf"):
            out.append(f'<i class="u" title="{unicodedata.name(c, "")}">U+{ord(c):04X}</i>')
        else:
            out.append(html.escape(c))
    return "".join(out)


def line_html(line, marks):
    out = []
    at = 0
    for begin, end in marks:
        out += [text_html(line[at:begin]), "<mark>", text_html(line[begin:end]), "</mark>"]
        at = end
    out.append(text_html(line[at:]))
    return "".join(out)


# The characters that differ between two lines, if they are alike enough for that to help.
def marks_of(old, new):
    matcher = difflib.SequenceMatcher(None, old, new, autojunk=False)
    if matcher.ratio() < 0.4:
        return [], []
    old_marks, new_marks = [], []
    for op, i1, i2, j1, j2 in matcher.get_opcodes():
        if op != "equal":
            old_marks += [(i1, i2)] if i1 < i2 else []
            new_marks += [(j1, j2)] if j1 < j2 else []
    return old_marks, new_marks


# Pairs of line indexes, None for no line. Lines are lined up by their text without whitespace, as formatting changes
# only whitespace.
def rows_of(old, new):
    def key(line):
        return "".join(line.split())

    matcher = difflib.SequenceMatcher(None, [key(l) for l in old], [key(l) for l in new], autojunk=False)
    rows = []
    for _, i1, i2, j1, j2 in matcher.get_opcodes():
        for k in range(max(i2 - i1, j2 - j1)):
            rows.append((i1 + k if i1 + k < i2 else None, j1 + k if j1 + k < j2 else None))
    return rows


# A line's two cells, kind "old" or "new" for a line that changes, "pad" where the other side has a line.
def side_html(lines, i, marks, kind):
    if i is None:
        return '<td class="ln pad"></td><td class="code pad"></td>'
    ln, code = ("ln " + kind, "code " + kind) if kind else ("ln", "code")
    return f'<td class="{ln}">{i + 1}</td><td class="{code}">{line_html(lines[i], marks)}</td>'


def describe(text):
    if not text:
        return "empty"
    count = len(lines_of(text))
    return f"{count} line{'' if count == 1 else 's'}" + ("" if text.endswith("\n") else ", no newline at the end")


# A test's section of the page, and whether formatting changes it.
def test_html(test):
    name = test.relative_to(INPUT).as_posix()
    old_text = test.read_bytes().decode("utf-8", "replace")
    new_text = wanted_of(test).read_bytes().decode("utf-8", "replace")
    old, new = lines_of(old_text), lines_of(new_text)

    rows = []
    differ = 0
    for i, j in rows_of(old, new):
        if i is not None and j is not None and old[i] == new[j]:
            rows.append(f"<tr>{side_html(old, i, [], '')}{side_html(new, j, [], '')}</tr>")
            continue
        differ += 1
        old_marks, new_marks = marks_of(old[i], new[j]) if i is not None and j is not None else ([], [])
        rows.append(f"<tr>{side_html(old, i, old_marks, 'old')}{side_html(new, j, new_marks, 'new')}</tr>")
    if not rows:
        rows = ['<tr><td colspan="4" class="none">Both files are empty.</td></tr>']

    if old_text == new_text:
        summary = "unchanged"
    else:
        summary = "1 line differs" if differ == 1 else f"{differ} lines differ"

    options = []
    for k, path in enumerate(options_chain(test)):
        label = ("inherits " if k else "") + path.relative_to(INPUT).as_posix()
        text = path.read_text(encoding="utf-8").rstrip("\n")
        options.append(f"<figure class=\"option\"><figcaption>{html.escape(label)}</figcaption>"
                       f"<pre>{html.escape(text)}</pre></figure>")

    directory = html.escape(test.parent.relative_to(INPUT).as_posix())
    anchor = html.escape(name)
    section = "\n".join([
        f'<section class="test" id="{anchor}">',
        f'<header><h2><a href="#{anchor}"><span class="dir">{directory}/</span>{html.escape(test.name)}</a></h2>',
        f'<span class="summary">{summary}</span></header>',
        f'<div class="options">{"".join(options)}</div>',
        '<div class="scroll"><table class="diff">',
        '<colgroup><col class="ln"><col><col class="ln"><col></colgroup>',
        f'<thead><tr><th colspan="2">Input<span>{describe(old_text)}</span></th>'
        f'<th colspan="2">Expected<span>{describe(new_text)}</span></th></tr></thead>',
        "<tbody>",
        *rows,
        "</tbody></table></div>",
        "</section>",
    ])
    return section, summary == "unchanged"


def write_page(tests, path):
    tests = sorted(tests, key=lambda t: (t.parent.relative_to(INPUT).parts, t.name))
    sections = []
    groups = []
    for directory, members in itertools.groupby(tests, key=lambda t: t.parent):
        links = []
        for test in members:
            section, unchanged = test_html(test)
            sections.append(section)
            anchor = html.escape(test.relative_to(INPUT).as_posix())
            same = '<span class="same">unchanged</span>' if unchanged else ""
            links.append(f'<a href="#{anchor}" data-test="{anchor}">{html.escape(test.name)}{same}</a>')
        name = html.escape(directory.relative_to(INPUT).as_posix())
        groups.append(f'<div class="group"><div class="dir">{name}/</div>{"".join(links)}</div>')
    page = PAGE.read_text(encoding="utf-8")
    page = page.replace("<!-- COUNT -->", str(len(tests)))
    page = page.replace("<!-- CONTENTS -->", "\n".join(groups))
    page = page.replace("<!-- TESTS -->", "\n".join(sections))
    pathlib.Path(path).write_bytes(page.encode("utf-8"))
    print(f"wrote {len(tests)} tests to {path}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--cmd", help="the sc_format command, to run the tests")
    parser.add_argument("--override", action="store_true", help="write the results to expected/, or remove those equal to their input")
    parser.add_argument("--html", metavar="FILE", help="write a page showing the tests side by side, instead")
    parser.add_argument("tests", nargs="*", help="files or directories in input/, every test if none")
    args = parser.parse_args()
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    tests = tests_in(args.tests)
    if not tests:
        print("no tests found", file=sys.stderr)
        return 1
    if args.html:
        write_page(tests, args.html)
        return 0
    if not args.cmd:
        parser.error("--cmd is needed to run the tests")
    failures = [f for f in (run(args.cmd, t, args.override) for t in tests) if f]
    for failure in failures:
        print(failure, file=sys.stderr)
    print(f"{len(tests)} tests, {len(failures)} failed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
