// emacs:		-*- c++ -*-
// file:		SC_StringParser.h
// copyright:	2003-2006 stefan kersten
// cvs:			$Id$

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
// USA

#ifndef _SC_StringParser_
#define _SC_StringParser_

#define SC_MAX_TOKEN_LENGTH 256

#ifdef _WIN32
	#define SC_STRPARSE_PATHDELIMITER ';'
#else
	#define SC_STRPARSE_PATHDELIMITER ':'
#endif

class SC_StringParser
{
	const char	*mSpec, *mStart, *mEnd;
	char		mSep, mBuf[SC_MAX_TOKEN_LENGTH];

public:
	SC_StringParser();
	SC_StringParser(const char *spec, char sep);

	bool AtEnd() const;
	const char *NextToken();
};

#endif

