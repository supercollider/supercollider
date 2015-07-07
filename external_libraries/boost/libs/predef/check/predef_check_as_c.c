/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BOOST_PREDEF_INTERNAL_GENERATE_TESTS

typedef struct predef_info
{
    unsigned tag;
    const char * name;
    const char * description;
    unsigned value;
} predef_info;

predef_info first_predef_info = { 0x43210DEF , "-" , "-" , 0xFFFFFFFF };

#define BOOST_PREDEF_DECLARE_TEST(x,s) \
    predef_info x##_predef_info = { 0x67890DEF , #x , s , x };
#include <boost/predef.h>

predef_info last_predef_info = { 0xFFFFFFFF , "-" , "-" , 0x43210DEF };

int predef_info_compare(const void * a, const void * b)
{
    const predef_info ** i = (const predef_info **)a;
    const predef_info ** j = (const predef_info **)b;
    return strcmp((*i)->name,(*j)->name);
}

const char * str_token(const char ** str, const char * space)
{
    unsigned span;
    char * token;
    for (; **str != 0; *str += 1)
    {
        if (0 == strchr(space, **str))
        {
            break;
        }
    }
    span = strcspn(*str, space);
    token = (char *)malloc(span+1);
    strncpy(token, *str, span);
    token[span] = 0;
    for (*str += span; **str != 0; *str += 1)
    {
        if (0 == strchr(space, **str))
        {
            break;
        }
    }
    return token;
}

const char * whitespace = " ";
const char * dot = ".";

int main(int argc, const char ** argv)
{
    unsigned x = 0;
    int argi = 1;
    predef_info ** predefs = 0;
    unsigned predef_count = 0;
    unsigned * i = &first_predef_info.tag;
    unsigned * e = &last_predef_info.tag;
    while (i < e)
    {
        i += 1;
        if (*i == 0x67890DEF)
        {
            predef_count += 1;
            predefs = (predef_info**)realloc(predefs,predef_count*sizeof(predef_info*));
            predefs[predef_count-1] = (predef_info*)i;
        }
    }
    qsort(predefs,predef_count,sizeof(predef_info*),predef_info_compare);
    for (argi = 1; argi < argc; ++argi)
    {
        const char * exp = argv[argi];
        const char * exp_name = str_token(&exp, whitespace);
        const char * exp_op = str_token(&exp, whitespace);
        const char * exp_val = str_token(&exp, whitespace);
        unsigned exp_version = 0;
        if (*exp_val != 0)
        {
            exp = exp_val;
            const char * exp_val_a = str_token(&exp, dot);
            const char * exp_val_b = str_token(&exp, dot);
            const char * exp_val_c = str_token(&exp, dot);
            exp_version = BOOST_VERSION_NUMBER(atoi(exp_val_a), atoi(exp_val_b),atoi(exp_val_c));
        }
        for (x = 0; x < predef_count; ++x)
        {
            if (*exp_op == 0 &&
                predefs[x]->value == 0 &&
                strcmp(exp_name, predefs[x]->name) == 0)
            {
                return argi;
            }
            else if (*exp_op != 0 && *exp_val != 0 &&
                     strcmp(exp_name, predefs[x]->name) == 0)
            {
                if (0 == strcmp(">",exp_op) && !(predefs[x]->value > exp_version)) return argi;
                if (0 == strcmp("<",exp_op) && !(predefs[x]->value < exp_version)) return argi;
                if (0 == strcmp(">=",exp_op) && !(predefs[x]->value >= exp_version)) return argi;
                if (0 == strcmp("<=",exp_op) && !(predefs[x]->value <= exp_version)) return argi;
                if (0 == strcmp("==",exp_op) && !(predefs[x]->value == exp_version)) return argi;
                if (0 == strcmp("!=",exp_op) && !(predefs[x]->value != exp_version)) return argi;
            }
        }
    }
    return 0;
}
