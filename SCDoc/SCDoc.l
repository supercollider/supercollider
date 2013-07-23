%{
/************************************************************************
*
* Copyright 2012 Jonatan Liljedahl <lijon@kymatica.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#include <string.h>
#include "SCDoc.h"
#include "SCDoc.tab.hpp"

int scdoc_start_token = 0;
//int colnum;
//#define YY_USER_ACTION { scdoclloc.first_column = colnum; colnum=colnum+yyleng; scdoclloc.last_column=colnum; if(scdoclloc.first_line!=yylineno) colnum=1; scdoclloc.first_line = scdoclloc.last_line = yylineno;}

static int method_caller;

%}

%option noyywrap never-interactive nounistd yylineno

%x verbatim
%x verbatim2

%x metadata
%x eat
%x eat2
%x eat3

%x method

%%

%{

if (scdoc_start_token) {
    int t = scdoc_start_token;
    scdoc_start_token = 0;
//    colnum = 1;
    if(t==START_METADATA) BEGIN(eat);
    return t;
}

%}

(?i:[ \t]*class::[ \t]*)         return CLASS;
(?i:[ \t]*title::[ \t]*)         return TITLE;
(?i:[ \t]*summary::[ \t]*)       return SUMMARY;
(?i:[ \t]*related::[ \t]*)       return RELATED;
(?i:[ \t]*categories::[ \t]*)    return CATEGORIES;
(?i:[ \t]*redirect::[ \t]*)      return REDIRECT;

(?i:[ \t]*classtree::[ \t]*)     return CLASSTREE;
(?i:[ \t]*keyword::[ \t]*)       return KEYWORD;

(?i:[ \t]*private::[ \t]*)       return PRIVATE;
(?i:[ \t]*section::[ \t]*)       return SECTION;
(?i:[ \t]*subsection::[ \t]*)    return SUBSECTION;
(?i:[ \t]*copymethod::[ \t]*)    return COPYMETHOD;
(?i:[ \t]*method::[ \t]*)        method_caller = YY_START; BEGIN(method); return METHOD;
(?i:[ \t]*argument::[ \t]*)      return ARGUMENT;

(?i:[ \t]*description::[ \t\n\r]*)   return DESCRIPTION;
(?i:[ \t]*classmethods::[ \t\n\r]*)  return CLASSMETHODS;
(?i:[ \t]*instancemethods::[ \t\n\r]*) return INSTANCEMETHODS;
(?i:[ \t]*examples::[ \t\n\r]*)      return EXAMPLES;

(?i:[ \t]*returns::[ \t\n\r]*)       return RETURNS;
(?i:[ \t]*discussion::[ \t\n\r]*)    return DISCUSSION;

(?i:[ \t]*list::[ \t\n\r]*)          return LIST;
(?i:[ \t]*tree::[ \t\n\r]*)          return TREE;
(?i:[ \t]*numberedlist::[ \t\n\r]*)  return NUMBEREDLIST;
(?i:[ \t]*definitionlist::[ \t\n\r]*) return DEFINITIONLIST;
(?i:[ \t]*table::[ \t\n\r]*)         return TABLE;
(?i:[ \t]*footnote::[ \t\n\r]*)      return FOOTNOTE;
(?i:[ \t]*warning::[ \t\n\r]*)       return WARNING;
(?i:[ \t]*note::[ \t\n\r]*)          return NOTE;

(?i:link::[ \t]*)           BEGIN(verbatim); return LINK;
(?i:anchor::[ \t]*)         BEGIN(verbatim); return ANCHOR;
(?i:image::[ \t]*)          BEGIN(verbatim); return IMAGE;
(?i:soft::[ \t\n\r]*)         BEGIN(verbatim); return SOFT;
(?i:strong::[ \t\n\r]*)       BEGIN(verbatim); return STRONG;
(?i:emphasis::[ \t\n\r]*)     BEGIN(verbatim); return EMPHASIS;
(?i:code::[ \t]*)           BEGIN(verbatim); return CODE;
(?i:teletype::[ \t]*)       BEGIN(verbatim); return TELETYPE;

(?i:[ \t]*code::[ \t]*\n+)      BEGIN(verbatim2); return CODEBLOCK;
(?i:[ \t]*teletype::[ \t]*\n+)  BEGIN(verbatim2); return TELETYPEBLOCK;

<INITIAL,verbatim>[ \t\n\r]*::    BEGIN(0); return TAGSYM;
<verbatim2>\n[ \t\n\r]*::         BEGIN(0); return TAGSYM;
<verbatim2>\n[ \t]*\\::           scdoclval.str = strdup("\n::"); return TEXT;
[ \t]*\|\|[ \t\n\r]*              return BARS;
[ \t]*\#\#[ \t\n\r]*              return HASHES;

<verbatim2>\n   scdoclval.str = strdup("\n"); return TEXT;
<verbatim>\n+   scdoclval.str = strdup(" "); return TEXT;
\n              return NEWLINE;
\n([ \t\r]*\n)+           return EMPTYLINES;

<INITIAL,method>[ \t]*,[ \t]*           scdoclval.str = strdup(scdoctext); return COMMA;

\\\|\|                  scdoclval.str = strdup("||"); return TEXT;
\\\#\#                  scdoclval.str = strdup("##"); return TEXT;
<INITIAL,verbatim>\\::  scdoclval.str = strdup("::"); return TEXT;
<verbatim,verbatim2>\t  scdoclval.str = strdup("    "); return TEXT;
[ \t]+                scdoclval.str = strdup(" "); return TEXT;

<*>\r                   /* eat this */

[a-zA-Z]+:\/\/[^ \t\n\r:,]+       scdoclval.str = strdup(scdoctext); return URL;
<method>[a-z][a-zA-Z0-9_]*|[-<>@|&%*+/!?=]+ scdoclval.str = strdup(scdoctext); return METHODNAME;
<method>\([^()]+\)              scdoclval.str = strdup(scdoctext); return METHODARGS;
<method>[ \r\t]+                /* eat this */
<method>\n                      BEGIN(method_caller); return NEWLINE;
<method>.                       return BAD_METHODNAME;

[a-zA-Z]+                       |
<INITIAL,verbatim,verbatim2>[.!?(){}\[\]'"0-9]+         |
<verbatim,verbatim2>[^:\\\t\n\r ]+ |
[^:\\\n\r]                        |
<INITIAL,verbatim,verbatim2>.    scdoclval.str = strdup(scdoctext); return TEXT;

<metadata,eat>(?i:[ \t]*class::[ \t]*)         BEGIN(metadata); return CLASS;
<metadata,eat>(?i:[ \t]*title::[ \t]*)         BEGIN(metadata); return TITLE;
<metadata,eat>(?i:[ \t]*summary::[ \t]*)       BEGIN(metadata); return SUMMARY;
<metadata,eat>(?i:[ \t]*related::[ \t]*)       BEGIN(metadata); return RELATED;
<metadata,eat>(?i:[ \t]*categories::[ \t]*)    BEGIN(metadata); return CATEGORIES;
<metadata,eat>(?i:[ \t]*redirect::[ \t]*)      BEGIN(metadata); return REDIRECT;
<metadata,eat>(?i:[ \t]*classtree::[ \t]*)     BEGIN(metadata); return CLASSTREE;
<metadata,eat>(?i:[ \t]*keyword::[ \t]*)       BEGIN(metadata); return KEYWORD;
<metadata,eat>(?i:[ \t]*private::[ \t]*)       BEGIN(metadata); return PRIVATE;
<metadata,eat>(?i:[ \t]*section::[ \t]*)       BEGIN(metadata); return SECTION;
<metadata,eat>(?i:[ \t]*subsection::[ \t]*)    BEGIN(metadata); return SUBSECTION;
<metadata,eat>(?i:[ \t]*copymethod::[ \t]*)    BEGIN(metadata); return COPYMETHOD;
<metadata,eat>(?i:[ \t]*method::[ \t]*)        method_caller = YY_START; BEGIN(method); return METHOD;
<metadata,eat>(?i:[ \t]*description::[ \t\n\r]*)   BEGIN(eat); return DESCRIPTION;
<metadata,eat>(?i:[ \t]*classmethods::[ \t\n\r]*)  BEGIN(eat); return CLASSMETHODS;
<metadata,eat>(?i:[ \t]*instancemethods::[ \t\n\r]*) BEGIN(eat); return INSTANCEMETHODS;
<metadata,eat>(?i:[ \t]*examples::[ \t\n\r]*)      BEGIN(eat); return EXAMPLES;
<metadata>\n    BEGIN(eat); return NEWLINE;
<metadata>[ \t]*,[ \t]*           scdoclval.str = strdup(scdoctext); return COMMA;
<metadata>[a-zA-Z]+ |
<metadata>[0-9]+    |
<metadata>[^:\\\n\r,]+     |
<metadata>.         scdoclval.str = strdup(scdoctext); return TEXT;
<metadata>\\::      scdoclval.str = strdup("::"); return TEXT;
<eat>(?i:link::[ \t]*)           BEGIN(eat2);
<eat>(?i:anchor::[ \t]*)         BEGIN(eat2);
<eat>(?i:image::[ \t]*)          BEGIN(eat2);
<eat>(?i:soft::[ \t\n\r]*)         BEGIN(eat2);
<eat>(?i:strong::[ \t\n\r]*)       BEGIN(eat2);
<eat>(?i:emphasis::[ \t\n\r]*)     BEGIN(eat2);
<eat>(?i:code::[ \t]*)           BEGIN(eat2);
<eat>(?i:teletype::[ \t]*)       BEGIN(eat2);
<eat>(?i:[ \t]*code::[ \t]*\n+)      BEGIN(eat3);
<eat>(?i:[ \t]*teletype::[ \t]*\n+)  BEGIN(eat3);
<eat2>[ \t\n\r]*::            BEGIN(eat);
<eat3>\n[ \t\n\r]*::         BEGIN(eat);
<eat,eat2,eat3>.|\n          /* empty */
%%