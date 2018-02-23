@ECHO OFF
REM ///////////////////////////////////////////////////////////////////////////
REM
REM Copyright (c) 2007-2009 Daniel Adler <dadler@uni-goettingen.de>, 
REM                         Tassilo Philipp <tphilipp@potion-studios.com>
REM
REM Permission to use, copy, modify, and distribute this software for any
REM purpose with or without fee is hereby granted, provided that the above
REM copyright notice and this permission notice appear in all copies.
REM
REM THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
REM WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
REM MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
REM ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
REM WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
REM ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
REM OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
REM
REM ///////////////////////////////////////////////////////////////////////////

REM We need at least 2 characters in the path.
IF [%DEVKITPRO:~0,1%]==[%DEVKITPRO%] (
	GOTO END_ABORT
)

REM Extract devkitPro path.
IF [%DEVKITPRO:~0,1%]==[/] (
	ECHO DEVKITPRO_PATH=%DEVKITPRO:~1,1%:%DEVKITPRO:~2%#>>ConfigVars
) ELSE IF [%DEVKITPRO:~1,1%]==[:] (
	ECHO DEVKITPRO_PATH=%DEVKITPRO%#>>ConfigVars
) ELSE (
	GOTO END_ABORT
)

GOTO END

:END_ABORT
ECHO ERROR: Environment variable DEVKITPRO must be set to absolute devkitPro path.
DEL ConfigVars

:END
