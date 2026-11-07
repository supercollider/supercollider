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

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "SCBase.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cerrno>
#include <fstream>

// extern "C" {
#include "SCDoc.h"
//}
#include "SCDocSearchIndex.h"
#include "SC_Codecvt.hpp"

PyrSymbol* s_scdoc_node;

static void _doc_traverse(struct VMGlobals* g, DocNode* n, PyrObject* parent, PyrSlot* slot) {
    PyrObject* result = instantiateObject(g->gc, s_scdoc_node->u.classobj, 0, false, true);
    SetObject(slot, result);
    if (parent) {
        assert(isKindOf(parent, class_array));
        g->gc->GCWriteNew(parent, result); // we know result is white so we can use GCWriteNew
        parent->size++;
    }

    // initialise the instance vars
    PyrSymbol* id = getsym(n->id);
    SetSymbol(result->slots, id); // id

    // text
    if (n->text) {
        PyrObject* str = (PyrObject*)newPyrString(g->gc, n->text, 0, true);
        SetObject(result->slots + 1, str);
        g->gc->GCWriteNew(result, str); // we know str is white so we can use GCWriteNew
    }

    // children
    if (n->n_childs) {
        PyrObject* array = newPyrArray(g->gc, n->n_childs, 0, true);
        SetObject(result->slots + 2, array);
        g->gc->GCWriteNew(result, array); // we know array is white so we can use GCWriteNew
        for (int i = 0; i < n->n_childs; i++) {
            _doc_traverse(g, n->children[i], array, array->slots + i);
        }
    }
    // makeDiv, notPrivOnly, sort remain nil for now
}


int prSCDoc_ParseFile(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    char filename[PATH_MAX];
    int mode, err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    err = slotStrVal(b, filename, PATH_MAX);
    if (err)
        return err;

    err = slotIntVal(c, &mode);
    if (err)
        return err;

    DocNode* n = scdoc_parse_file(filename, mode);
    if (n) {
        //        doc_node_dump(n);
        _doc_traverse(g, n, NULL, a);
        doc_node_free_tree(n);
    } else {
        SetNil(a);
    }
    return errNone;
}

/**
 * Extracts the content of the SCDocs such that it is accessible by the JS search index.
 *
 * @param slot an Array of [path, fullPath, title, summary, isExtension, additions]
 * @param docs generated content for the file
 * @return error code
 */
static int readDocTextSources(const PyrSlot* slot, std::vector<SCDocTextSource>& docs) {
    if (!isKindOfSlot(slot, class_array))
        return errWrongType;
    PyrObject* array = slotRawObject(slot);
    docs.resize(array->size);
    for (int i = 0; i < array->size; i++) {
        PyrSlot* item = array->slots + i;
        if (!isKindOfSlot(item, class_array) || slotRawObject(item)->size != 6)
            return errWrongType;
        PyrSlot* fields = slotRawObject(item)->slots;
        SCDocTextSource& doc = docs[i];
        int err;
        std::tie(err, doc.path) = slotStrStdStrVal(fields + 0);
        if (err)
            return err;
        std::tie(err, doc.fullPath) = slotStrStdStrVal(fields + 1);
        if (err)
            return err;
        // title and summary may be nil
        if (NotNil(fields + 2))
            std::tie(err, doc.title) = slotStrStdStrVal(fields + 2);
        if (err)
            return err;
        if (NotNil(fields + 3))
            std::tie(err, doc.summary) = slotStrStdStrVal(fields + 3);
        if (err)
            return err;
        doc.isExtension = IsTrue(fields + 4);
        PyrSlot* additions = fields + 5;
        if (NotNil(additions)) {
            if (!isKindOfSlot(additions, class_array))
                return errWrongType;
            PyrObject* additionsArray = slotRawObject(additions);
            for (int j = 0; j < additionsArray->size; j++) {
                std::string addition;
                std::tie(err, addition) = slotStrStdStrVal(additionsArray->slots + j);
                if (err)
                    return err;
                doc.additions.push_back(std::move(addition));
            }
        }
    }
    return errNone;
}

//
// writes doctext.js, returns the number of entries, or nil if the file could not be written
/**
 * Primitive called by \c SCDoc.prBuildSearchIndex .
 * Builds \c doctext.js which is used by the JS full page search index.
 * The sclang function returns the number of parsed documents and nil if there was an error beyond sclang error codes.
 */
int prSCDoc_BuildSearchIndex(VMGlobals* g, int numArgsPushed) {
    auto* returnCountSlot = g->sp - 2;
    auto* jsonPathSlot = g->sp - 1;
    auto* docArraySlot = g->sp;

    auto [err, outPath] = slotStrStdStrVal(jsonPathSlot);
    if (err)
        return err;

    std::vector<SCDocTextSource> docs;
    err = readDocTextSources(docArraySlot, docs);
    if (err)
        return err;

    std::ofstream out(SC_Codecvt::utf8_str_to_path(outPath), std::ios::binary);
    if (!out) {
        SetNil(returnCountSlot);
        return errNone;
    }
    int count = scdocExportSearchIndex(docs, out);
    out.close();
    if (out.fail())
        SetNil(returnCountSlot);
    else
        SetInt(returnCountSlot, count);
    return errNone;
}

void initSCDocPrimitives() {
    int base, index = 0;

    s_scdoc_node = getsym("SCDocNode");

    base = nextPrimitiveIndex();

    definePrimitive(base, index++, "_SCDoc_ParseFile", prSCDoc_ParseFile, 3, 0);
    definePrimitive(base, index++, "_SCDoc_BuildSearchIndex", prSCDoc_BuildSearchIndex, 3, 0);
}
