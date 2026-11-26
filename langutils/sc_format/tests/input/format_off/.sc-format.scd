// From a `sc-format off` comment to a `sc-format on` comment, or the end of the file, the lines stay as written.
(
	indent: \tab,
	space: (around_assignment: \one, around_binary_operators: \one, after_comma: \one, inside_brackets: \none),
	multiline: (arrays: \one_per_line),
	parameters: (pipes: \on),
	trailing_closures: \control,
	final_semicolon: \remove,
)
