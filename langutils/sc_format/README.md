# sc_format

Formats all sc code. 

## Overview

`format()` in `src/format.cpp` runs the steps below in order.

### 1. Parse
See `sc_parse`.
### 2. Find the verbatim lines

`collect_verbatim_lines()` (`src/verbatim_lines.cpp`) lists the lines that are not changed.

These are:

- the lines of anything the parser recovered from,
- the lines between `// sc-format off` to `// sc-format on`,
- when a `LineRange` is provided, every line outside it.

Now changes to verbatim lines is made.

### 3. Rewrite

`apply_rewrites()` (`src/rewrite/`) makes the options that change tokens.
There are many rewriters that do specific things, each returns edits to the text.

The rewrites run in rounds. 
A round makes a `LayoutDraft` of the current text, collects the edits, applies them, and parses the result again. 
The new AST must be the same as the old one, apart from the changes the round's rewriters allow in `AllowedAstChanges`. 
If it isn't, `format()` returns an `InternalError`. 
Otherwise the verbatim lines are moved past the edits, and the next round starts from the new text and AST.

`control_prefix` rewrites `x.if` into `if(x)`, it runs in rounds of its own until no more edits can be made.

### 4. Lay out and print

`lay_out_and_print()` (`src/layout/`) only changes whitespace.
The passes run in this order:

1. `make_layout_draft()` lists the tokens and the whitespace as written, and marks the tokens on verbatim lines.
2. `mark_syntax()` walks the AST once and returns `SyntaxMarks` describing what each token is, where list items and regions start, and relative indentation information.
3. `limit_blank_lines_and_add_final_newline()`.
4. `break_and_join_lines()` breaks and joins lines for the K&R options. It returns `TablesToAlign`.
5. `set_spaces_between_tokens()` sets the spaces between tokens on the same line.
6. `align_table_columns()` pads the lists into columns. It runs after spacing, as it measures the printed width.
7. `restore_verbatim_whitespace()` puts back the whitespace of the verbatim lines, which the passes before may have
   changed.
8. `print()` writes the tokens and the whitespace, and indents each line from the brackets open at its start and the
   marks.

Each pass takes what the passes before it return as parameters, so they can't run out of order.

### 5. Check

The result's tokens, whitespace aside, must be the same as those of the text after the rewrites. If they aren't,
`format()` returns an `InternalError`.


## Options

The options come from the nearest `.sc-format.scd` file. 

It is sclang code that evaluates to an event, as a `.quark` file does.

This must be constantly evaluated.

```
(
	final_newline: \one,
	trailing_whitespace: \remove,
	max_blank_lines: 2,
	space: (
		after_comma: \one,
		inside_parens: \none,
	),
	kr: \on,
)
```

The default value for the entries is `\keep`, which means no change.
An empty file or returning `nil` keeps everything.
`inherit: true` inherits from the (grand*)parent's directory and overrides.

The options named `group.option` are written with nested events, e.g., `space: (after_comma: \one)`.
A group can also be given one value directly, setting each of its options if valid.

