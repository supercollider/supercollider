#include "PyrSlot.h"
#include "PyrKernel.h"
#include "PyrObject.h"
#include "PyrObjectHdr.h"
#include "SCBase.h"
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

void PyrSlot::appendToStringForDebug(std::string& stream, size_t sizeLimit,
                                     std::unordered_set<PyrObjectHdr*> alreadyVisited) const {
    if (isDouble()) {
        stream += std::to_string(getDouble());
    } else if (isChar()) {
        switch (getChar()) {
        case '\n':
            stream += "$\\n";
            break;
        case '\f':
            stream += "$\\f";
            break;
        case '\v':
            stream += "$\\v";
            break;
        case '\r':
            stream += "$\\r";
            break;
        default:
            stream += "$" + std::to_string(getChar());
            break;
        }
    } else if (isInt()) {
        stream += std::to_string(getInt());
    } else if (isPtr()) {
        stream += "RawPointer(" + std::to_string(reinterpret_cast<intptr_t>(getPtr())) + ")";
    } else if (isSymbol()) {
        const auto& s = *getSymbol();
        stream += "'";
        const auto sz = std::min<size_t>(s.length, 20);
        size_t written { 0 };
        for (; written < sz; ++written) {
            const auto c = s.name[written];
            if (c == '\n' || !std::isprint(c)) {
                stream += "...";
                break;
            } else {
                stream += c;
            }
        }
        if (written != s.length && written == sz)
            stream += "...";
        stream += "'";
    } else if (isNil()) {
        stream += "nil";
    } else if (isTrue()) {
        stream += "true";
    } else if (isFalse()) {
        stream += "false";
    } else if (isObjectHdr()) {
        if (alreadyVisited.count(getObjectHdr())) {
            stream += "#recursion#";
            return;
        }
        alreadyVisited.insert(getObjectHdr());
        const auto& obj = getObjectHdr();
        const auto& c = obj->classptr;
        if (c == class_string) {
            const auto& str = *getPyrObjType<PyrString>();
            stream += '"';
            const auto sz = std::min<size_t>(str.size, 20);
            size_t written { 0 };
            for (; written < sz; ++written) {
                const auto c = str.s[written];
                if (c == '\n' || !std::isprint(c)) {
                    stream += "...";
                    break;
                } else {
                    stream += c;
                }
            }
            if (written != str.size && written == sz)
                stream += "...";
            stream += '"';
        } else if (c == class_array) {
            const auto& array = *getPyrObjType<PyrObject>();
            stream += '[';
            for (size_t i { 0 }; i < array.size; ++i) {
                if (stream.size() >= sizeLimit) {
                    stream += "...";
                    break;
                }
                array.slots[i].appendToStringForDebug(stream, sizeLimit, alreadyVisited);
                if (i + 1 != array.size) {
                    stream += ", ";
                }
            }
            stream += ']';
        } else if (c == class_signal || c == class_floatarray) {
            const auto& sig = *getPyrObjType<PyrFloatArray>();
            if (c == class_signal) {
                stream += "Signal[";
            } else {
                stream += "FloatArray[";
            }
            for (size_t i { 0 }; i < sig.size; ++i) {
                if (stream.size() >= sizeLimit) {
                    stream += "...";
                    break;
                }
                stream += std::to_string(sig.f[i]);
                if (i + 1 != sig.size) {
                    stream += ", ";
                }
            }
            stream += ']';
        } else if (c == class_class) {
            const auto& c = *getPyrObjType<PyrClass>();
            stream += "Class(";
            const auto& sym = *c.name.getSymbol();
            stream.append(sym.name, sym.length);
            stream += ")";
        } else if (c == class_fundef) {
            const auto& block = *getPyrObjType<PyrBlock>();
            if (!block.name.isNil()) {
                const auto& name = *block.name.getSymbol();
                stream.append(name.name, name.length);
                stream += ": ";
            } else {
                stream += "Anonymous Function: ";
            }
            const auto start = block.sourceCodeStartIndex.getInt();
            const auto end = block.sourceCodeEndIndex.getInt();
            const auto& str = *block.sourceCodeFileOrSnippet.getPyrObjType<PyrString>();
            for (size_t i { 0 }; i < 20 && i + start <= end; ++i) {
                const auto c = str.s[start + i];
                if (c == '\n')
                    stream += "\\n";
                else
                    stream += c;
            }
            if (str.size > 20) {
                stream += "...}";
            }
        } else {
            const auto& objhdr = *getObjectHdr();
            stream += objhdr.classptr->name.getSymbol()->name;
            stream += "(";
            if (objhdr.obj_format == obj_slot || objhdr.obj_format == obj_notindexed) {
                const PyrObject& obj = *reinterpret_cast<const PyrObject*>(&objhdr);

                const PyrClass& objClass = *obj.classptr;
                PyrSymbol** instanceNames = objClass.instVarNames.getPyrObjType<PyrSymbolArray>()->symbols;
                for (size_t i { 0 }; i < obj.size; ++i) {
                    if (stream.size() >= sizeLimit) {
                        stream += "...";
                        break;
                    }

                    stream += instanceNames[i]->name;
                    stream += "=";

                    obj.slots[i].appendToStringForDebug(stream, sizeLimit, alreadyVisited);

                    if (i + 1 != obj.size) {
                        stream += ", ";
                    }
                }
            } else {
                stream += "...";
            }
            stream += ")";
        }

    } else {
        assert(false); // shouldn't happen
        stream += "unknown";
    }
}

