#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "SCDoc.h"

void error(const char* fmt, ...) {
    fprintf(stderr, "ERROR: ");
    va_list vargs;
    va_start(vargs, fmt);
    vfprintf(stderr, fmt, vargs);
    fflush(stderr);
    va_end(vargs);
}

void post(const char* fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);
    vfprintf(stderr, fmt, vargs);
    fflush(stderr);
    va_end(vargs);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        DocNode* n;
        if (argc > 2 && strcmp(argv[1], "--partial") == 0)
            n = scdoc_parse_file(argv[2], SCDOC_PARSE_PARTIAL);
        else if (argc > 2 && strcmp(argv[1], "--metadata") == 0)
            n = scdoc_parse_file(argv[2], SCDOC_PARSE_METADATA);
        else
            n = scdoc_parse_file(argv[1], SCDOC_PARSE_FULL);
        if (n) {
            doc_node_dump(n);
            doc_node_free_tree(n);
        } else
            return 1;
    } else {
        fprintf(stderr, "Usage: %s inputfile.schelp\n", argv[0]);
    }
    return 0;
}