| Option | Values other than `\keep` |
|---|---|
| `final_newline` | `\one`: exactly one newline at the end of the file. |
| `trailing_whitespace` | `\remove`: no whitespace at the end of a line, except inside strings and multi-line comments. |
| `max_blank_lines` | A number: at most this many blank lines in a row. |
| `unicode_spaces` | `\ascii`: Unicode spaces between tokens become ` `. |
| `indent` | `\tab` or a number of spaces: each line indented by its level. |
| `indent_method_chains` | `\on` or `\off`: a line starting with `.` one level more than the line its chain starts on, or not. |
| `indent_regions` | `\on` or `\off`: the inside of a top-level `( )` in an `.scd` file indented, or not. |
| `region_brackets` | `\own_lines`: a region's `(` and `)` each on a line of their own, `(a); (b);` becomes `(\na\n);\n(\nb\n);`. A comment after the `(` stays on its line. |
| `space.around_binary_operators` | `\one`: `a + b`, `a +.x b`. |
| `space.around_keyword_operators` | `\one`: `a min: b`. |
| `space.around_assignment` | `\one`: `a = 1`, `var a = 1`, `a[0] = 1`, `a.b = 1`. |
| `space.after_comma` | `\one`. |
| `space.before_comma` | `\none`. |
| `space.before_semicolon` | `\none`. |
| `space.inside_braces` | `\one` or `\none`: `{ 1 }`, `{1}`. |
| `space.in_empty_braces` | `\one` or `\none`: `{ }`, `{}`. |
| `space.before_parameters` | `\one` or `\none`: `{ \|a\|`, `{\|a\|`. |
| `space.after_parameters` | `\one` or `\none`: `\|a\| a`, `\|a\|a`. |
| `space.inside_parens` | `\none`: `f(1)`. |
| `space.inside_brackets` | `\none`: `[1, 2]`. |
| `space.inside_pipes` | `\none`: `\|a, b\|`. |
| `space.around_parameter_equals` | `\one` or `\none`: `\|a = 1\|`, `arg a=1;`. |
| `space.after_key` | `\one` or `\none`: `f(freq: 440)`, `(a: 1)`. |
| `space.around_dot` | `\none`: `a.b`. |
| `space.before_control_paren` | `\one` or `\none`: `if (x)`, `if(x)`, also `while`, `for`, `forBy`, `switch`, `case`. |
| `space.before_closure` | `\one` or `\none`: `x.do { }`, `if(x) { 1 } { 2 }`, `x.do{ }`. |
| `kr.braces` | `\on`: a multi-line block's `{` ends the line before, `x = {`, `if (x) {`, `} {`, and ends its own line, after its parameters: `{ a;\n\tb\n}` becomes `{\n\ta;\n\tb\n}`. A control message's functions after its `)` join the line before if one spans lines or each is a literal, `if(x)\n{ 1 }\n{ 2 }` becomes `if(x) { 1 } { 2 }`; else they stay on their lines, as one line could be long. Not a `switch` or `case`'s, see `multiline.control_pairs`. |
| `kr.parens`, `kr.brackets` | `\on`: the same for `(` and `[`, `f(`, `x = [`. |
| `multiline.arrays` | `\one_per_line`: an array with a line break before an element gets each element on its own line. |
| `multiline.arguments` | `\one_per_line`: the same for a message's arguments in `( )`. |
| `multiline.control_pairs` | `\one_per_line`: a `switch` or `case` over several lines gets each test and its action on a line of their own, and the default on its own: `switch(x)\n\t{ 1 } { a }\n\t{ b };`. |
| `parameters.on_brace_line` | `\on`: `{ \|a\|` and `{ arg a;` on the line of their `{`. |
| `parameters.pipes` | `\on` or `\one_line`: `arg a, b = 4.dbamp;` becomes `\|a, b = (4.dbamp)\|`, a default that isn't a literal in brackets, and `arg;` goes, except at the start of a file, where it makes the file a function. `\one_line` only changes a list on one line, and a newline inside a block comment counts. |
| `parameters.commas` | `\on`: `\|a b\|` becomes `\|a, b\|`. |
| `parameters.default_equals` | `\on`: `\|a 1\|` becomes `\|a = 1\|`, and `\|a(1)\|` becomes `\|a = (1)\|`. |
| `final_semicolon` | `\remove`: `{ a; b; }` becomes `{ a; b }`, in functions and methods, not at the top level of a file. |
| `region_semicolons` | `\insert`: a `;` between two top-level items of an `.scd` file where there is none, `(\na\n)\n(\nb\n)` becomes `(\na\n);\n(\nb\n)`, as the parser guesses. |
| `trailing_closures` | `\control` or `\all`: `if(c, { 1 }, { 2 })` becomes `if(c) { 1 } { 2 }`, and `x.do({ })` becomes `x.do { }`. `\control` is `if`, `while`, `for`, `forBy`, `switch`, `case`, `try`, `do`, and `collect`; `if`, `switch`, and `case` only when every argument after the receiver is a function, except that a `switch`'s literal tests go in braces, `switch(x, \a, { 1 })` becomes `switch(x) { \a } { 1 }`, where both compilers inline it either way (integer, float, character, boolean, and symbol tests, functions without parameters or `var`s). |
| `control_prefix` | `\on`: `x.if { }` becomes `if(x) { }`, and `(a > b).if({ }, { })` becomes `if(a > b, { }, { })`, for `if`, `while`, `for`, `forBy`, `switch`, `case`, and `try`, not `do` or `collect`. Before trailing closures, which then act on the call it makes. |
| `trailing_comma_lists` | `\expand`: a message's arguments, an array, or an event ending in a comma goes over lines: the opener ends its line, the closer starts one, and the elements go one per line, or as many per line as each line has if each has the same (the last fewer). An element is what commas separate, so `freq: 440` is one. `multiline.arrays: \one_per_line` wins for arrays. |
| `align_columns` | `\on`: the rows of a list `trailing_comma_lists` expands line up in columns, and so do comments ending them, e.g., `Pbind` pairs. |

The `parameters` options, `final_semicolon`, `region_semicolons`, `trailing_closures`, and `control_prefix` change tokens, so the result is parsed again
and must give the same AST, apart from a default's brackets, a control message's receiver, and a `switch` test's braces. Otherwise the file isn't formatted. 

A default that gains brackets can be folded to a constant in 3.16, with the same value.

Line endings always become `\n`.

From a `// sc-format off` comment to a `// sc-format on` comment, or the end of the file, the lines are left as
written. `/* sc-format off */` works too.

## Command line

```
sc_format file.scd                         # prints the result
sc_format -i dir/                          # rewrites every .sc and .scd file in place
sc_format -i --allow-unformatted dir/      # the same, but a file that can't be formatted only gets a message
sc_format --check dir/                     # lists the files that would change, exit code 1 if any
sc_format --verify dir/                    # checks that formatting twice changes nothing
sc_format --config options.scd dir/        # uses this options file
sc_format --set final_newline=one file.scd # sets an option, after the options file
sc_format --set kr.braces=on file.scd      # an option in a group
sc_format --set kr=on file.scd             # every option in a group
sc_format --kind sc < file                 # stdin, a class file
sc_format --lines 3:5 < file.scd           # changes only lines 3 to 5 of stdin
```
## API

```cpp
#include "sc_format/format.hpp"

const auto options = sc::format::find_style_file(path);
const auto style = options ? sc::format::read_style(*options) : sc::format::StyleOrError { sc::format::Style {} };
if (const auto* error = std::get_if<std::string>(&style))
    return report(*error);
const auto result = sc::format::format(text, sc::format::kind_of(path), std::get<sc::format::Style>(style));
if (const auto* formatted = std::get_if<sc::format::Text>(&result))
    return write(formatted->text);
```

The lines of what the parser skipped around a parse error are left as written,
`Text::unparsed_lines` lists them, and `Text::parse_error` holds the first error. `options_file_text()` writes a style
as an options file. With a `LineRange`, e.g., an editor's selection, only those lines change.

## Tests
A test in `tests/input/` with no file in `tests/expected/` must format to its input unchanged; the `sc_format_golden_override` target removes expected files that equal their input.
Build the `sc_format_golden_html` target, or run and then open `tests/test.py --html=page.html`, to see every test's input beside its expected output.
