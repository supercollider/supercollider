// Around a parse error, the lines of what the parser recovered from stay as written.
(
	indent: \tab,
	space: (around_assignment: \one, around_binary_operators: \one, after_comma: \one),
	parameters: (pipes: \on),
	trailing_closures: \control,
	final_semicolon: \remove,
)
