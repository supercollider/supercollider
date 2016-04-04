Jackey
======

Jackey (jack-key) is a simple define-only header that serves as an ad-hoc
standard for Jack Metadata property keys.  Using a single header for this
coordination makes it simple to discover, use, and extend the keys used by Jack
clients.

It is not necessary to use this header, since keys are defined by their URI
string, but developers are encouraged to use and contribute to this header to
promote interoperability in the Jack ecoystem.

There are no rules about the namespaces (e.g. domains) used for keys here, but
developers SHOULD NOT define keys in others' domains, with the exception of
http://jackaudio.org/metadata assuming the proposed key is not defined by Jack
itself, or the master jackey.h.

 -- David Robillard <d@drobilla.net>
