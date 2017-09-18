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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SCDoc.h"

//#define YYLEX_PARAM &yylval, &yylloc

int scdocparse();

extern int scdoclineno;
extern char *scdoctext;
extern int scdoc_start_token;
extern FILE *scdocin;
//extern struct YYLTYPE scdoclloc;

//int scdoc_metadata_mode;

static const char * method_type = NULL;

static DocNode * topnode;

void scdocerror(const char *str);

extern void error(const char *fmt, ...);
extern void post(const char *fmt, ...);

static inline bool stringEqual(const char * a, const char * b)
{
    return strcmp(a, b) == 0;
}

%}
%locations
%error-verbose
%union {
    intptr_t i;
    const char *id;
    char *str;
    DocNode *doc_node;
}
// single line header tags that take text
%token CLASS TITLE SUMMARY RELATED CATEGORIES REDIRECT
// single line body tags that take text
%token CLASSTREE COPYMETHOD KEYWORD PRIVATE
// single line structural tags that take text, with children
%token SECTION SUBSECTION METHOD ARGUMENT
// single line structural tags with no text, with children
%token DESCRIPTION CLASSMETHODS INSTANCEMETHODS EXAMPLES RETURNS DISCUSSION
// nestable range tags with no text, with children
%token LIST TREE NUMBEREDLIST DEFINITIONLIST TABLE FOOTNOTE NOTE WARNING
// modal range tags that take multi-line text
%token CODE LINK ANCHOR SOFT IMAGE TELETYPE STRONG EMPHASIS
%token CODEBLOCK "CODE block" TELETYPEBLOCK "TELETYPE block"
// symbols
%token TAGSYM "::" BARS "||" HASHES "##"
// text and whitespace
%token <str> TEXT "text" URL COMMA METHODNAME "method name" METHODARGS "arguments string"
%token NEWLINE "newline" EMPTYLINES "empty lines"
%token BAD_METHODNAME "bad method name"

%token END 0 "end of file"

%type <id> headtag sectiontag listtag rangetag inlinetag blocktag
%type <str> anyword words anywordnl wordsnl anywordurl words2 nocommawords optMETHODARGS methodname
%type <doc_node> document arg optreturns optdiscussion body bodyelem
%type <doc_node> optsubsections optsubsubsections methodbody
%type <doc_node> dochead headline optsections sections section
%type <doc_node> subsections subsection subsubsection subsubsections
%type <doc_node> optbody optargs args listbody tablebody tablecells tablerow
%type <doc_node> prose proseelem blockA blockB commalist
%type <doc_node> deflistbody deflistrow defterms methnames

%token START_FULL START_PARTIAL START_METADATA

%start start

%destructor { doc_node_free_tree($$); } <doc_node>
%destructor { free($$); } <str>

%{
//int scdoclex (YYSTYPE * yylval_param, struct YYLTYPE * yylloc_param );
int scdoclex (void);
%}

%%

start: document { topnode = $1; }
     | document error { topnode = NULL; doc_node_free_tree($1); }
     ;

document: START_FULL eateol dochead optsections
    {
        $$ = doc_node_create("DOCUMENT");
        doc_node_add_child($$, $3);
        doc_node_add_child($$, $4);
    }
       | START_PARTIAL sections
    {
        $$ = doc_node_make_take_children("BODY",NULL,$2);
    }
       | START_METADATA dochead optsections
    {
        $$ = doc_node_create("DOCUMENT");
        doc_node_add_child($$, $2);
        doc_node_add_child($$, $3);
    }
;

eateol: eol
      | /* empty */
      ;

dochead: dochead headline { $$ = doc_node_add_child($1,$2); }
       | headline { $$ = doc_node_make("HEADER",NULL,$1); }
;

headline: headtag words2 eol { $$ = doc_node_make($1,$2,NULL); }
        | CATEGORIES commalist eol { $$ = doc_node_make_take_children("CATEGORIES",NULL,$2); }
        | RELATED commalist eol { $$ = doc_node_make_take_children("RELATED",NULL,$2); }
;

