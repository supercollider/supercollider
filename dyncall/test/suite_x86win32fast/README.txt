dyncall stress test suite: testing standard C calls
===================================================
Copyright (c) 2007-2009 Daniel Adler <dadler@uni-goettingen.de>, 
                        Tassilo Philipp <tphilipp@potion-studios.com>

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


requirements:
  python (for preprocessing)

configuration:
  edit "nargs" in mkcase.py and run
  
    make distclean 
    make 
    
  on shell.
  

usage:

  suite         	        - run full range
  suite <id> 			- run test id <id>
  suite <from> <to>		- run test id <from> to id <to> (inclusive).

  see test details in case.h


description:

  the test invokes 'void'-functions that effectively overwrite global 
  variables in a structured way.
  
  functions are identified by an id (a number starting at 0).
  
  the implementation of each function does
  
    sets the gID to its id (which is compiled in) 
  
    sets all arguments into type-specific value vectors at the given position
  

   
  
  
