/**
 * \file SC_PrimRegistryMacros.hpp
 *
 * Helper macros for SC_PrimRegistry.hpp. The macros here should not be used directly.
 */
#ifndef SC_PRIMREGISTRYMACROS_HPP_INCLUDED
#define SC_PRIMREGISTRYMACROS_HPP_INCLUDED

#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

// utilities ----------------------------------------------------------------------------------------

#define SCLANG_NUMARGS_MSG "sclang primitive must have a nonnegative number of arguments"
#define SCLANG_VARARGS_MSG "sclang primitive's varargs quality must be either 0 or 1"
#define SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs )                                           \
    static_assert( numArgs >= 0, SCLANG_NUMARGS_MSG );                                                \
    static_assert( varArgs == 0 || varArgs == 1, SCLANG_VARARGS_MSG )

// Makes it easy to refer to the global definer for Primitives, Symbols, etc.
#define SCLANG_REGISTRY_INSTANCE( Type )                                                              \
    BOOST_PP_SEQ_CAT( (SC_)(Type)(Definer::instance()) )

// Makes it easy to declare the type of an entry for Primitives, Symbols, etc.
#define SCLANG_REGISTRY_ENTRY_TYPE( Type )                                                            \
    BOOST_PP_SEQ_CAT( (detail::SC_PrimRegistryEntry<detail::SC_)(Type)(Definition>) )

// A unique (semi-unique) name for this registry entry definer
#define SCLANG_REGISTRY_ENTRY_DEFINER_UNIQUE_NAME( Type, name )                                       \
    BOOST_PP_SEQ_CAT( (name)(_)(Type)(DefinerEntry) )

// A declaration for a registry entry definer
#define SCLANG_REGISTRY_ENTRY_DEFINER_DECL( Type, name )                                              \
    static SCLANG_REGISTRY_ENTRY_TYPE( Type ) SCLANG_REGISTRY_ENTRY_DEFINER_UNIQUE_NAME( Type, name )

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

#define SCLANG_REGISTRY_HELPER_DEFN( name, numArgs, varArgs )                                         \
    SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs );                                              \
    SCLANG_REGISTRY_ENTRY_DEFINER_DECL( Prim, name ) {                                                \
        {                                                                                             \
            & SCLANG_PRIMITIVE_NAME( name ),                                                          \
            SCLANG_PRIMITIVE_SYMBOL( name ),                                                          \
            numArgs,                                                                                  \
            varArgs                                                                                   \
        },                                                                                            \
        SCLANG_REGISTRY_INSTANCE( Prim )                                                              \
    };

/// Generates C++ function signature and registers with SC_PrimRegistry; private, do not use directly
#define SCLANG_DEFINE_PRIMITIVE_HELPER( name, numArgs, varArgs )                                      \
    SCLANG_PRIMITIVE_SIGNATURE( name );                                                               \
    SCLANG_REGISTRY_HELPER_DEFN( name, numArgs, varArgs );                                            \
    SCLANG_PRIMITIVE_SIGNATURE( name )

// ----------- template primitives -------------

/// Helper, do not use directly.
#define SCLANG_REGISTER_TEMPLATE_PRIMITIVE( name, numArgs, varArgs, Type )                            \
    SCLANG_PRIMITIVE_STATIC_ASSERTS( numArgs, varArgs );                                              \
    SCLANG_REGISTRY_ENTRY_DEFINER_DECL( Prim, BOOST_PP_SEQ_CAT( (name)(_)(Type) ) ) {                 \
        {                                                                                             \
            & SCLANG_PRIMITIVE_NAME( name )<Type>,                                                    \
            SCLANG_PRIMITIVE_SYMBOL( BOOST_PP_SEQ_CAT( (name)(_)(Type) ) ),                           \
            numArgs,                                                                                  \
            varArgs                                                                                   \
        },                                                                                            \
        SCLANG_REGISTRY_INSTANCE( Prim )                                                              \
    }

/// Helper, do not use directly.
#define SCLANG_REGISTER_TEMPLATE_PRIMITIVE_HELPER( r, data, elem )                                    \
    SCLANG_REGISTER_TEMPLATE_PRIMITIVE(                                                               \
        BOOST_PP_SEQ_ELEM(0, data),                                                                   \
        BOOST_PP_SEQ_ELEM(1, data),                                                                   \
        BOOST_PP_SEQ_ELEM(2, data),                                                                   \
        elem);

// ------------ symbols ----------------------

#define SCLANG_SYMBOL_DECL( name )                                                                    \
    PyrSymbol * name;

#define SCLANG_SYMBOL_DEFINER( name, value )                                                          \
    SCLANG_REGISTRY_ENTRY_DEFINER_DECL( Symbol, name ) {                                              \
        { &name, value },                                                                             \
        SCLANG_REGISTRY_INSTANCE( Symbol )                                                            \
    }

// ------------ custom initializers ----------

#define SCLANG_CUSTOM_INITIALIZER_DECL( name )                                                        \
    void name()

#define SCLANG_CUSTOM_INITIALIZER_DEFINER( name )                                                     \
    SCLANG_REGISTRY_ENTRY_DEFINER_DECL( Initializer, name ) {                                         \
        { &name },                                                                                    \
        SCLANG_REGISTRY_INSTANCE( Initializer )                                                       \
    }

// ------------ libsclang macros -------------

// utility
#define LIBSCLANG_DUMMY_VAR( name )                                                                   \
    libsclang_dummy_var_ ## name

#define LIBSCLANG_DUMMY_VAR_LOCAL( name )                                                             \
    libsclang_dummy_var_ ## name ## _local

#endif // SC_PRIMREGISTRYMACROS_HPP_INCLUDED