headtag: CLASS { $$ = "TITLE"; } /* no need for the separate class:: tag actually. */
       | TITLE { $$ = "TITLE"; }
       | SUMMARY { $$ = "SUMMARY"; }
       | REDIRECT { $$ = "REDIRECT"; }
;

sectiontag: CLASSMETHODS { $$ = "CLASSMETHODS"; method_type = "CMETHOD"; }
          | INSTANCEMETHODS { $$ = "INSTANCEMETHODS"; method_type = "IMETHOD"; }
          | DESCRIPTION { $$ = "DESCRIPTION"; method_type = "METHOD"; }
          | EXAMPLES { $$ = "EXAMPLES"; method_type = "METHOD"; }
;

optsections: sections
           | { $$ = doc_node_make("BODY",NULL,NULL); }
;

sections: sections section { $$ = doc_node_add_child($1,$2); }
        | section { $$ = doc_node_make("BODY",NULL,$1); }
        | subsubsections { $$ = doc_node_make_take_children("BODY",NULL,$1); } /* allow text before first section */
;

section: SECTION { method_type = "METHOD"; } words2 eol optsubsections { $$ = doc_node_make_take_children("SECTION",$3,$5); }
       | sectiontag optsubsections { $$ = doc_node_make_take_children($1, NULL,$2); }
;

optsubsections: subsections
              | { $$ = NULL; }
;

subsections: subsections subsection { $$ = doc_node_add_child($1,$2); }
           | subsection { $$ = doc_node_make("(SUBSECTIONS)",NULL,$1); }
           | subsubsections
;

subsection: SUBSECTION words2 eol optsubsubsections { $$ = doc_node_make_take_children("SUBSECTION", $2, $4); }
;

optsubsubsections: subsubsections
                 | { $$ = NULL; }
;

subsubsections: subsubsections subsubsection { $$ = doc_node_add_child($1,$2); }
              | subsubsection { $$ = doc_node_make("(SUBSUBSECTIONS)",NULL,$1); }
              | body { $$ = doc_node_make_take_children("(SUBSUBSECTIONS)",NULL,$1); }
; 

subsubsection: METHOD methnames optMETHODARGS eol methodbody
    {
        $2->id = "METHODNAMES";
        $$ = doc_node_make(method_type,$3,$2);
        doc_node_add_child($$, $5);
//        doc_node_add_child($2, $3);
    }
             | COPYMETHOD words eol {
                if ( !(strchr($2, ' ')) ) {
                  yyerror("COPYMETHOD requires 2 arguments (class name and method name)");
                  YYERROR;
                }

                $$ = doc_node_make(
                stringEqual(method_type, "CMETHOD") ? "CCOPYMETHOD"
                                                    : (stringEqual(method_type, "IMETHOD") ? "ICOPYMETHOD"
                                                                                           : "COPYMETHOD"),
                $2, NULL
                ); }
             | PRIVATE commalist eoleof { $$ = doc_node_make_take_children( stringEqual(method_type, "CMETHOD") ? "CPRIVATE"
                                                                                                                : "IPRIVATE",
                NULL, $2); }
;

optMETHODARGS: { $$ = NULL; }
             | METHODARGS
    {
//        $$ = doc_node_make("ARGSTRING",$1,NULL);
        $$ = $1;
        if(!stringEqual(method_type, "METHOD")) {
            yyerror("METHOD argument string is not allowed inside CLASSMETHODS or INSTANCEMETHODS");
            YYERROR;
        }
    }
;

methodname: METHODNAME
    {
        char *p = $1+strlen($1)-1;
        if(*p=='_') {
            post("WARNING: SCDoc: In %s\n  Property setter %s should be documented without underscore.\n", scdoc_current_file, $1);
            *p = '\0';
        };
        $$ = $1;
    }
;

methnames: methnames COMMA methodname { free($2); $2 = NULL; $$ = doc_node_add_child($1, doc_node_make("STRING",$3,NULL)); }
         | methodname { $$ = doc_node_make("(METHODNAMES)",NULL,doc_node_make("STRING",$1,NULL)); }
;

