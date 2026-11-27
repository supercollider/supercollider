# sc_diagnostic

Every error and warning the compiler can report and the code that prints them with the source marked. 
It depends on [sc_source](../sc_source/README.md) (and on [sc_lexer](../sc_lexer/README.md) and sc_util inside its `.cpp` files). 

Everything is in namespace `sc::diag`.

The stages (lexer, parser, sema, compiler) return `Diagnostic`s rather than print.

Colour styles can be used when appropriate.

The output of these is ***heavily*** inspired by rust.

## What is in it

| Header | Purpose |
|---|---|
| `sc_diagnostic.hpp` | `Diagnostic`, `Severity`, and one factory function per kind of diagnostic. |
| `style.hpp` | `Style`, and `PlainStyle`, `AnsiStyle`, and `HtmlStyle`: how the printed output is decorated. |
| `details/highlight.hpp` | `streamHighlights`, which prints source text with ranges marked. `Diagnostic` uses it, and it can be used alone. |

## A diagnostic

A `Diagnostic` is:
- a **name**, the title line (`"Duplicate Class definition."`). It is a `const char*`, so it must be a string literal.
- a **severity**, `Error` or `Warning`. `fatal()` is true for errors.
- a list of **locations**. Each has a `TextInfoPtr` (the text and where it came from), a `SourceCodeRange`, and a message
  printed next to the range.
- optional **extra** text, printed under the source for longer explanations.

Locations are ordered, and the **last one is the true location**. 
Earlier ones give context, so they usually read as a pair: "Duplicate here..." then "...and here."
`within(text, range)` checks the true location only. 
It is how callers ask "did this diagnostic come from this piece of code?".

Locations can be in different texts, so a diagnostic can point from one file into another.

## Printing

```cpp
std::cerr << diagnostic;                    // paths are file:// URIs
std::string s = diagnostic.to_string(root); // paths are relative to root
```

Use the second form in tests, so the output does not depend on where the source is checked out.

### Colour and formatting

The printer never writes escape codes. It writes text with a `Role` (`Error`, `Path`, `LineNumber`, `Gutter`, `Source`, ...)
and a `Style` decides what that looks like.

```cpp
std::cerr << diagnostic.to_string(sc::diag::AnsiStyle {});  // terminal colours
page += diagnostic.to_string(sc::diag::HtmlStyle {}, root);       // <pre class="sc-diagnostic"> with <span class="sc-...">
```

- `PlainStyle` is the default. It adds nothing, and is what the regression tests compare.
- `AnsiStyle` uses the 16 colour SGR codes. The library does not check for a terminal or `NO_COLOR`; the caller does.
- `HtmlStyle` gives each role a CSS class and escapes `& < > "` in all source and messages. The colours are up to the CSS.

Adding a new one is done by subclassing.


## How to add a diagnostic

1. Declare a factory function in `sc_diagnostic.hpp`. It takes the `TextInfoPtr` and `SourceCodeRange` for each place
   to mark, plus anything the message needs. Mark it `[[nodiscard]]`.
2. Define it in `src/impl.cpp`, returning `{ name, severity, locations, extra }`.
3. Call it from the stage that found the problem.
4. Add a regression input. See below.

Guidelines for the message:
- The name is short and says what is wrong. The location messages say what to do, if a quick fix is known.
- Keep the location message short, use the extra message for longer output.
- Use `Warning` only if the code still compiles to something meaningful — often used in legacy.

## Known problems

- `DiagnosticKind` and `Severity` are partly unused, the name string is what identifies a diagnostic.
- Some messages were written quickly (by AI) and read badly. As long as the diagnostic is emitted in the correct place and
  highlights the right code, the wording is trivial to change.

## Tests

`test/tests.cpp` is a Boost test of the highlighting. Run it with:

```
ctest --test-dir build -R langutils.sc_diagnostic_test_run
```

The text of each diagnostic is tested by the error regression suite in [tests/error_regression](../tests/error_regression),
which runs the compiler on small inputs and compares the printed diagnostics. 
Add an input there for every new diagnostic. 
If the output changes on purpose, regenerate the expected files with the suite's `test.py --override`, and never edit them by hand.
