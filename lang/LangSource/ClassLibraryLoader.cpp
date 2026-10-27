#include "ClassLibraryLoader.hpp"

#include "GC.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"
#include "PyrKernelProto.h"
#include "PyrListPrim.h"
#include "PyrObject.h"
#include "PyrPrimitive.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
#include "PyrSymbolTable.h"
#include "SCBase.h"
#include "SC_Codecvt.hpp"
#include "SC_Filesystem.hpp"
#include "SC_LanguageConfig.hpp"
#include "VMGlobals.h"
#include "sc_compiler/sc_compiler.hpp"
#include "sc_util/narrow.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#ifndef NDEBUG
#    include "PyrMessage.h"
#    include <unordered_set>
#endif

namespace {

namespace fs = std::filesystem;
namespace backend = sc::backend;
namespace compiler = sc::compiler;
namespace ir = sc::ir;
using sc::util::narrow;

// As the old compiler allocated objects: a class file's are permanent, interpreted code's are the GC's, made without
// collecting.
struct Allocation {
    PyrGC* gc;
    int flags;
};
constexpr Allocation classLibraryAllocation { nullptr, obj_permanent | obj_immutable };

////////////////////////////////////////////////////////////////////////////////
// Class files
////////////////////////////////////////////////////////////////////////////////

// As the old compiler's pass one walked the directories, each included directory is a module.
struct ClassFileFinder {
    compiler::ClassFiles files {};
    std::set<fs::path> compiledDirectories {};

    bool shouldSkip(const fs::path& dir) const {
        return compiledDirectories.count(dir) != 0 || gLanguageConfig->pathIsExcluded(dir)
            || SC_Filesystem::instance().shouldNotCompileDirectory(dir);
    }

    void addFile(const fs::path& path, ir::ModuleIndex module) {
        if (gLanguageConfig->pathIsExcluded(path)) {
            post("\texcluding file: '%s'\n", SC_Codecvt::path_to_utf8_str(path).c_str());
            return;
        }
        if (path.filename().string().front() != '.' && path.extension() == ".sc")
            (void)files.push_back({ path, module });
    }

