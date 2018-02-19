/**
 * \file SC_PrimRegistry.hpp
 *
 * TODO description
 */
#ifndef SC_PRIMREGISTRY_HPP_INCLUDED
#define SC_PRIMREGISTRY_HPP_INCLUDED

#include <vector>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

#include "PyrPrimitive.h"
#include "SC_PrimRegistryMacros.hpp"

class SC_PrimRegistry {
public:
    struct Entry {
        PrimitiveHandler const primitive;
        const char * symbol;
        int numArgs;
        int varArgs;

        Entry(PrimitiveHandler const primitive, const char * symbol, int numArgs, int varArgs) :
            primitive{primitive},
            symbol{symbol},
            numArgs{numArgs},
            varArgs{varArgs}
        {}
    };

    void run_all() const
    {
        for (auto const& entry : m_entries) {
            definePrimitive(
                    nextPrimitiveIndex(),
                    0,
                    entry.symbol,
                    entry.primitive,
                    entry.numArgs,
                    entry.varArgs
                    );
        }
    }

    void add_entry(Entry&& entry)
    {
        m_entries.push_back(entry);
    }

    // singleton
    static SC_PrimRegistry& instance()
    {
        static SC_PrimRegistry reg{};
        return reg;
    }

    SC_PrimRegistry() = default;
    SC_PrimRegistry(SC_PrimRegistry const&) = delete;
    SC_PrimRegistry(SC_PrimRegistry&&) = delete;

private:
    std::vector<Entry> m_entries;
};

struct SC_PrimRegistryHelper {
    SC_PrimRegistryHelper(
            PrimitiveHandler const primitive,
            const char * const symbol,
            int numArgs,
            int varArgs,
            SC_PrimRegistry& reg)
    {
        reg.add_entry( {primitive, symbol, numArgs, varArgs} );
    }
};
/// Use to define an ordinary (non-varArgs) primitive.
#define SCLANG_DEFINE_PRIMITIVE( name, numArgs )                                                      \
    SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, 0 )

/// Use to define a varArgs primitive.
#define SCLANG_DEFINE_VARARGS_PRIMITIVE( name, numArgs )                                              \
    SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, 1 )

/**
 * Use to define a templated primitive.
 *
 * \c T is the typename for the template, and \c TypeSeq is a preprocessor-seq of type names with
 * which the primitive will be instantiated. TypeSeq is of the form "(T1)(T2)(T3)" (no quotes).
 */
#define SCLANG_DEFINE_TEMPLATE_PRIMITIVE( name, T, numArgs, TypeSeq )                                 \
    SCLANG_TEMPLATE_PRIMITIVE_SIGNATURE( name, T );                                                   \
    BOOST_PP_SEQ_FOR_EACH( SCLANG_REGISTER_TEMPLATE_PRIMITIVE_HELPER, (name)(numArgs)(0), TypeSeq );  \
    SCLANG_TEMPLATE_PRIMITIVE_SIGNATURE( name, T )

// ------------ libsclang macros -------------

/// Use at the top of a file containing primitives
#define LIBSCLANG_PRIMITIVE_GROUP( name )                                                             \
    char LIBSCLANG_DUMMY_VAR( name )

/**
 * Use in \c initPrimitives() to initialize the primitive group
 *
 * This macro is necessary because libsclang is statically linked into sclang, which means that
 * static/dynamic initialization are not guaranteed to happen. We need to trigger it manually by
 * referencing some external symbol. In this case we reference a dummy char that is created in
 * another TU with \c LIBSCLANG_PRIMITIVE_GROUP.
 */
#define INIT_LIBSCLANG_PRIMITIVE_GROUP( name )                                                        \
    extern char LIBSCLANG_DUMMY_VAR( name );                                                          \
    char LIBSCLANG_DUMMY_VAR_LOCAL( name ) = LIBSCLANG_DUMMY_VAR( name );                             \
    (void)LIBSCLANG_DUMMY_VAR_LOCAL( name )

#endif // SC_PRIMREGISTRY_HPP_INCLUDED
