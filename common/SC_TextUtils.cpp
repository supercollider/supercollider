/*
	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com
	Copyright (c) 2012 Tim Blechmann. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "SC_TextUtils.hpp"
#include <cstring>

#define OPENCURLY '{'
#define CLOSCURLY '}'

int rtf2txt(char* txt)
{
	int rdpos=0, wrpos=0;
	char c;
	if (strncmp(txt,"{\\rtf",5)!=0) return 0;  // OK, not an RTF file
text:
	switch (txt[wrpos]=txt[rdpos++])
	{
	case 0:
		/*{
					char fname[32];
					sprintf(fname, "rtf2txt_out%d.txt", bugctr++);
					FILE *fp = fopen(fname, "w");
					fwrite(txt,wrpos,1,fp);
					fclose(fp);
				}*/
		return wrpos;
	case OPENCURLY:
	case CLOSCURLY:
	case '\n': goto text;
	case '\\':
		if (strncmp(txt+rdpos,"fonttbl",7)==0
			|| strncmp(txt+rdpos,"filetbl",7)==0
			|| strncmp(txt+rdpos,"colortbl",8)==0
			|| strncmp(txt+rdpos,"stylesheet",10)==0
			)
		{
			int level = 1;
			while(level && (c=txt[rdpos++]) != 0) {
				if (c == OPENCURLY) level++;
				else if (c == CLOSCURLY) level--;
			}
		} else if (strncmp(txt+rdpos,"\'a0",3)==0 || (strncmp(txt+rdpos,"\'A0",3)==0))
		{
			txt[wrpos++] = ' '; rdpos = rdpos + 3;
		} else {
			if (txt[rdpos]==CLOSCURLY || txt[rdpos]==OPENCURLY
				|| txt[rdpos]=='\\' || txt[rdpos]=='\t'|| txt[rdpos]=='\n')
			{ txt[wrpos++] = txt[rdpos++]; goto text; }
			if (strncmp(txt+rdpos,"tab",3)==0) { txt[wrpos++] = '\t'; }
			if (strncmp(txt+rdpos,"par",3)==0) { txt[wrpos++] = '\n'; }

			while((c=txt[rdpos++]) && c!=' ' && c!='\\');
			if (c=='\\') rdpos--;
		}
		goto text;
	default :
		wrpos++;
		goto text;
	}
}

// strips HTML down to plaintext tags in a fairly simple-minded way
int html2txt(char* txt)
{
	int rdpos=-1, wrpos=0, bodypos=-1;
	bool intag = false;

	// First check if we can find a BODY tag to start at
	while(bodypos == -1 && txt[++rdpos] != 0){
		if(strncmp(txt+rdpos, "<body", 5) == 0) // FIXME: should be case-insensitive, ideally
			bodypos = rdpos;
	}
	if(bodypos != -1)
		rdpos = bodypos;
	else
		rdpos = 0;

	// Now we start from our start, and add the non-tag text to the result
	while(txt[rdpos] != 0){
		if(intag){
			if(txt[rdpos++] == '>')
				intag = false;
		}else{
			if(txt[rdpos] == '<'){
				intag = true;
				++rdpos;
			}else{
				/*
				if(strncmp(txt+rdpos, "&amp;", 5)==0){
					txt[wrpos++] = '&';
					rdpos += 5;
				}else if(strncmp(txt+rdpos, "&nbsp;", 6)==0){
					txt[wrpos++] = ' ';
					rdpos += 6;
				}else if(strncmp(txt+rdpos, "&lt;", 4)==0){
					txt[wrpos++] = '<';
					rdpos += 4;
				}else if(strncmp(txt+rdpos, "&gt;", 4)==0){
					txt[wrpos++] = '>';
					rdpos += 4;
				}else{
				*/
					txt[wrpos++] = txt[rdpos++];
				//}
			}
		}
	}
	txt[wrpos] = 0;
	return wrpos;
}