    bool addDirectory(const fs::path& dir, ir::ModuleIndex module) {
        std::error_code ec;
        const fs::path expanded = SC_Filesystem::instance().expandTilde(dir);
        fs::recursive_directory_iterator it(expanded, fs::directory_options::follow_directory_symlink, ec);
        if (ec) {
            if (ec.default_error_condition() != std::errc::no_such_file_or_directory) {
                error("Could not open directory '%s': (%d) %s\n", SC_Codecvt::path_to_utf8_str(expanded).c_str(),
                      ec.value(), ec.message().c_str());
                return false;
            }
            const auto& defaults = gLanguageConfig->defaultClassLibraryDirectories();
            if (std::find(defaults.begin(), defaults.end(), expanded) != defaults.end())
                fs::create_directories(expanded, ec);
            else
                post("WARNING: Could not open directory: '%s'\n"
                     "\tTo resolve this, either create the directory or remove it from your compilation paths.\n\n",
                     SC_Codecvt::path_to_utf8_str(expanded).c_str());
            return true;
        }
        if (shouldSkip(expanded))
            return true;
        post("\tCompiling directory '%s'\n", SC_Codecvt::path_to_utf8_str(expanded).c_str());
        compiledDirectories.insert(expanded);

        for (; it != fs::end(it); it.increment(ec)) {
            if (ec) {
                error("Could not iterate on '%s': %s\n", SC_Codecvt::path_to_utf8_str(it->path()).c_str(),
                      ec.message().c_str());
                return true;
            }
            const fs::path path = *it;
            if (fs::is_directory(path)) {
                if (shouldSkip(path))
                    it.disable_recursion_pending();
                else
                    compiledDirectories.insert(path);
                continue;
            }
            bool isAlias = false;
            const fs::path resolved = SC_Filesystem::resolveIfAlias(path, isAlias);
            if (isAlias && fs::is_directory(resolved)) {
                if (!addDirectory(resolved, module))
                    return false;
            } else if (resolved.empty()) {
                error("Could not resolve symlink: %s\n", SC_Codecvt::path_to_utf8_str(path).c_str());
            } else {
                addFile(resolved, module);
            }
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Loader
////////////////////////////////////////////////////////////////////////////////

// The loader only loads class graphs.
const compiler::CompiledClass& classOf(const compiler::CompiledGraph& g) {
    return std::get<compiler::CompiledClass>(g.unit);
}

int instanceFormat(ir::SCClass::MemoryLayout layout) {
    using Layout = ir::SCClass::MemoryLayout;
    switch (layout) {
    case Layout::NotIndexed:
        return obj_notindexed;
    case Layout::Slot:
        return obj_slot;
    case Layout::Double:
        return obj_double;
    case Layout::Float:
        return obj_float;
    case Layout::Int32:
        return obj_int32;
    case Layout::Int16:
        return obj_int16;
    case Layout::Int8:
        return obj_int8;
    case Layout::Char:
        return obj_char;
    case Layout::Symbol:
        return obj_symbol;
    }
    return obj_notindexed;
}

PyrObject* newObject(Allocation alloc, std::size_t numBytes, int format) {
    return alloc.gc ? alloc.gc->New(numBytes, alloc.flags, format, false)
                    : PyrGC::NewPermanent(numBytes, alloc.flags, format);
}

template <typename T>
PyrObject* newRawArray(Allocation alloc, const std::vector<T>& values, int format, PyrClass* classptr) {
    const auto size = narrow<int>(values.size());
    auto* array = newObject(alloc, size * sizeof(T), format);
    array->classptr = classptr;
    array->size = size;
    if (size)
        std::memcpy(array->slots, values.data(), size * sizeof(T));
    return array;
}

PyrSymbolArray* newSymbolArray(Allocation alloc, int size) {
    auto* array = newPyrSymbolArray(alloc.gc, size, alloc.flags, false);
    array->size = size;
    return array;
}

PyrObject* newArray(Allocation alloc, int size) {
    auto* array = newPyrArray(alloc.gc, size, alloc.flags, false);
    array->size = size;
    nilSlots(array->slots, array->size);
    return array;
}

struct Source {
    PyrString* text;
    PyrSymbol* path;
    // An interpreted text's line and column in its file, nil for a class file's.
    std::optional<sc::src::FileCodeLocation> startInFile;
};

// The class library's compiler and the objects found while loading it, kept for interpreted code until the library is
// freed.
struct LoadedLibrary {
    std::unique_ptr<compiler::ClassLibrary> library;
    std::unordered_map<ir::Symbol, PyrSymbol*, std::hash<ir::Symbol>> symbols {};
    // A member's owning class, either of its names, to the class object holding its class variables and constants.
    std::unordered_map<ir::Symbol, PyrClass*, std::hash<ir::Symbol>> memberOwners {};
    // By absolute class variable index, the name of the class declaring it.
    std::vector<PyrSymbol*> classVariableOwners {};
    // Interpreted code's root function is compiled in it.
    PyrMethod* functionCompileContext = nullptr;
    // From the language config's interpretedVersion.
    sc::ast::LanguageVersion interpretedVersion = sc::ast::LanguageVersion::Legacy;
};
std::unique_ptr<LoadedLibrary> gLoadedLibrary;

#ifndef NDEBUG
// Every class and meta class made, with the superclass and methods the compiler gave it, to check the runtime's
// finalisation against, see checkFinalisedClassLibrary.
struct LoadedClass {
    PyrClass* superclass;
    std::vector<PyrMethod*> methods;
};
std::unordered_map<PyrClass*, LoadedClass> gLoadedClasses;
#endif

// Makes the VM's objects from the compiled graphs, as the old compiler made them.
struct Loader {
    Loader(LoadedLibrary& loaded, Allocation alloc):
        library(*loaded.library),
        alloc(alloc),
        symbols(loaded.symbols),
        memberOwners(loaded.memberOwners),
        classVariableOwners(loaded.classVariableOwners) {}

    const compiler::ClassLibrary& library;
    Allocation alloc;
    std::unordered_map<ir::Symbol, PyrSymbol*, std::hash<ir::Symbol>>& symbols;
    std::unordered_map<ir::Symbol, PyrClass*, std::hash<ir::Symbol>>& memberOwners;
    std::vector<PyrSymbol*>& classVariableOwners;
    std::unordered_map<const sc::src::TextInfo*, Source> sources {};

    PyrSymbol* symbol(ir::Symbol s) {
        if (const auto fnd = symbols.find(s); fnd != symbols.end())
            return fnd->second;
        const auto text = library.symbols(s);
        auto* sym = getsymlen(text.data(), text.size());
        symbols.emplace(s, sym);
        return sym;
    }

    Source source(const compiler::CompiledGraph& g, ir::SourceIndex index) {
        const auto& text = g.sources[index];
        if (const auto fnd = sources.find(text.get()); fnd != sources.end())
            return fnd->second;
        const auto& str = text->source.as_string();
        auto* pyrString = newPyrStringN(alloc.gc, narrow<int>(str.size()), alloc.flags, false);
        std::memcpy(pyrString->s, str.data(), str.size());
        const auto path = text->file_path ? SC_Codecvt::path_to_utf8_str(*text->file_path) : std::string {};
        const Source s { pyrString, getsym(path.c_str()), startInFile(*text) };
        sources.emplace(text.get(), s);
        return s;
    }

    static std::optional<sc::src::FileCodeLocation> startInFile(const sc::src::TextInfo& text) {
        if (text.kind == sc::src::TextKind::ClassFile)
            return std::nullopt;
        return text.source_start_in_file;
    }

    // Values

    PyrSlot value(ir::ConstExprIndex index) {
        const auto& constants = library.ctx.constant_table;
        return constants(index).visit(sc::util::overload {
            [](double d) { return PyrSlot::make(d); },
            [](std::int32_t i) { return PyrSlot::make(i); },
            [](char c) { return PyrSlot::make(c); },
            [](bool b) { return PyrSlot::make(b); },
            [](ir::Nil) { return PyrSlot {}; },
            [&](ir::SymSCSymbol s) { return PyrSlot::make(symbol(s)); },
            [&](ir::SymSCString s) {
                const auto text = library.symbols(s);
                auto* str = newPyrStringN(alloc.gc, narrow<int>(text.size()), alloc.flags, false);
                std::memcpy(str->s, text.data(), text.size());
                return PyrSlot::make(str);
            },
            // As a literal array's element, a class name is a symbol.
            [&](ir::SymSCClassName s) { return PyrSlot::make(symbol(s)); },
            [&](const ir::ConstExprArray& elements) {
                auto* array = newArray(alloc, narrow<int>(elements.size()));
                for (const auto i : elements.indices())
                    array->slots[*i] = value(elements[i]);
                return PyrSlot::make(array);
            },
            [&](const ir::ConstExprEvent& pairs) { return event(pairs); },
            [&](const ir::ConstExprDoubleArray& v) {
                return PyrSlot::make(newRawArray(alloc, v, obj_double, class_doublearray));
            },
            [&](const ir::ConstExprFloatArray& v) {
                return PyrSlot::make(newRawArray(alloc, v, obj_float, class_floatarray));
            },
            [&](const ir::ConstExprInt32Array& v) {
                return PyrSlot::make(newRawArray(alloc, v, obj_int32, class_int32array));
            },
            [&](const ir::ConstExprInt16Array& v) {
                return PyrSlot::make(newRawArray(alloc, v, obj_int16, class_int16array));
            },
            [&](const ir::ConstExprInt8Array& v) {
                return PyrSlot::make(newRawArray(alloc, v, obj_int8, class_int8array));
            },
            [&](const ir::ConstExprSymbolArray& v) {
                auto* array = newSymbolArray(alloc, narrow<int>(v.size()));
                for (std::size_t i = 0; i < v.size(); ++i)
                    array->symbols[i] = symbol(v[i]);
                return PyrSlot::make(array);
            },
        });
    }

    // As `Event.new(n, nil, nil, true)` then `put` for each pair would make it, see Set:initSet and identDictPut: a
    // `proto` or `parent` key sets that variable, a later key replaces an earlier one's value, and a nil value removes
    // it. The array, four slots per pair, is never full enough to grow.
    PyrSlot event(const ir::ConstExprEvent& pairs) {
        auto* protoName = getsym("proto");
        auto* parentName = getsym("parent");
        std::vector<std::pair<PyrSlot, PyrSlot>> entries;
        PyrSlot proto {};
        PyrSlot parent {};
        for (const auto& [k, v] : pairs) {
            const auto key = value(k);
            const auto val = value(v);
            if (IsSym(&key) && (slotRawSymbol(&key) == protoName || slotRawSymbol(&key) == parentName)) {
                (slotRawSymbol(&key) == protoName ? proto : parent) = val;
                continue;
            }
            const auto fnd =
                std::find_if(entries.begin(), entries.end(), [&](const auto& e) { return SlotEq(&e.first, &key); });
            if (IsNil(&val)) {
                if (fnd != entries.end())
                    entries.erase(fnd);
            } else if (fnd != entries.end()) {
                fnd->second = val;
            } else {
                entries.push_back({ key, val });
            }
        }

        auto* array = newArray(alloc, std::max(2 * narrow<int>(pairs.size()), 2) * 2);
        for (auto& [key, val] : entries) {
            PyrSlot* pair = array->slots + arrayAtIdentityHashInPairs(array, &key);
            pair[0] = key;
            pair[1] = val;
        }

        auto* eventClass = s_event->u.classobj;
        const auto numSlots = slotRawObject(&eventClass->iprototype)->size;
        auto* obj = newObject(alloc, numSlots * sizeof(PyrSlot), obj_notindexed);
        obj->classptr = eventClass;
        obj->size = numSlots;
        nilSlots(obj->slots, numSlots);
        const auto set = [&](const char* name, PyrSlot slot) {
            const auto index = instVarOffset("IdentityDictionary", name);
            assert(index >= 0);
            obj->slots[index] = slot;
        };
        set("array", PyrSlot::make(array));
        set("size", PyrSlot::make(narrow<int32_t>(entries.size())));
        set("proto", proto);
        set("parent", parent);
        set("know", PyrSlot::make(true));
        return PyrSlot::make(obj);
    }

    PyrSlot value(ir::OptionalConstExprIndex index) {
        return index ? value(ir::ConstExprIndex { *index }) : PyrSlot {};
    }

    // The slot itself, shared by identity, as the old compiler pushed the class's constValues.
    PyrSlot classConstant(const backend::ClassConstant& c) {
        auto* owner = memberOwners.at(c.owner);
        auto* names = slotRawSymbolArray(&owner->constNames);
        auto* sym = symbol(c.name);
        for (int i = 0; i < names->size; ++i)
            if (names->symbols[i] == sym)
                return slotRawObject(&owner->constValues)->slots[i];
        assert(false);
        return value(c.value);
    }

    PyrSlot switchTable(const backend::SwitchTable& table) {
        auto* array = newArray(alloc, narrow<int>(table.capacity));
        // The first of duplicate keys wins.
        for (const auto& [key, offset] : table.offsets) {
            auto keySlot = value(key);
            PyrSlot* pair = array->slots + arrayAtIdentityHashInPairs(array, &keySlot);
            if (IsNil(pair)) {
                pair[0] = keySlot;
                pair[1] = PyrSlot::make(narrow<int32_t>(*offset));
            }
        }
        for (int i = 0; i < array->size; i += 2)
            if (IsNil(array->slots + i))
                array->slots[i + 1] = PyrSlot::make(narrow<int32_t>(*table.default_offset));
        return PyrSlot::make(array);
    }

    // Classes

    // Both the class and its meta class, as the old compiler made them, with the names of their members, whose values
    // are filled once every class is made, see fillClass, as a value can be an object of any class, e.g., an Event.
    void makeClass(const compiler::CompiledGraph& g, const compiler::CompiledGraph& meta) {
        const auto& layout = classOf(g);
        const auto& metaLayout = classOf(meta);
        auto* name = symbol(layout.name);
        auto* superclassName = layout.superclass ? symbol(ir::SymSCClassName { *layout.superclass }) : nullptr;
        PyrClass* superclassobj = superclassName ? superclassName->u.classobj : nullptr;
        // Sema rejects an undefined superclass, and the classes are made in topological order.
        assert(!superclassName || superclassobj);

        const auto numInstVars = narrow<int>(layout.instance_variables.size());
        const auto numSuper = numSuperInstVars(superclassobj);
        const auto numClassVars = narrow<int>(metaLayout.class_variables.size());
        const auto numConsts = narrow<int>(metaLayout.constants.size());
        const auto numInstMethods = narrow<int>(g.methods.size() + g.accessors.size());
        const auto numClassMethods = narrow<int>(meta.methods.size() + meta.accessors.size());
        const auto format = instanceFormat(layout.memory_layout);
        assert(numSuper <= numInstVars);

        // A meta class's instance variables are Class's, copied before Class itself is remade.
        const auto copyClassInstanceVariables = [](PyrClass* metaclassobj) {
            memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
                   sizeof(PyrSlot) * classClassNumInstVars);
            memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
                   slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);
            slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
            slotRawSymbolArray(&metaclassobj->instVarNames)->size = classClassNumInstVars;
        };

        PyrClass* classobj = name->u.classobj;
        PyrClass* metaclassobj;
        if (classobj) {
            // Intrinsic, made by initClasses, sema checked the class library declares it as the VM does.
            assert(slotRawSymbol(&classobj->superclass)->u.classobj == superclassobj);
            assert(slotRawInt(&classobj->instanceFormat) == format);
            metaclassobj = classobj->classptr;
            reallocClassObj(metaclassobj, classClassNumInstVars, 0, 0, numClassMethods, format, 0);
            copyClassInstanceVariables(metaclassobj);
            reallocClassObj(classobj, numInstVars, numClassVars, numConsts, numInstMethods, format, 0);
        } else {
            auto* metaName = getmetasym(name->name);
            metaName->flags |= sym_MetaClass;
            auto* metaSuperclassName = superclassName ? getmetasym(superclassName->name) : nullptr;
            metaclassobj = newClassObj(class_class, metaName, metaSuperclassName, classClassNumInstVars, 0, 0,
                                       numClassMethods, format, 0);
            copyClassInstanceVariables(metaclassobj);
            classobj = newClassObj(metaclassobj, name, superclassName, numInstVars, numClassVars, numConsts,
                                   numInstMethods, format, 0);
            name->u.classobj = classobj;
            metaName->u.classobj = metaclassobj;
        }
#ifndef NDEBUG
        gLoadedClasses[classobj] = { superclassobj, {} };
        gLoadedClasses[metaclassobj] = { superclassobj ? superclassobj->classptr : class_class, {} };
#endif

        const auto file = source(g, layout.location.source);
        const auto charPos = PyrSlot::make(narrow<int32_t>(layout.location.range.begin.absolute));
        for (auto* c : { classobj, metaclassobj }) {
            c->filenameSym = PyrSlot::make(file.path);
            c->charPos = charPos;
        }

        if (numInstVars) {
            auto* names = slotRawSymbolArray(&classobj->instVarNames);
            if (numSuper)
                memcpy(names->symbols, slotRawSymbolArray(&superclassobj->instVarNames)->symbols,
                       sizeof(PyrSymbol*) * numSuper);
            for (int i = numSuper; i < numInstVars; ++i)
                names->symbols[i] = symbol(layout.instance_variables[i].name);
            names->size = numInstVars;
        }
        const auto setNames = [&](const std::vector<compiler::CompiledMember>& members, PyrSlot& namesSlot) {
            if (members.empty())
                return;
            auto* names = slotRawSymbolArray(&namesSlot);
            for (std::size_t i = 0; i < members.size(); ++i)
                names->symbols[i] = symbol(members[i].name);
            names->size = narrow<int>(members.size());
        };
        setNames(metaLayout.class_variables, classobj->classVarNames);
        setNames(metaLayout.constants, classobj->constNames);

        // The backend numbers class variables by meta class name.
        const auto firstClassVariable = **metaLayout.first_class_variable;
        classobj->classVarIndex = PyrSlot::make(narrow<int32_t>(firstClassVariable));
        for (int i = 0; i < numClassVars; ++i)
            classVariableOwners[firstClassVariable + i] = name;

        memberOwners[layout.name] = classobj;
        memberOwners[metaLayout.name] = classobj;
    }

    // The values of the class's members, its superclass's filled first.
    void fillClass(const compiler::CompiledGraph& g, const compiler::CompiledGraph& meta) {
        const auto& layout = classOf(g);
        auto* classobj = symbol(layout.name)->u.classobj;
        auto* superclassobj =
            layout.superclass ? symbol(ir::SymSCClassName { *layout.superclass })->u.classobj : nullptr;
        const auto numInstVars = narrow<int>(layout.instance_variables.size());
        const auto numSuper = numSuperInstVars(superclassobj);

        // The inherited instance variables are the superclass's, so their prototypes are the same objects.
        if (numInstVars) {
            auto* prototype = slotRawObject(&classobj->iprototype);
            if (numSuper)
                memcpy(prototype->slots, slotRawObject(&superclassobj->iprototype)->slots, sizeof(PyrSlot) * numSuper);
            for (int i = numSuper; i < numInstVars; ++i)
                prototype->slots[i] = value(layout.instance_variables[i].value);
            prototype->size = numInstVars;
        }
        const auto setValues = [&](const std::vector<compiler::CompiledMember>& members, PyrSlot& valuesSlot) {
            if (members.empty())
                return;
            auto* values = slotRawObject(&valuesSlot);
            for (std::size_t i = 0; i < members.size(); ++i)
                values->slots[i] = value(members[i].value);
            values->size = narrow<int>(members.size());
        };
        setValues(classOf(meta).class_variables, classobj->cprototype);
        setValues(classOf(meta).constants, classobj->constValues);
    }

    // Methods and functions

    void fillFunction(const compiler::CompiledGraph& g, backend::lir::FunctionIndex index, PyrBlock* block,
                      MethodType type) {
        const auto& f = g.functions[index];
        const auto& emitted = f.emitted;
        const auto numSlots = narrow<int>(f.slots.size());
        const int numPositional = (f.has_this ? 1 : 0) + narrow<int>(f.num_positional);
        const int numVariadic = static_cast<int>(f.variadic);
        const int numArguments = numPositional + numVariadic;
        const int numVariables = numSlots - numArguments;
        // Too many is reported, so nothing is loaded.
        assert(numSlots <= 255);

        *METHRAW(block) = PyrMethodRaw { 0,
                                         0,
                                         static_cast<unsigned short>(type),
                                         narrow<unsigned short>((numSlots + FRAMESIZE) * sizeof(PyrSlot)),
                                         0,
                                         narrow<unsigned char>(numPositional),
                                         narrow<unsigned char>(numVariadic),
                                         narrow<unsigned char>(numVariables),
                                         narrow<unsigned char>(numSlots),
                                         static_cast<unsigned char>(emitted.properties.needs_heap_context),
                                         narrow<unsigned char>(f.has_this ? numSlots - 1 : numSlots),
                                         narrow<unsigned char>(numArguments) };

        const auto file = source(g, f.location.source);
        block->sourceCodeFileOrSnippet = PyrSlot::make(file.text);
        block->filePath = PyrSlot::make(file.path);
        block->sourceCodeStartIndex = PyrSlot::make(narrow<int32_t>(f.location.range.begin.absolute));
        block->sourceCodeEndIndex = PyrSlot::make(narrow<int32_t>(f.location.range.end.absolute));
        block->isClosed = PyrSlot::make(emitted.properties.is_closed);
        // Each block's own, mutable, as the old compiler made it.
        if (file.startInFile) {
            assert(alloc.gc);
            auto* location = newPyrArray(alloc.gc, 2, 0, false);
            location->size = 2;
            location->slots[0] = PyrSlot::make(narrow<int32_t>(file.startInFile->line_number));
            location->slots[1] = PyrSlot::make(narrow<int32_t>(file.startInFile->column));
            block->fileLocation = PyrSlot::make(location);
        }

        const auto names = [&](int first, int last) {
            auto* array = newSymbolArray(alloc, last - first);
            for (int i = first; i < last; ++i)
                array->symbols[i - first] = symbol(f.slots[backend::FrameSlotIndex { narrow<std::uint32_t>(i) }].name);
            return PyrSlot::make(array);
        };
        if (f.has_this && numArguments == 1)
            block->argNames = o_argnamethis;
        else if (numArguments)
            block->argNames = names(0, numArguments);
        if (numVariables)
            block->varNames = names(numArguments, numSlots);

        if (f.has_this && numSlots == 1) {
            block->prototypeFrame = o_onenilarray;
        } else if (numSlots) {
            auto* prototype = newArray(alloc, numSlots);
            for (int i = 0; i < numSlots; ++i) {
                if (i >= numPositional && i < numArguments)
                    prototype->slots[i] = o_emptyarray;
                else
                    prototype->slots[i] =
                        value(f.slots[backend::FrameSlotIndex { narrow<std::uint32_t>(i) }].prototype);
            }
            block->prototypeFrame = PyrSlot::make(prototype);
        }

        // A method run by its shortcut has no code.
        if (!std::holds_alternative<std::monostate>(emitted.shortcut))
            return;

        const auto& code = emitted.code;
        if (!code.codes.raw().empty()) {
            auto* bytes = newPyrInt8Array(alloc.gc, narrow<int>(code.codes.size()), alloc.flags, false);
            std::memcpy(bytes->b, code.codes.raw().data(), code.codes.size());
            bytes->size = narrow<int>(code.codes.size());
            block->code = PyrSlot::make(bytes);

            const auto numLocations = narrow<int>(2 * code.locations.size());
            auto* locations = newPyrInt32Array(alloc.gc, numLocations, alloc.flags, false);
            std::memcpy(locations->i, code.locations.raw().data(), numLocations * sizeof(int32_t));
            locations->size = numLocations;
            block->codeLocations = PyrSlot::make(locations);

            auto* sizes = newPyrInt8Array(alloc.gc, narrow<int>(code.sizeOfCodes.size()), alloc.flags, false);
            std::memcpy(sizes->b, code.sizeOfCodes.raw().data(), code.sizeOfCodes.size());
            sizes->size = narrow<int>(code.sizeOfCodes.size());
            block->codeSizes = PyrSlot::make(sizes);
        }

        if (!emitted.selectors.raw().empty()) {
            auto* selectors = newArray(alloc, narrow<int>(emitted.selectors.size()));
            for (const auto i : emitted.selectors.indices())
                selectors->slots[*i] = std::visit(
                    sc::util::overload {
                        [&](ir::SymSCSelector s) { return PyrSlot::make(symbol(s)); },
                        [&](ir::SymSCClassName s) { return PyrSlot::make(symbol(s)); },
                        [&](backend::lir::FunctionIndex nested) { return PyrSlot::make(makeBlock(g, nested, block)); },
                    },
                    emitted.selectors[i]);
            block->selectors = PyrSlot::make(selectors);
        }

        if (!emitted.constants.raw().empty()) {
            auto* constants = newArray(alloc, narrow<int>(emitted.constants.size()));
            for (const auto i : emitted.constants.indices())
                constants->slots[*i] =
                    std::visit(sc::util::overload {
                                   [&](ir::ConstExprIndex v) { return value(v); },
                                   [&](ir::SymSCSymbol s) { return PyrSlot::make(symbol(s)); },
                                   [&](const backend::ClassConstant& c) { return classConstant(c); },
                                   [&](backend::SwitchTableIndex t) { return switchTable(emitted.switch_tables[t]); },
                               },
                               emitted.constants[i]);
            block->constants = PyrSlot::make(constants);
        }
    }

    PyrBlock* makeBlock(const compiler::CompiledGraph& g, backend::lir::FunctionIndex index, PyrBlock* parent) {
        const auto& f = g.functions[index];
        assert(!f.emitted.thrown_away);
        auto* block = newPyrBlock(alloc.gc != nullptr, alloc.flags);
        block->name = f.name ? PyrSlot::make(symbol(ir::SymSCSymbol { *f.name })) : PyrSlot {};
        block->contextDef = f.emitted.properties.has_context ? PyrSlot::make(parent) : PyrSlot {};
        fillFunction(g, index, block, methBlock);
        return block;
    }

    // The old compiler's payloads: a class variable's shortcut keeps its class's name.
    void setShortcut(PyrMethod* method, const backend::OptionalMethodShortcut& shortcut,
                     const compiler::CompiledGraph& g) {
        auto* raw = METHRAW(method);
        const auto set = [&](MethodType type, std::uint32_t specialIndex = 0) {
            raw->methType = static_cast<unsigned short>(type);
            raw->specialIndex = narrow<unsigned short>(specialIndex);
        };
        const auto classVariableOwner = [&](backend::ClassVariableIndex index) {
            return PyrSlot::make(classVariableOwners[*index]);
        };
        std::visit(sc::util::overload {
                       [](std::monostate) {},
                       [&](backend::shortcut::ReturnSelf) { set(methReturnSelf); },
                       [&](backend::shortcut::ReturnLiteral s) {
                           set(methReturnLiteral);
                           method->selectors = literal(s.value, g);
                       },
                       [&](backend::shortcut::ReturnArgument s) { set(methReturnArg, *s.slot); },
                       [&](backend::shortcut::ReturnInstance s) { set(methReturnInstVar, *s.slot); },
                       [&](backend::shortcut::AssignInstance s) { set(methAssignInstVar, *s.slot); },
                       [&](backend::shortcut::Redirect s) {
                           set(s.super ? methRedirectSuper : methRedirect);
                           method->selectors = PyrSlot::make(symbol(s.selector));
                       },
                       [&](backend::shortcut::ForwardInstance s) {
                           set(methForwardInstVar, *s.slot);
                           method->selectors = PyrSlot::make(symbol(s.selector));
                       },
                       [&](backend::shortcut::ForwardClassVariable s) {
                           set(methForwardClassVar, *s.index);
                           method->selectors = PyrSlot::make(symbol(s.selector));
                           method->constants = classVariableOwner(s.index);
                       },
                       [&](backend::shortcut::ReturnClassVariable s) {
                           set(methReturnClassVar, *s.index);
                           method->selectors = classVariableOwner(s.index);
                       },
                       [&](backend::shortcut::AssignClassVariable s) {
                           set(methAssignClassVar, *s.index);
                           method->selectors = classVariableOwner(s.index);
                       },
                   },
                   shortcut);
    }

    // A constant's accessor returns its constValues' slot.
    PyrSlot literal(ir::ConstExprIndex v, const compiler::CompiledGraph& g) {
        const auto& layout = classOf(g);
        if (const auto fnd = memberOwners.find(layout.name); fnd != memberOwners.end()) {
            auto* owner = fnd->second;
            const auto& constants = layout.constants;
            for (std::size_t i = 0; i < constants.size(); ++i)
                if (constants[i].value && ir::ConstExprIndex { *constants[i].value } == v)
                    return slotRawObject(&owner->constValues)->slots[i];
        }
        return value(v);
    }

    void makeMethod(const compiler::CompiledGraph& g, const compiler::CompiledMethod& m, PyrClass* owner) {
        const auto& f = g.functions[m.function];
        auto* method = newPyrMethod();
        auto* primitive = m.primitive ? symbol(ir::SymSCPrimitive { *m.primitive }) : nullptr;
        fillFunction(g, m.function, method, primitive ? methPrimitive : methNormal);
        method->ownerclass = PyrSlot::make(owner);
        method->name = PyrSlot::make(symbol(m.selector));
        method->contextDef = PyrSlot {};
        if (primitive) {
            method->primitiveName = PyrSlot::make(primitive);
            // Not the symbol's special index.
            METHRAW(method)->specialIndex = narrow<unsigned short>(primitive->u.index);
        }
        setShortcut(method, f.emitted.shortcut, g);
        addMethod(owner, method);
#ifndef NDEBUG
        gLoadedClasses.at(owner).methods.push_back(method);
#endif
    }

    void makeAccessor(const compiler::CompiledGraph& g, const compiler::CompiledAccessor& a, PyrClass* owner) {
        const bool setter = std::holds_alternative<backend::shortcut::AssignInstance>(a.shortcut)
            || std::holds_alternative<backend::shortcut::AssignClassVariable>(a.shortcut);
        const unsigned char numArgs = setter ? 2 : 1;
        auto* method = newPyrMethod();
        *METHRAW(method) =
            PyrMethodRaw { 0,      0, methNormal, 0, 0, numArgs, 0, 0, numArgs, 0, narrow<unsigned char>(numArgs - 1),
                           numArgs };
        const auto file = source(g, a.location.source);
        method->ownerclass = PyrSlot::make(owner);
        method->name = PyrSlot::make(symbol(a.selector));
        method->filePath = PyrSlot::make(file.path);
        method->isClosed = PyrSlot::make(false);
        method->sourceCodeFileOrSnippet = PyrSlot::make(file.text);
        method->sourceCodeStartIndex = PyrSlot::make(narrow<int32_t>(a.location.range.begin.absolute));
        method->sourceCodeEndIndex = PyrSlot::make(narrow<int32_t>(a.location.range.end.absolute));
        setShortcut(method, a.shortcut, g);
        addMethod(owner, method);
#ifndef NDEBUG
        gLoadedClasses.at(owner).methods.push_back(method);
#endif
    }

    // The accessors first, then the methods, as the old compiler added them.
    void makeMethods(const compiler::CompiledGraph& g) {
        auto* owner = symbol(classOf(g).name)->u.classobj;
        if (!owner)
            return;
        for (const auto& a : g.accessors)
            makeAccessor(g, a, owner);
        for (const auto& m : g.methods)
            makeMethod(g, m, owner);
    }
};

////////////////////////////////////////////////////////////////////////////////
// VM definitions
////////////////////////////////////////////////////////////////////////////////

const char* slotTypeName(int format) {
    switch (format) {
    case obj_slot:
        return "slot";
    case obj_double:
        return "double";
    case obj_float:
        return "float";
    case obj_int32:
        return "int32";
    case obj_int16:
        return "int16";
    case obj_int8:
        return "int8";
    case obj_char:
        return "char";
    case obj_symbol:
        return "symbol";
    default:
        return nullptr;
    }
}

// The intrinsic classes initClasses made, and the primitives initPrimitives defined, for sema to check.
compiler::VMDefinitions vmDefinitions() {
    compiler::VMDefinitions vm;
    for (PyrClass* c = gClassList; c; c = slotRawClass(&c->nextclass)) {
        auto* name = slotRawSymbol(&c->name);
        if (!(slotRawInt(&c->classFlags) & classIsIntrinsic) || (name->flags & sym_MetaClass))
            continue;
        compiler::IntrinsicClass intrinsic { name->name, {}, {}, {} };
        auto* superclass = slotRawSymbol(&c->superclass);
        if (superclass != s_none)
            intrinsic.superclass = superclass->name;
        if (const auto* slotType = slotTypeName(slotRawInt(&c->instanceFormat)))
            intrinsic.slot_type = slotType;
        const auto first = numSuperInstVars(superclass->u.classobj);
        if (NotNil(&c->instVarNames)) {
            auto* names = slotRawSymbolArray(&c->instVarNames);
            for (int i = first; i < names->size; ++i)
                intrinsic.instance_variables.push_back(names->symbols[i]->name);
        }
        vm.classes.push_back(std::move(intrinsic));
    }
    for (int i = 0; i < gPrimitiveTable.size; ++i) {
        const auto& prim = gPrimitiveTable.table[i];
        if (prim.func == undefinedPrimitive || !prim.name)
            continue;
        vm.primitives.push_back({ prim.name->name, prim.numNormalArguments, prim.hasVariablePositionalArguments,
                                  prim.hasVariableKeywordArguments });
    }
    return vm;
}

// The compiler requires an Interpreter class, which defines it.
PyrMethod* findFunctionCompileContext() {
    auto* interpreter = getsym("Interpreter")->u.classobj;
    assert(interpreter && IsObj(&interpreter->methods));
    auto* methods = slotRawObject(&interpreter->methods);
    auto* selector = getsym("functionCompileContext");
    for (int i = 0; i < methods->size; ++i) {
        auto* method = slotRawMethod(&methods->slots[i]);
        if (slotRawSymbol(&method->name) == selector)
            return method;
    }
    assert(false && "Interpreter has no functionCompileContext");
    return nullptr;
}

sc::ast::LanguageVersion interpretedVersion() {
    const auto& release = gLanguageConfig->getInterpretedVersion();
    if (release.empty())
        return sc::ast::LanguageVersion::V3_16;
    if (const auto version = sc::ast::parse_release(release))
        return *version;
    post("WARNING: interpretedVersion '%s' in the language config isn't a known release, e.g., 3.16, or legacy, using "
         "legacy\n",
         release.c_str());
    return sc::ast::LanguageVersion::Legacy;
}

std::size_t postDiagnostic(const sc::diag::Diagnostic& d, std::size_t& errors) {
    if (d.severity() == sc::diag::Severity::Error)
        ++errors;
    const auto text = d.to_string();
    postText(text.c_str(), text.size());
    postText("\n", 1);
    return 1;
}

using ClassAndMeta = std::pair<const compiler::CompiledGraph*, const compiler::CompiledGraph*>;

void makeClassNamesAsOldCompiler(Loader& loader, const compiler::ClassFiles& files,
                                 const std::vector<compiler::CompiledGraph>& graphs) {
    std::map<fs::path, ir::SourceEntryIndex> fileOrder;
    for (const auto i : files.indices())
        fileOrder.emplace(files[i].path, i);
    struct Definition {
        ir::SourceEntryIndex file;
        std::uint32_t position;
        const compiler::CompiledClass* layout;
    };
    std::vector<Definition> definitions;
    for (const auto& g : graphs) {
        const auto& layout = classOf(g);
        if (loader.library.symbols.is_meta(layout.name))
            continue;
        const auto& path = g.sources[layout.location.source]->file_path;
        definitions.push_back(
            { path ? fileOrder.at(*path) : files.next_index(), layout.location.range.begin.absolute, &layout });
    }
    std::sort(definitions.begin(), definitions.end(), [](const Definition& l, const Definition& r) {
        return std::pair { *l.file, l.position } < std::pair { *r.file, r.position };
    });
    for (const auto& d : definitions) {
        loader.symbol(d.layout->name);
        if (d.layout->superclass)
            loader.symbol(ir::SymSCClassName { *d.layout->superclass });
    }
}

void orderAsOldCompiler(Loader& loader, std::vector<ClassAndMeta>& classes) {
    std::unordered_map<PyrSymbol*, int> slots;
    for (const auto& [g, meta] : classes)
        slots.emplace(loader.symbol(classOf(*g).name), 0);
    const auto* table = gMainVMGlobals->symbolTable;
    for (int i = 0; i < table->TableSize(); ++i)
        if (const auto fnd = slots.find(table->Get(i)); fnd != slots.end())
            fnd->second = i;

    std::unordered_map<PyrSymbol*, int> depths;
    std::unordered_map<const compiler::CompiledGraph*, std::pair<int, int>> keys;
    for (const auto& [g, meta] : classes) {
        const auto& layout = classOf(*g);
        auto* name = loader.symbol(layout.name);
        int depth = 0;
        if (layout.superclass)
            if (const auto fnd = depths.find(loader.symbol(ir::SymSCClassName { *layout.superclass }));
                fnd != depths.end())
                depth = fnd->second + 1;
        depths.emplace(name, depth);
        keys.emplace(g, std::pair { depth, slots.at(name) });
    }
    std::sort(classes.begin(), classes.end(),
              [&](const ClassAndMeta& l, const ClassAndMeta& r) { return keys.at(l.first) < keys.at(r.first); });
}
}

bool loadClassLibrary(std::size_t& thingsPosted) {
#ifndef NDEBUG
    gLoadedClasses.clear();
#endif
    ClassFileFinder finder;
    ir::ModuleIndex module { 0 };
    const bool found = gLanguageConfig->forEachIncludedDirectory([&](const fs::path& dir) {
        const bool ok = finder.addDirectory(dir, module);
        module = sc::util::advance(module, 1u);
        return ok;
    });
    if (!found)
        return false;

    compiler::ClassLibraryTimes times;
    const auto numThreads = std::max(1u, std::thread::hardware_concurrency());
    auto library = compiler::compile_class_library(finder.files, numThreads, times, vmDefinitions());

    std::size_t errors { 0 };
    if (!library->parse_log.empty()) {
        postText(library->parse_log.c_str(), library->parse_log.size());
        ++errors;
    }
    for (const auto& d : library->parse_diagnostics)
        thingsPosted += postDiagnostic(d, errors);
    for (const auto& d : library->diagnostics)
        thingsPosted += postDiagnostic(d, errors);

    // In topological order, so a superclass is made before its subclasses. With an error, a class may be poisoned and
    // not compiled, so nothing is made.
    const auto classes = compiler::compile_class_graphs(*library, numThreads);
    for (const auto& g : classes.raw())
        for (const auto& d : g.diagnostics)
            thingsPosted += postDiagnostic(d, errors);
    if (errors)
        return false;

    auto loaded = std::make_unique<LoadedLibrary>();
    loaded->library = std::move(library);
    loaded->classVariableOwners.resize(loaded->library->backend->members.num_class_variables(), nullptr);
    Loader loader { *loaded, classLibraryAllocation };
    const auto& lib = loader.library;
    makeClassNamesAsOldCompiler(loader, finder.files, classes.raw());
    std::unordered_map<PyrSymbol*, const compiler::CompiledGraph*> metaClasses;
    for (const auto& g : classes.raw())
        if (lib.symbols.is_meta(classOf(g).name))
            metaClasses.emplace(loader.symbol(classOf(g).name), &g);
    // Every class graph has its meta class's, they are only poisoned together, which is an error.
    std::vector<ClassAndMeta> classAndMeta;
    for (const auto& g : classes.raw())
        if (!lib.symbols.is_meta(classOf(g).name)) {
            const auto fnd = metaClasses.find(getmetasym(loader.symbol(classOf(g).name)->name));
            assert(fnd != metaClasses.end());
            classAndMeta.push_back({ &g, fnd->second });
        }
    orderAsOldCompiler(loader, classAndMeta);
    for (const auto& [g, meta] : classAndMeta)
        loader.makeClass(*g, *meta);
    for (const auto& [g, meta] : classAndMeta)
        loader.fillClass(*g, *meta);
    for (const auto& g : classes.raw())
        loader.makeMethods(g);
    loaded->functionCompileContext = findFunctionCompileContext();
    loaded->interpretedVersion = interpretedVersion();
    post("\tInterpreted code is compiled as %s\n", sc::ast::to_string(loaded->interpretedVersion));
    gNumClassVars = narrow<int>(lib.backend->members.num_class_variables());
    gLoadedLibrary = std::move(loaded);
    return true;
}

void unloadClassLibrary() { gLoadedLibrary.reset(); }

compiler::ClassLibrary* loadedClassLibrary() { return gLoadedLibrary ? gLoadedLibrary->library.get() : nullptr; }

PyrBlock* loadInterpreted(VMGlobals* g, const compiler::CompiledGraph& compiled, PyrString* text, PyrSymbol* path,
                          PyrBlock* context) {
    assert(gLoadedLibrary);
    const auto& functions = std::get<compiler::CompiledInterpreted>(compiled.unit).functions;
    assert(functions.size() == 1 && compiled.sources.size() == 1);
    Loader loader { *gLoadedLibrary, { g->gc, obj_immutable } };
    const auto& source = compiled.sources.raw().front();
    loader.sources.emplace(source.get(), Source { text, path, Loader::startInFile(*source) });
    return loader.makeBlock(compiled, functions.front(), context);
}

PyrBlock* compileInterpreted(VMGlobals* g, std::string_view code, PyrSymbol* path,
                             sc::src::FileCodeLocation startInFile) {
    assert(gLoadedLibrary);
    auto& library = *gLoadedLibrary->library;
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { code.data(), code.size() }, startInFile,
                                                    path ? std::optional<fs::path> { path->name } : std::nullopt,
                                                    sc::src::TextKind::InterpretedFunction);