void dumpPyrSlot(PyrSlot* slot) {
    char str[1024];
    slotString(slot, str);
    post("   %s\n", str);
}

void slotString(PyrSlot* slot, char* str) {
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "Integer %d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "Character %d '%c'", static_cast<int>(slotRawChar(slot)), static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        if (strlen(slotRawSymbol(slot)->name) > 240) {
            char str2[256];
            memcpy(str2, slotRawSymbol(slot)->name, 240);
            str2[240] = 0;
            snprintf(str, 256, "Symbol '%s...'", str2);
        } else {
            snprintf(str, 256, "Symbol '%s'", slotRawSymbol(slot)->name);
        }
        break;
    case tagObj:
        if (slotRawObject(slot)) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class) {
                sprintf(str, "class %s (%p)", slotRawSymbol(&((PyrClass*)slotRawObject(slot))->name)->name,
                        slotRawObject(slot));
            } else if (classptr == class_string) {
                char str2[48];
                int len;
                if (slotRawObject(slot)->size > 47) {
                    memcpy(str2, (char*)slotRawObject(slot)->slots, 44);
                    str2[44] = '.';
                    str2[45] = '.';
                    str2[46] = '.';
                    str2[47] = 0;
                } else {
                    len = sc_min(47, slotRawObject(slot)->size);
                    memcpy(str2, (char*)slotRawObject(slot)->slots, len);
                    str2[len] = 0;
                }
                sprintf(str, "\"%s\"", str2);
            } else if (classptr == class_method) {
                sprintf(str, "instance of Method %s:%s (%p)",
                        slotRawSymbol(&slotRawClass(&slotRawMethod(slot)->ownerclass)->name)->name,
                        slotRawSymbol(&slotRawMethod(slot)->name)->name, slotRawMethod(slot));
            } else if (classptr == class_fundef) {
                PyrSlot *context, *nextcontext;
                // find function's method
                nextcontext = &slotRawBlock(slot)->contextDef;
                if (NotNil(nextcontext)) {
                    do {
                        context = nextcontext;
                        nextcontext = &slotRawBlock(context)->contextDef;
                    } while (NotNil(nextcontext));
                    if (isKindOf(slotRawObject(context), class_method)) {
                        sprintf(str, "instance of FunctionDef in Method %s:%s",
                                slotRawSymbol(&slotRawClass(&slotRawMethod(context)->ownerclass)->name)->name,
                                slotRawSymbol(&slotRawMethod(context)->name)->name);
                    } else {
                        sprintf(str, "instance of FunctionDef in closed FunctionDef");
                    }
                } else {
                    sprintf(str, "instance of FunctionDef - closed");
                }
            } else if (classptr == class_frame) {
                if (!slotRawFrame(slot)) {
                    sprintf(str, "Frame (%0X)", slotRawInt(slot));
                } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
                    sprintf(str, "Frame (%p) of %s:%s", slotRawObject(slot),
                            slotRawSymbol(&slotRawClass(&slotRawMethod(&slotRawFrame(slot)->method)->ownerclass)->name)
                                ->name,
                            slotRawSymbol(&slotRawMethod(&slotRawFrame(slot)->method)->name)->name);
                } else {
                    sprintf(str, "Frame (%p) of Function", slotRawFrame(slot));
                }
            } else {
                sprintf(str, "instance of %s (%p, size=%d, set=%d)", slotRawSymbol(&classptr->name)->name,
                        slotRawObject(slot), slotRawObject(slot)->size, slotRawObject(slot)->obj_sizeclass);
            }
        } else {
            sprintf(str, "NULL Object Pointer");
        }
        break;
    case tagNil:
        sprintf(str, "nil");
        break;
    case tagFalse:
        sprintf(str, "false");
        break;
    case tagTrue:
        sprintf(str, "true");
        break;
    case tagPtr:
        sprintf(str, "RawPointer %p", slotRawPtr(slot));
        break;
    default: {
        union {
            int32 i[2];
            double f;
        } u;
        u.f = slotRawFloat(slot);
        sprintf(str, "Float %f   %08X %08X", u.f, u.i[0], u.i[1]);
        break;
    }
    }
}

