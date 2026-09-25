#include "PyrSlot.h"
#include "PyrKernel.h"
#include "PyrObject.h"
#include <cctype>
#include <cstdint>
#include <string>

void PyrSlot::appendToStringForDebug(std::string& stream, size_t sizeLimit) const {
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
        stream += "Integer(" + std::to_string(getInt()) + ")";
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
                array.slots[i].appendToStringForDebug(stream, sizeLimit);
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
        }

    } else {
        assert(false); // shouldn't happen
        stream += "unknown";
    }
}
