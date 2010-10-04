CLASS:: Foo
SUMMARY:: This is a great class!
RELATED:: Bar

This is normal prose before any section..

DESCRIPTION::
This is normal prose.
Here's another line with a link:: Link ::link that you can follow.

METHODS::
Here comes some methods:

CLASSMETHOD:: ar
Create a new instance
    ARGUMENT:: freq
        The Frequency
    ARGUMENT:: amp
        The Amplitude

CLASSMETHOD:: foo
This is a nonsense method.

SECTION:: This is a named section
With some emphasis>> normal << prose,
on multiple lines..

SUBSECTION:: And a subsection
With some prose...

EXAMPLES::
Here is some example code:
CODE::
  "in here, even tags like emphasis:: should be ignored".postln;
::CODE
But now we are back at normal prose.