methodbody: optbody optargs optreturns optdiscussion
    {
        $$ = doc_node_make_take_children("METHODBODY",NULL,$1);
        doc_node_add_child($$, $2);
        doc_node_add_child($$, $3);
        doc_node_add_child($$, $4);
    }
;

optbody: body
       | { $$ = NULL; }
;

optargs: args
       | { $$ = NULL; }
;

args: args arg { $$ = doc_node_add_child($1,$2); }
    | arg { $$ = doc_node_make("ARGUMENTS",NULL,$1); }
;

arg: ARGUMENT words eol optbody { $$ = doc_node_make_take_children("ARGUMENT", $2, $4); }
   | ARGUMENT eol body { $$ = doc_node_make_take_children("ARGUMENT", NULL, $3); }
;

optreturns: RETURNS body { $$ = doc_node_make_take_children("RETURNS",NULL,$2); }
          | { $$ = NULL; }
;

optdiscussion: DISCUSSION body { $$ = doc_node_make_take_children("DISCUSSION",NULL,$2); }
             | { $$ = NULL; }
;

/*
body contains a list of bodyelem's (A) and prose (B) such that
the list can start and end with either A or B, and A can repeat while B can not
*/

body: blockA
    | blockB
    ;

blockA: blockB bodyelem { $$ = doc_node_add_child($1,$2); }
      | blockA bodyelem { $$ = doc_node_add_child($1,$2); }
      | bodyelem { $$ = doc_node_make("(SECTIONBODY)",NULL,$1); }
      ;

blockB: blockA prose { $$ = doc_node_add_child($1,$2); }
      | prose { $$ = doc_node_make("(SECTIONBODY)",NULL,$1); }
      ;

bodyelem: rangetag body TAGSYM { $$ = doc_node_make_take_children($1,NULL,$2); }
        | listtag listbody TAGSYM { $$ = doc_node_make_take_children($1,NULL,$2); }
        | TABLE tablebody TAGSYM { $$ = doc_node_make_take_children("TABLE",NULL,$2); }
        | DEFINITIONLIST deflistbody TAGSYM { $$ = doc_node_make_take_children("DEFINITIONLIST",NULL,$2); }
        | blocktag wordsnl TAGSYM { $$ = doc_node_make($1,$2,NULL); }
        | CLASSTREE words eoleof { $$ = doc_node_make("CLASSTREE",$2,NULL); }
        | KEYWORD commalist eoleof { $$ = doc_node_make_take_children("KEYWORD",NULL,$2);
//            printf("keyword '%s'\n",$2->children[0]->text);
        }
        | EMPTYLINES { $$ = NULL; }
        | IMAGE words2 TAGSYM { $$ = doc_node_make("IMAGE",$2,NULL); }
        ;

prose: prose proseelem { $$ = doc_node_add_child($1, $2); }
     | proseelem { $$ = doc_node_make("PROSE",NULL,$1); }
     ;

proseelem: anyword { $$ = doc_node_make(NODE_TEXT,$1,NULL); } // one TEXT for each word
         | URL { $$ = doc_node_make("LINK",$1,NULL); }
         | inlinetag words TAGSYM { $$ = doc_node_make($1,$2,NULL); }
         | FOOTNOTE body TAGSYM { $$ = doc_node_make_take_children("FOOTNOTE",NULL,$2); }
         | NEWLINE { $$ = doc_node_create(NODE_NL); }
         ;

inlinetag: LINK { $$ = "LINK"; }
         | STRONG { $$ = "STRONG"; }
         | SOFT { $$ = "SOFT"; }
         | EMPHASIS { $$ = "EMPHASIS"; }
         | CODE { $$ = "CODE"; }
         | TELETYPE { $$ = "TELETYPE"; }
         | ANCHOR { $$ = "ANCHOR"; }
;

blocktag: CODEBLOCK { $$ = "CODEBLOCK"; }
        | TELETYPEBLOCK { $$ = "TELETYPEBLOCK"; }
;

listtag: LIST { $$ = "LIST"; }
       | TREE { $$ = "TREE"; }
       | NUMBEREDLIST { $$ = "NUMBEREDLIST"; }
;
       