    std::size_t errors { 0 };
    auto parsed = compiler::parse_interpreted(text);
    for (const auto& d : parsed.diagnostics)
        postDiagnostic(d, errors);
    auto ir = compiler::compile_interpreted_ir(library, std::move(parsed), gLoadedLibrary->interpretedVersion);
    for (const auto& d : ir.diagnostics)
        postDiagnostic(d, errors);
    if (errors || !ir.graph)
        return nullptr;

    const auto& graph = *ir.graph;
    auto framed = compiler::layout_frames(library, graph, std::get<ir::SCInterpreted_I>(*graph.root()));
    const auto compiled =
        compiler::finish(library, compiler::emit(library, compiler::lower(library, std::move(framed))));
    for (const auto& d : compiled.diagnostics)
        postDiagnostic(d, errors);
    if (errors)
        return nullptr;
    // Unimplemented by the backend, e.g., a `~x` pack target.
    if (std::get<compiler::CompiledInterpreted>(compiled.unit).functions.empty()) {
        post("ERROR: the new compiler can't compile this code yet.\n");
        return nullptr;
    }

    const auto& normalised = text->source.as_string();
    auto* string = newPyrString(g->gc, normalised.c_str(), 0, false);
    return loadInterpreted(g, compiled, string, path ? path : getsym("interpreted_text"),
                           gLoadedLibrary->functionCompileContext);
}

