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

#include "SCDoc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#    include "SC_Codecvt.hpp" // utf8_cstr_to_utf16_wstring
#endif // _WIN32

extern void error(const char *fmt, ...); // for scdoc_parse_file

DocNode * scdoc_parse_run(int partial);
void scdocrestart (FILE *input_file);
int scdoclex_destroy(void);

const char * scdoc_current_file = NULL;

const char * NODE_TEXT = "TEXT";
const char * NODE_NL = "NL";

static int doc_node_dump_level_done[32] = {0,};

// merge a+b and free b
char *strmerge(char *a, char *b) {
    if(a==NULL) return b;
    if(b==NULL) return a;
    char *s = (char *)realloc(a,strlen(a)+strlen(b)+1);
    strcat(s,b);
    free(b);
    return s;
}

static char *striptrailingws(char *s) {
    char *s2 = strchr(s,0);
    // don't use isspace -- triggers assert with VS
    while(--s2 > s && (*s2 == ' ' || *s2 == '\t' || *s2 == '\n' || *s2 == '\v' || *s2 == '\f' || *s2 == '\r')) {
        *s2 = 0;
    }
    return s;
}

DocNode * doc_node_create(const char *id) {
    DocNode *n = (DocNode *)malloc(sizeof(DocNode));
    n->id = id;
    n->text = NULL;
    n->n_childs = 0;
    n->children = NULL;
    return n;
}

// takes ownership of child
DocNode * doc_node_add_child(DocNode *n, DocNode *child) {
    if(child) {
        n->children = (DocNode **)realloc(n->children, (n->n_childs+1) * sizeof(DocNode*));
        n->children[n->n_childs] = child;
        n->n_childs++;
    }
    return n;
}

// moves the childs from src doc_node to n
void doc_node_move_children(DocNode *n, DocNode *src) {
    if(src) {
        free(n->children);
        n->children = src->children;
        n->n_childs = src->n_childs;
//        src->children = NULL;
//        src->n_childs = 0;
        free(src->text);
        free(src);
    }
}

DocNode * doc_node_make(const char *id, char *text, DocNode *child) {
    DocNode *n = doc_node_create(id);
    n->text = text;
    doc_node_add_child(n, child);
    return n;
}

DocNode * doc_node_make_take_children(const char *id, char *text, DocNode *src) {
    DocNode *n = doc_node_make(id, text, NULL);
    doc_node_move_children(n, src);
    return n;
}

void doc_node_free_tree(DocNode *n) {
    int i;
    if(!n) return;
    free(n->text);
    for(i=0;i<n->n_childs;i++) {
        doc_node_free_tree(n->children[i]);
    }
    free(n->children);
    free(n);
}

void doc_node_fixup_tree(DocNode *n) {
    int i;
    if(n->id != NODE_TEXT && n->text) {
        n->text = striptrailingws(n->text);
    }
    if(n->n_childs) {
        DocNode *last = n->children[n->n_childs-1];
        if(last->id==NODE_NL) {
            free(last); // NL has no text or children
            n->n_childs--;
        }
        last = NULL;
        for(i = 0; i < n->n_childs; i++) {
            DocNode *child = n->children[i];
            if((child->id==NODE_TEXT || child->id==NODE_NL) && last && last->id==NODE_TEXT) {
                if(child->id==NODE_NL) {
                    last->text = (char*)realloc(last->text,strlen(last->text)+2);
                    strcat(last->text," ");
                } else {
                    last->text = strmerge(last->text,child->text);
                }
                free(child); // we took childs text and it has no children
                n->children[i] = NULL;
            } else {
                doc_node_fixup_tree(child);
                last = child;
            }
        }
        int j = 0;
        for(i = 0; i < n->n_childs; i++) {
            if(n->children[i]) {
                n->children[j++] = n->children[i];
            }
        }
        n->n_childs = j;
    }
}

static void doc_node_dump_recursive(DocNode *n, int level, int last) {
    int i;
    for(i=0;i<level;i++) {
        if(doc_node_dump_level_done[i])
            printf("    ");
        else
            printf("|   ");
    }
    if(last) {
        printf("`-- ");
        doc_node_dump_level_done[level] = 1;
    } else {
        printf("|-- ");
    }
    printf("%s",n->id);
    if(n->text) printf(" \"%s\"",n->text);
    printf("\n");
    for(i = 0; i < n->n_childs; i++) {
        doc_node_dump_recursive(n->children[i], level+1, i==n->n_childs-1);
    }
    doc_node_dump_level_done[level] = 0;
}

void doc_node_dump(DocNode *n) {
    doc_node_dump_recursive(n, 0, 1);
}

DocNode * scdoc_parse_file(const std::string& fn, int mode) {
    FILE *fp;
    DocNode *n;

    // incoming fn is from QString.toStdString(), so it's UTF-8.
#ifdef _WIN32
    const std::wstring fn_w = SC_Codecvt::utf8_cstr_to_utf16_wstring(fn.c_str());
    fp = _wfopen(fn_w.c_str(), L"r");
#else
    fp = fopen(fn.c_str(), "r");
#endif // _WIN32

    if (!fp) {
        error("scdoc_parse_file: could not open '%s'\n", fn.c_str());
        return nullptr;
    }
    scdoc_current_file = fn.c_str();
    scdocrestart(fp);
    n = scdoc_parse_run(mode);
    if(n) {
        doc_node_fixup_tree(n);
    }
    fclose(fp);
    scdoclex_destroy();
    scdoc_current_file = NULL;
    return n;
}