static void printObject(PyrSlot* slot, PyrObject* obj, char* str) {
    assert(obj);
    PyrClass* classptr = obj->classptr;
    if (classptr == class_class) {
        sprintf(str, "class %s", slotRawSymbol(&((PyrClass*)obj)->name)->name);
    } else if (classptr == class_string) {
        char str2[32];
        int len;
        if (obj->size > 31) {
            memcpy(str2, (char*)obj->slots, 28);
            str2[28] = '.';
            str2[29] = '.';
            str2[30] = '.';
            str2[31] = 0;
        } else {
            len = sc_min(31, obj->size);
            memcpy(str2, (char*)obj->slots, len);
            str2[len] = 0;
        }
        sprintf(str, "\"%s\"", str2);
    } else if (classptr == class_method) {
        sprintf(str, "%s:%s", slotRawSymbol(&slotRawClass(&slotRawMethod(slot)->ownerclass)->name)->name,
                slotRawSymbol(&slotRawMethod(slot)->name)->name);
    } else if (classptr == class_fundef) {
        PyrSlot *context, *nextcontext;
        // find function's method
        nextcontext = &slotRawBlock(slot)->contextDef;
        if (NotNil(nextcontext)) {
            do {
                context = nextcontext;
                nextcontext = &slotRawBlock(context)->contextDef;
            } while (NotNil(nextcontext));
            if (isKindOf(slotRawObject(context), class_method)) {
                sprintf(str, "< FunctionDef in Method %s:%s >",
                        slotRawSymbol(&slotRawClass(&slotRawMethod(context)->ownerclass)->name)->name,
                        slotRawSymbol(&slotRawMethod(context)->name)->name);
            } else {
                sprintf(str, "< FunctionDef in closed FunctionDef >");
            }
        } else {
            sprintf(str, "< closed FunctionDef >");
        }
    } else if (classptr == class_frame) {
        if (!slotRawFrame(slot)) {
            sprintf(str, "Frame (null)");
        } else if (!slotRawBlock(&slotRawFrame(slot)->method)) {
            sprintf(str, "Frame (null method)");
        } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
            sprintf(str, "Frame (%p) of %s:%s", obj,
                    slotRawSymbol(&slotRawClass(&slotRawMethod(&slotRawFrame(slot)->method)->ownerclass)->name)->name,
                    slotRawSymbol(&slotRawMethod(&slotRawFrame(slot)->method)->name)->name);
        } else {
            sprintf(str, "Frame (%p) of Function", obj);
        }
    } else if (classptr == class_array) {
        sprintf(str, "[*%d]", obj->size);
    } else {
        sprintf(str, "<instance of %s>", slotRawSymbol(&classptr->name)->name);
    }
}

// Assumed: str has enough space to hold the representation of d
static void prettyFormatFloat(char* str, double d) {
    if (std::isnan(d)) {
        // Do this manually as different implementations disagree on how to format nan.
        str[0] = 'n';
        str[1] = 'a';
        str[2] = 'n';
        str[3] = '\0';
        return;
    } else if (std::isinf(d)) {
        if (d > 0.0) {
            str[0] = 'i';
            str[1] = 'n';
            str[2] = 'f';
            str[3] = '\0';
        } else {
            str[0] = '-';
            str[1] = 'i';
            str[2] = 'n';
            str[3] = 'f';
            str[4] = '\0';
        }
        return;
    }
    sprintf(str, "%.14g", d);

    // append a trailing '.0' if the number would look like an integer.
    for (; *str; ++str) {
        if ((*str < '0' || *str > '9') && *str != '-')
            return;
    }

    str[0] = '.';
    str[1] = '0';
    str[2] = '\0';
}