void checkFinalisedClassLibrary() {
#ifndef NDEBUG
    const auto loaded = [](PyrClass* c) -> const LoadedClass& { return gLoadedClasses.at(c); };

    // The class tree, built from the superclasses, has every class.
    assert(narrow<std::size_t>(gNumClasses) == gLoadedClasses.size());
    std::unordered_map<PyrClass*, int> numSubclasses;
    for (const auto& [c, l] : gLoadedClasses) {
        assert((IsNil(&c->superclass) ? nullptr : slotRawSymbol(&c->superclass)->u.classobj) == l.superclass);
        if (l.superclass)
            ++numSubclasses[l.superclass];
    }
    for (const auto& [c, l] : gLoadedClasses) {
        const auto* subclasses = IsObj(&c->subclasses) ? slotRawObject(&c->subclasses) : nullptr;
        assert((subclasses ? subclasses->size : 0) == numSubclasses[c]);
        for (int i = 0; subclasses && i < subclasses->size; ++i)
            assert(loaded(slotRawClass(&subclasses->slots[i])).superclass == c);
    }

    // Each class variable has one slot.
    std::vector<bool> used(narrow<std::size_t>(gNumClassVars), false);
    for (const auto& [c, l] : gLoadedClasses) {
        const auto first = slotRawInt(&c->classVarIndex);
        for (int i = 0; i < numClassVars(c); ++i) {
            assert(!used.at(narrow<std::size_t>(first + i)));
            used[first + i] = true;
        }
    }
    assert(std::all_of(used.begin(), used.end(), [](bool b) { return b; }));

    // A selector finds the nearest class's method, a class has one method per selector.
    for (const auto& [c, l] : gLoadedClasses) {
        std::unordered_set<PyrSymbol*> own;
        for (auto* m : l.methods)
            assert(own.insert(slotRawSymbol(&m->name)).second);
        std::unordered_set<PyrSymbol*> seen;
        for (PyrClass* ancestor = c; ancestor; ancestor = loaded(ancestor).superclass)
            for (auto* m : loaded(ancestor).methods) {
                auto* selector = slotRawSymbol(&m->name);
                if (seen.insert(selector).second)
                    assert(gRowTable[slotRawInt(&c->classIndex) + selector->u.index] == m);
            }
    }
    gLoadedClasses.clear();
#endif
}
