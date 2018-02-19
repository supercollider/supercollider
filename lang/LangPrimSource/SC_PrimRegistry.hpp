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

// utilities
#define SCLANG_NUMARGS_MSG "sclang primitive must have at least 1 argument"
#define SCLANG_VARARGS_MSG "sclang primitive's varargs quality must be either 0 or 1"
#define SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs )                                           \
    static_assert( numArgs > 0, SCLANG_NUMARGS_MSG );                                                 \
    static_assert( varArgs == 0 || varArgs == 1, SCLANG_VARARGS_MSG )

/// Builds the C++ primitive function name
#define SCLANG_PRIMITIVE_NAME( name )                                                                 \
    BOOST_PP_CAT( pr, name )

/// Function signature for the C++ primitive
#define SCLANG_PRIMITIVE_SIGNATURE( name )                                                            \
    int SCLANG_PRIMITIVE_NAME( name ) ( struct VMGlobals *g, int numArgsPushed )

/// Function signature for a templated C++ primitive
#define SCLANG_TEMPLATE_PRIMITIVE_SIGNATURE( name, T )                                                \
    template <typename T>                                                                             \
    SCLANG_PRIMITIVE_SIGNATURE( name )

/// SuperCollider primitive symbol
#define SCLANG_PRIMITIVE_SYMBOL( name )                                                               \
    BOOST_PP_STRINGIZE( BOOST_PP_CAT( _, name) )

#define SCLANG_REGISTRY_HELPER_DECL( name )                                                           \
    static SC_PrimRegistryHelper BOOST_PP_CAT( name, _RegistryHelper )

#define SCLANG_REGISTRY_HELPER_DEFN( name, numArgs, varArgs )                                         \
    SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs );                                              \
    SCLANG_REGISTRY_HELPER_DECL( name ) {                                                             \
        & SCLANG_PRIMITIVE_NAME( name ),                                                              \
        SCLANG_PRIMITIVE_SYMBOL( name ),                                                              \
        numArgs,                                                                                      \
        varArgs,                                                                                      \
        SC_PrimRegistry::instance()                                                                   \
    };

/// Generates C++ function signature and registers with SC_PrimRegistry; private, do not use directly
#define SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, varArgs )                                      \
    SCLANG_PRIMITIVE_SIGNATURE( name );                                                               \
    SCLANG_REGISTRY_HELPER_DEFN( name, numArgs, varArgs );                                            \
    SCLANG_PRIMITIVE_SIGNATURE( name )

/// Use to define an ordinary (non-varArgs) primitive.
#define SCLANG_DEFINE_PRIMITIVE( name, numArgs )                                                      \
    SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, 0 )

/// Use to define a varArgs primitive.
#define SCLANG_DEFINE_VARARGS_PRIMITIVE( name, numArgs )                                              \
    SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, 1 )

// ----------- template primitives -------------

/// Helper, do not use directly.
#define SCLANG_REGISTER_TEMPLATE_PRIMITIVE( name, numArgs, varArgs, Type )                            \
    SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs );                                              \
    SCLANG_REGISTRY_HELPER_DECL( BOOST_PP_CAT( name, BOOST_PP_CAT(_, Type) ) ) {                      \
        & SCLANG_PRIMITIVE_NAME( name )<Type>,                                                        \
        SCLANG_PRIMITIVE_SYMBOL( BOOST_PP_CAT( name, BOOST_PP_CAT(_, Type) ) ),                       \
        numArgs,                                                                                      \
        varArgs,                                                                                      \
        SC_PrimRegistry::instance()                                                                   \
    }

/// Helper, do not use directly.
#define SCLANG_REGISTER_TEMPLATE_PRIMITIVE_HELPER( r, data, elem )                                    \
    SCLANG_REGISTER_TEMPLATE_PRIMITIVE(                                                               \
        BOOST_PP_SEQ_ELEM(0, data),                                                                   \
        BOOST_PP_SEQ_ELEM(1, data),                                                                   \
        BOOST_PP_SEQ_ELEM(2, data),                                                                   \
        elem);

/**
 * Use to define a templated primitive.
 *
 * T is the typename for the template, and TypeSeq is a preprocessor-seq of type names with which
 * the primitive will be instantiated. TypeSeq is of the form "(T1)(T2)(T3)" (no quotes).
 */
#define SCLANG_DEFINE_TEMPLATE_PRIMITIVE( name, T, numArgs, TypeSeq )                                 \
    SCLANG_TEMPLATE_PRIMITIVE_SIGNATURE( name, T );                                                   \
    BOOST_PP_SEQ_FOR_EACH( SCLANG_REGISTER_TEMPLATE_PRIMITIVE_HELPER, (name)(numArgs)(0), TypeSeq );  \
    SCLANG_TEMPLATE_PRIMITIVE_SIGNATURE( name, T )

// ------------ libsclang macros -------------

// utility
#define LIBSCLANG_DUMMY_VAR( name )                                                                   \
    libsclang_dummy_var_ ## name

#define LIBSCLANG_DUMMY_VAR_LOCAL( name )                                                             \
    libsclang_dummy_var_ ## name ## _local

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
