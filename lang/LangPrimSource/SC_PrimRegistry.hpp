/**
 * \file SC_PrimRegistry.hpp
 *
 * TODO description
 */
#ifndef SC_PRIMREGISTRY_HPP_INCLUDED
#define SC_PRIMREGISTRY_HPP_INCLUDED

#include <vector>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

#include "PyrPrimitive.h"
#include "SC_PrimRegistryMacros.hpp"
#include "PyrSymbolTable.h"

template<typename Entry>
class SC_PrimRegistry {
public:

    void run_all() const
    {
        for (auto const& entry : m_entries)
            entry();
    }

    void add_entry(Entry&& entry)
    {
        m_entries.push_back(entry);
    }

    static SC_PrimRegistry<Entry>& instance()
    {
        static SC_PrimRegistry<Entry> inst{};
        return inst;
    }

    SC_PrimRegistry() = default;
    SC_PrimRegistry operator=(SC_PrimRegistry const&) = delete;
    SC_PrimRegistry(SC_PrimRegistry const&) = delete;

private:
    std::vector<Entry> m_entries;
};

namespace detail {

template <typename T>
class SC_PrimRegistryEntry
{
public:
    using Registry_t = SC_PrimRegistry<SC_PrimRegistryEntry<T>>;

    SC_PrimRegistryEntry(T&& entry, Registry_t& reg) : m_entry(entry)
    {
        reg.add_entry(std::move(*this));
    }

    void operator()() const { m_entry(); }

private:
    T const m_entry;
};

struct SC_PrimDefinition
{
    PrimitiveHandler const m_primitive;
    char const * const m_symbol;
    int const m_numArgs;
    int const m_varArgs;

    void operator()() const
    {
        definePrimitive(nextPrimitiveIndex(), 0, m_symbol, m_primitive, m_numArgs, m_varArgs);
    }
};

struct SC_SymbolDefinition
{
    PyrSymbol ** const m_symbol;
    char const * const m_string;
    void operator()() const { *m_symbol = getsym(m_string); }
};

struct SC_InitializerDefinition
{
    void (*m_func)();
    void operator()() const { (*m_func)(); }
};

struct SC_DeinitializerDefinition
{
    void (*m_func)();
    void operator()() const { (*m_func)(); }
};

} // namespace detail

// Global registry types
using SC_PrimDefiner = SC_PrimRegistry<detail::SC_PrimRegistryEntry<detail::SC_PrimDefinition>>;
using SC_SymbolDefiner = SC_PrimRegistry<detail::SC_PrimRegistryEntry<detail::SC_SymbolDefinition>>;
using SC_InitializerDefiner = SC_PrimRegistry<detail::SC_PrimRegistryEntry<detail::SC_InitializerDefinition>>;
using SC_DeinitializerDefiner = SC_PrimRegistry<detail::SC_PrimRegistryEntry<detail::SC_DeinitializerDefinition>>;

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

// ------------ symbols ----------------------

/// Use to define a symbol. During interpreter init the symbol will be reloaded using getsym
#define SCLANG_DEFINE_SYMBOL( name, value )                                                           \
    SCLANG_SYMBOL_DECL( name );                                                                       \
    SCLANG_SYMBOL_DEFINER( name, value )

/// Use to define a symbol whose variable is \c s_name and whose value is \c "name".
#define SCLANG_DEFINE_SYMBOL_SIMPLE( name )                                                           \
    SCLANG_SYMBOL_DECL( BOOST_PP_CAT( s_, name ) );                                                   \
    SCLANG_SYMBOL_DEFINER( BOOST_PP_CAT( s_, name), BOOST_PP_STRINGIZE( name ) )

// ------------ custom init and deinit -------

/// Use to define a custom initializing method that will run after those for primitives and symbols.
#define SCLANG_DEFINE_CUSTOM_INITIALIZER( name )                                                      \
    SCLANG_DEFINE_CUSTOM_FUNCTION( Initializer, name )

/// Use to define a custom deinitializing method that will run during language shutdown.
#define SCLANG_DEFINE_CUSTOM_DEINITIALIZER( name )                                                    \
    SCLANG_DEFINE_CUSTOM_FUNCTION( Deinitializer, name )

// ------------ libsclang macros -------------

/// Use in a translation unit that uses any of these macros.
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
    char LIBSCLANG_DUMMY_VAR_LOCAL( name ) = LIBSCLANG_DUMMY_VAR( name )

#endif // SC_PRIMREGISTRY_HPP_INCLUDED
