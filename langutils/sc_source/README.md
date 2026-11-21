# sc_source

All supercollider code is UTF8.

The first stage. 
It holds the text being compiled, its place in a file, and the character and location helpers that the
lexer and the diagnostics use. 
It depends only on `utf8proc`. Everything is in namespace `sc::src`.

## What is in it

| Header | Purpose |
|---|---|
| `normalise_source.hpp` | `NormalisedSource`, text with the line endings fixed. |
| `text_info.hpp` | `TextInfo`, the text plus where it came from. The input to the parser. |
| `text_location.hpp` | Locations and ranges, both in a snippet and in a whole file. |
| `codepoint.hpp` | UTF-8 decoding, and the SuperCollider character classes (`is_space`, `is_newline`, and so on). |
| `source_utils.hpp` | `CodePointIterator` and `LineIter`, for walking text forwards and backwards. |

## Normalising

`NormalisedSource` copies the text and changes `\r\n`, `\r`, `\v`, and `\f` into `\n`. 
After this, the only newline in the ASCII range is `\n`. 
The lexer only treats `\n` as an ASCII newline, so it would not end a line at a stray `\r`, `\v`, or `\f`.

Rewrite `\r\n` into a single byte will break the byteoffest of the diagnostic printing, but this is needed because we have multiline strings,
and their contents should not depend on operating system.
All IDEs should use `\n` as normalisation is really only there to support the older mac standard and not break existing windows code.


`is_newline` also accepts the Unicode newlines (`U+0085`, `U+2028`, and `U+2029`).  
This is different from the old lexer where they were essentially spaces.

These are not rewritten, as that would change the byte offsets of everything after them and are much rarer. 
IDE should display them as newlines, which the unicode spec recommends.
This could break code that had them inside a single line comment, but is highly unlikely.


## TextKind

There are three kinds, `ClassFile`, `ScdFile` and `InterpretedFunction`.

The first is self explanatory.
`ScdFile` is used by tools to understand an entire scd file.
`InterpretedFunction` is what the IDE sends when a user evaluates a function.

There is a complex issue here in that we need the parser to identify regions.
Therefore, the IDE sends to entire file to the tooling, which return the regions, then the IDE can identify what a region is, and send just that as an `InterpretedFunction`.


## TextInfo

`TextInfo` holds:
- `source`, the `NormalisedSource`.
- `source_start_in_file`, where this text starts in its file. It is zero for a whole file, and not zero for a region
  of a file.
- `file_path`, empty when the text has no file.
- `kind`, an enum, one of:

`TextInfo` is shared with `std::shared_ptr<const TextInfo>`. 
The lexer, parser, and diagnostics keep it alive.

`read(range)` returns the text for a range.

The constructor throws `std::length_error` for text over `max_source_size` (4 GiB minus 1), as locations use 32 bit
offsets.

## Locations

A location has three fields: `absolute` (a byte offset), `line_number` (from zero), and `codepoint_in_line` (a count of code points since the start of the
line, not bytes and not a visual column). 

There are two kinds.

| Type | Relative to |
|---|---|
| `SourceCodeLocation`, `SourceCodeRange` | The start of the `TextInfo`'s text. This is what the lexer and parser produce. |
| `FileCodeLocation`, `FileCodeRange` | The start of the file. |

They are needed to determine if we are evaluating a whole file, or just a little peice of one.

For a whole file they are equal. 

They differ for a region of a file, where `TextInfo::source_start_in_file` is the offset.

`FileCodeRange::sourceToFile(start, range)` converts one to the other.

Ordering of locations (`<`, `>`, `<=`, and `>=`) uses `absolute` only. 
`==` compares all three fields. 
Ranges use `range(a, b)` to join the start of one with
the end of another, and `flatten()` for a one byte range at the start, which is used to point at a position.

## Code points

`utf8_sequence_to_codepoint` returns the code point and the bytes used. 
On invalid UTF-8 it returns `invalid_utf8_flag` and one byte, so a caller can skip it. 
Past the end of the text it returns `invalid_utf8_flag` and zero bytes. 
The ASCII case is inline, as the lexer calls it for every character. 

`codepoint_width` guesses the display width. 
The real width depends on font and locale. 
Tabs cannot be guessed, use `codepoint_as_whitespace`.

The diagnostics use these to line up the highlights under the source. 