rangetag: WARNING { $$ = "WARNING"; }
        | NOTE { $$ = "NOTE"; }
;

listbody: listbody HASHES body { $$ = doc_node_add_child($1, doc_node_make_take_children("ITEM",NULL,$3)); }
        | HASHES body { $$ = doc_node_make("(LISTBODY)",NULL, doc_node_make_take_children("ITEM",NULL,$2)); }
;

tablerow: HASHES tablecells { $$ = doc_node_make_take_children("TABROW",NULL,$2); }
;

tablebody: tablebody tablerow { $$ = doc_node_add_child($1,$2); }
         | tablerow { $$ = doc_node_make("(TABLEBODY)",NULL,$1); }
;

tablecells: tablecells BARS optbody { $$ = doc_node_add_child($1, doc_node_make_take_children("TABCOL",NULL,$3)); }
          | optbody { $$ = doc_node_make("(TABLECELLS)",NULL, doc_node_make_take_children("TABCOL",NULL,$1)); }
;

defterms: defterms HASHES body { $$ = doc_node_add_child($1,doc_node_make_take_children("TERM",NULL,$3)); }
        | HASHES body { $$ = doc_node_make("(TERMS)",NULL,doc_node_make_take_children("TERM",NULL,$2)); }
;

deflistrow: defterms BARS optbody
    {
        $$ = doc_node_make_take_children("DEFLISTITEM", NULL, $1);
        doc_node_add_child($$, doc_node_make_take_children("DEFINITION", NULL, $3));
    }
;

deflistbody: deflistbody deflistrow { $$ = doc_node_add_child($1,$2); }
           | deflistrow { $$ = doc_node_make("(DEFLISTBODY)",NULL,$1); }
;

anywordurl: anyword
          | URL
;

anyword: TEXT
       | COMMA
;

words: words anyword { $$ = strmerge($1,$2); }
     | anyword
;

words2: words2 anywordurl { $$ = strmerge($1,$2); }
      | anywordurl
;

eol: NEWLINE
   | EMPTYLINES
;

eoleof: eol
      | END
      ;

anywordnl: anyword
         | eol { $$ = strdup("\n"); }
;

wordsnl: wordsnl anywordnl { $$ = strmerge($1,$2); }
       | anywordnl
;

nocommawords: nocommawords TEXT { $$ = strmerge($1,$2); }
            | nocommawords URL  { $$ = strmerge($1,$2); }
            | TEXT
            | URL
;

commalist: commalist COMMA nocommawords { free($2); $2=NULL; $$ = doc_node_add_child($1,doc_node_make("STRING",$3,NULL)); }
         | nocommawords { $$ = doc_node_make("(COMMALIST)",NULL,doc_node_make("STRING",$1,NULL)); }
;

%%

DocNode * scdoc_parse_run(int mode) {
    int modes[] = {START_FULL, START_PARTIAL, START_METADATA};
    if(mode<0 || mode>=sizeof(modes)) {
        error("scdoc_parse_run(): unknown mode: %d\n",mode);
    }
    scdoc_start_token = modes[mode];
/*    scdoc_start_token = START_FULL;
    scdoc_metadata_mode = 0;
    if(mode==SCDOC_PARSE_PARTIAL) {
        scdoc_start_token = START_PARTIAL;
    } else
    if(mode==SCDOC_PARSE_METADATA) {
        scdoc_metadata_mode = 1;
    }*/
    topnode = NULL;
    method_type = "METHOD";
    if(scdocparse()!=0) {
        return NULL;
    }
    return topnode;
}

void scdocerror(const char *str)
{
    error("In %s:\n  At line %d: %s\n\n",scdoc_current_file,scdoclineno,str);

/*  FIXME: this does not work well, since the reported linenumber is often *after* the actual error line
    fseek(scdocin, 0, SEEK_SET);
    int line = 1;
    char buf[256],*txt;
    while(line!=scdoclineno && !feof(scdocin)) {
        int c = fgetc(scdocin);
        if(c=='\n') line++;
    }
    txt = fgets(buf, 256, scdocin);
    if(txt)
        fprintf(stderr,"  %s\n-------------------\n", txt);
*/
}

