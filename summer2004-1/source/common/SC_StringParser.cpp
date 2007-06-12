//  copyright 2003 stefan kersten <steve@k-hornz.de>
//
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#include "SC_StringParser.h"
#include "SC_BoundsMacros.h"
#include <string.h>
#include <stdlib.h>

SC_StringParser::SC_StringParser()
	: mSpec(0), mStart(0), mEnd(0), mSep(0)
{
}

SC_StringParser::SC_StringParser(char *spec, char sep)
	: mSpec(spec), mStart(0), mEnd(0), mSep(sep)
{
	if (mSpec) {
		size_t len = strlen(mSpec);
		if (len > 0) {
			mStart = mSpec;
			mEnd = mStart + len;
		} else {
			mSpec = 0;
		}
	}
}

bool SC_StringParser::AtEnd() const
{
	return mSpec == 0;
}

const char *SC_StringParser::NextToken()
{
	if (mSpec) {
		char *end = strchr(mStart, mSep);
		if (end == 0) {
			end = mEnd;
		}

		size_t len = sc_min(SC_MAX_TOKEN_LENGTH-1, end-mStart);
		memcpy(mBuf, mStart, len);
		mBuf[len] = '\0';

		if (end == mEnd) {
			mSpec = 0;
		} else {
			mStart = end+1;
		}

		return mBuf;
	}

	return 0;
}

// EOF
