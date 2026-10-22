// A multi-line comment that is never closed is a lexer error, which used to make region recovery loop forever.
// The errors are reported before the dump, as they are after the region.
1;
/* never
closed
