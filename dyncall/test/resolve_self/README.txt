test for dynload library to resolve symbols from application image itself.
by Daniel Adler in 2010

Description
A test function is specified in the main application, that will be 
dynamically resolved on main entry of the test program.

The library path argument to dlLoadLibrary is set to NULL to 
specify the application image.

Background
self referencing is important on configurations where there is no
shared library but everything is statically linked.

Note that on some platforms, linker flags have to be used so symbol
names are in the binary, at first hand (e.g. --export-dynamic).

