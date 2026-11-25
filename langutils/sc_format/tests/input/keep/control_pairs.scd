// With keep, a switch over lines stays as written.
f = { |x|
	// From the arguments, with line breaks.
	switch(x, \low, { 1 }, \mid, {
		2
	}, { 3 });
	// On one line: as written.
	switch(x, \low, { 1 }, \mid, { 2 }, { 3 });
	switch(x) { \low } { 1 } { 3 };
	// Pairs split over lines, or on one line.
	switch(x)
	{ \a } { 1 }
	{ \b }
	{ 2 }
	{ 3 };
	switch(x) { \a } { 1 } { \b } {
		2
	} { 3 };
	// A receiver.
	x.switch(\a, { 1 },
		{ 2 });
	// No default.
	switch(x) { \a } {
		1
	} { \b } { 2 };
	// case.
	case
	{ x > 1 } { 1 }
	{ x < 0 } {
		2
	}
	{ 3 };
	case { x > 1 } { 1 } { x < 0 } {
		2
	};
	case { x } { 1 } { 2 };
	// Comments stay where they are.
	switch(x)
	{ \a } // first
	{ 1 }
	{ \b } { 2 } // second
	{ 3 };
	// Not when the switch keeps its arguments.
	switch(x, "a", { 1 },
		"b", { 2 });
	x
};
