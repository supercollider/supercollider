#include "PyrSymbol.h"
#include "PyrObject.h"
#include <cstring>
#include "PyrSymbolTable.h"

PyrSymbol* stringToSymbol(PyrString* str) {
    static constexpr auto SmallStringBufferSize = 128;
    char small_string_buf[SmallStringBufferSize];

    const auto size = str->size;
    const auto uses_heap_string = size >= SmallStringBufferSize;

    char* const string_copy = size >= SmallStringBufferSize ? (char*)malloc(size + 1) : small_string_buf;
    std::memcpy(string_copy, str->s, size);
    string_copy[size] = 0; // symbols must be null terminated.

    const auto sym = getsym(string_copy);

    if (uses_heap_string)
        delete string_copy;

    return sym;
}