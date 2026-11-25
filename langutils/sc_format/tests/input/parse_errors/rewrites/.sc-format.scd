// Every rewrite on. A rewrite that would change a line the parser skipped doesn't happen.
(
	inherit: true,
	parameters: (pipes: \on, commas: \on, default_equals: \on),
	control_prefix: \on,
	region_semicolons: \insert,
)
