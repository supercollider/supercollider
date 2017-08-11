#ifndef SCDOC_H
#define SCDOC_H

#include <stdint.h>
#include <string.h>
#include <string>

#define SCDOC_PARSE_FULL 0
#define SCDOC_PARSE_PARTIAL 1
#define SCDOC_PARSE_METADATA 2

extern const char * NODE_TEXT;
extern const char * NODE_NL;

typedef struct DocNode {
    const char *id;
    char *text;
    int n_childs;
    struct DocNode **children;
} DocNode;

char *strmerge(char *a, char *b);

DocNode * doc_node_make_take_children(const char *id, char *text, DocNode *src);
DocNode * doc_node_make(const char *id, char *text, DocNode *child);
DocNode * doc_node_add_child(DocNode *n, DocNode *child);
DocNode * doc_node_create(const char *id);
void doc_node_free_tree(DocNode *n);

/**
 * \brief Parses a .schelp file and generates a help file parse tree
 *
 * \arg fn A UTF-8 string, the filename to be parsed
 * \arg mode The mode to parse, should be one of SCDOC_PARSE_FULL, PARTIAL, or METADATA
 * \returns A parse tree pointer, or nullptr if the file count not be opened or parsed.
 */
DocNode * scdoc_parse_file(const std::string& fn, int mode);

void doc_node_dump(DocNode *n);

extern const char * scdoc_current_file;

#endif