void slotOneWord(PyrSlot* slot, char* str) {
    str[0] = 0;
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "$%c", static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        if (strlen(slotRawSymbol(slot)->name) > 240) {
            char str2[256];
            memcpy(str2, slotRawSymbol(slot)->name, 240);
            str2[240] = 0;
            snprintf(str, 256, "'%s...'", str2);
        } else {
            snprintf(str, 256, "'%s'", slotRawSymbol(slot)->name);
        }
        break;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj)
            printObject(slot, slotObj, str);
        else
            sprintf(str, "NULL Object Pointer");
        break;
    }
    case tagNil:
        sprintf(str, "nil");
        break;
    case tagFalse:
        sprintf(str, "false");
        break;
    case tagTrue:
        sprintf(str, "true");
        break;
    case tagPtr:
        sprintf(str, "ptr%p", slotRawPtr(slot));
        break;
    default:
        prettyFormatFloat(str, slotRawFloat(slot));
        break;
    }
}

bool postString(PyrSlot* slot, char* str) {
    bool res = true;
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "%c", slotRawChar(slot));
        break;
    case tagSym:
        str[0] = 0;
        res = false;
        break;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class || classptr == class_method || classptr == class_fundef
                || classptr == class_frame)
                printObject(slot, slotObj, str);
            else {
                str[0] = 0;
                res = false;
            }
        } else
            sprintf(str, "NULL Object Pointer");
        break;
    }
    case tagNil:
        sprintf(str, "nil");
        break;
    case tagFalse:
        sprintf(str, "false");
        break;
    case tagTrue:
        sprintf(str, "true");
        break;
    case tagPtr:
        sprintf(str, "%p", slotRawPtr(slot));
        break;
    default:
        prettyFormatFloat(str, slotRawFloat(slot));
        break;
    }
    return res;
}


int asCompileString(PyrSlot* slot, char* str) {
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar: {
        int c = slotRawChar(slot);
        if (c == '\\') {
            strcpy(str, "$\\\\");
        } else if (isprint(c)) {
            sprintf(str, "$%c", c);
        } else {
            switch (c) {
            case '\n':
                strcpy(str, "$\\n");
                break;
            case '\r':
                strcpy(str, "$\\r");
                break;
            case '\t':
                strcpy(str, "$\\t");
                break;
            case '\f':
                strcpy(str, "$\\f");
                break;
            case '\v':
                strcpy(str, "$\\v");
                break;
            default:
                sprintf(str, "%d.asAscii", c);
            }
        }
        break;
    }
    case tagSym:
        return errFailed;
    case tagObj:
        return errFailed;
    case tagNil:
        sprintf(str, "nil");
        break;
    case tagFalse:
        sprintf(str, "false");
        break;
    case tagTrue:
        sprintf(str, "true");
        break;
    case tagPtr:
        strcpy(str, "/*Ptr*/ nil");
        break;
    default:
        sprintf(str, "%f", slotRawFloat(slot));
        break;
    }
    return errNone;
}


void stringFromPyrString(PyrString* obj, char* str, int maxlength);
void stringFromPyrString(PyrString* obj, char* str, int maxlength) {
    if (obj->classptr == class_string) {
        int len;
        if (obj->size > maxlength - 4) {
            memcpy(str, obj->s, maxlength - 4);
            str[maxlength - 4] = '.';
            str[maxlength - 3] = '.';
            str[maxlength - 2] = '.';
            str[maxlength - 1] = 0;
        } else {
            len = sc_min(maxlength - 1, obj->size);
            memcpy(str, obj->s, len);
            str[len] = 0;
        }
    } else {
        sprintf(str, "not a string");
    }
}

void pstrncpy(unsigned char* s1, unsigned char* s2, int n) {
    int i, m;
    m = *s2++;
    n = (n < m) ? n : m;
    *s1 = n;
    s1++;
    for (i = 0; i < n; ++i) {
        *s1 = *s2;
        s1++;
        s2++;
    }
}
void pstringFromPyrString(PyrString* obj, unsigned char* str, int maxlength) {
    static const char not_a_string[] = "not a string";
    const char* src;
    int len;
    if (obj && obj->classptr == class_string) {
        len = sc_min(maxlength - 1, obj->size);
        src = obj->s;
    } else {
        len = sizeof(not_a_string);
        src = not_a_string;
    }
    memcpy(str + 1, src, len);
    str[0] = len;
}
