
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4100) // unreferenced formal parameter
#endif

#include <boost/concept_check.hpp>

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/limits.hpp>
#include "../helpers/check_return_type.hpp"

typedef long double comparison_type;

template <class T> void sink(T const&) {}
template <class T> T rvalue(T const& v) { return v; }

template <class X, class T>
void container_test(X& r, T const&)
{
    typedef BOOST_DEDUCED_TYPENAME X::iterator iterator;
    typedef BOOST_DEDUCED_TYPENAME X::const_iterator const_iterator;
    typedef BOOST_DEDUCED_TYPENAME X::difference_type difference_type;
    typedef BOOST_DEDUCED_TYPENAME X::size_type size_type;

    typedef BOOST_DEDUCED_TYPENAME boost::iterator_value<iterator>::type iterator_value_type;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_value<const_iterator>::type const_iterator_value_type;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<iterator>::type iterator_difference_type;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<const_iterator>::type const_iterator_difference_type;

    typedef BOOST_DEDUCED_TYPENAME X::value_type value_type;
    typedef BOOST_DEDUCED_TYPENAME X::reference reference;
    typedef BOOST_DEDUCED_TYPENAME X::const_reference const_reference;

    // value_type

    BOOST_MPL_ASSERT((boost::is_same<T, value_type>));
    boost::function_requires<boost::CopyConstructibleConcept<X> >();

    // reference_type / const_reference_type

    BOOST_MPL_ASSERT((boost::is_same<T&, reference>));
    BOOST_MPL_ASSERT((boost::is_same<T const&, const_reference>));

    // iterator

    boost::function_requires<boost::InputIteratorConcept<iterator> >();
    BOOST_MPL_ASSERT((boost::is_same<T, iterator_value_type>));
    BOOST_MPL_ASSERT((boost::is_convertible<iterator, const_iterator>));

    // const_iterator

    boost::function_requires<boost::InputIteratorConcept<const_iterator> >();
    BOOST_MPL_ASSERT((boost::is_same<T, const_iterator_value_type>));

    // difference_type

    BOOST_MPL_ASSERT((boost::mpl::bool_<
                std::numeric_limits<difference_type>::is_signed>));
    BOOST_MPL_ASSERT((boost::mpl::bool_<
                std::numeric_limits<difference_type>::is_integer>));
    BOOST_MPL_ASSERT((boost::is_same<difference_type,
                iterator_difference_type>));
    BOOST_MPL_ASSERT((boost::is_same<difference_type,
                const_iterator_difference_type>));

    // size_type

    BOOST_MPL_ASSERT_NOT((boost::mpl::bool_<
                std::numeric_limits<size_type>::is_signed>));
    BOOST_MPL_ASSERT((boost::mpl::bool_<
                std::numeric_limits<size_type>::is_integer>));

    // size_type can represent any non-negative value type of difference_type
    // I'm not sure about either of these tests...
    size_type max_diff((std::numeric_limits<difference_type>::max)());
    difference_type converted_diff(max_diff);
    BOOST_CHECK((std::numeric_limits<difference_type>::max)()
            == converted_diff);

    BOOST_CHECK(
        static_cast<comparison_type>(
            (std::numeric_limits<size_type>::max)()) >
        static_cast<comparison_type>(
            (std::numeric_limits<difference_type>::max)()));

    // I don't test the runtime post-conditions here.
    X u;
    BOOST_CHECK(u.size() == 0);
    BOOST_CHECK(X().size() == 0);

    X a,b;

    sink(X(a));
    X u2(a);
    X u3 = a;

    X* ptr = new X();
    X& a1 = *ptr;
    (&a1)->~X();

    X const a_const;
    test::check_return_type<iterator>::equals(a.begin());
    test::check_return_type<const_iterator>::equals(a_const.begin());
    test::check_return_type<const_iterator>::equals(a.cbegin());
    test::check_return_type<const_iterator>::equals(a_const.cbegin());
    test::check_return_type<iterator>::equals(a.end());
    test::check_return_type<const_iterator>::equals(a_const.end());
    test::check_return_type<const_iterator>::equals(a.cend());
    test::check_return_type<const_iterator>::equals(a_const.cend());

    a.swap(b);
    test::check_return_type<X>::equals_ref(r = a);
    test::check_return_type<size_type>::equals(a.size());
    test::check_return_type<size_type>::equals(a.max_size());
    test::check_return_type<bool>::convertible(a.empty());

    // Allocator

    typedef BOOST_DEDUCED_TYPENAME X::allocator_type allocator_type;
    test::check_return_type<allocator_type>::equals(a_const.get_allocator());
}

template <class X, class Key>
void unordered_set_test(X&, Key const&)
{
    typedef BOOST_DEDUCED_TYPENAME X::value_type value_type;
    typedef BOOST_DEDUCED_TYPENAME X::key_type key_type;

    BOOST_MPL_ASSERT((boost::is_same<value_type, key_type>));
}

template <class X, class Key, class T>
void unordered_map_test(X& r, Key const& k, T const& v)
{
    typedef BOOST_DEDUCED_TYPENAME X::value_type value_type;
    typedef BOOST_DEDUCED_TYPENAME X::key_type key_type;
    BOOST_MPL_ASSERT((boost::is_same<value_type, std::pair<key_type const, T> >));

    r.insert(std::pair<Key const, T>(k, v));

    Key k_lvalue(k);
    T v_lvalue(v);

    r.emplace(k, v);
    r.emplace(k_lvalue, v_lvalue);
    r.emplace(rvalue(k), rvalue(v));
}

template <class X>
void equality_test(X& r)
{
    X const a = r, b = r;

    test::check_return_type<bool>::equals(a == b);
    test::check_return_type<bool>::equals(a != b);
}

template <class X, class T>
void unordered_unique_test(X& r, T const& t)
{
    typedef BOOST_DEDUCED_TYPENAME X::iterator iterator;
    test::check_return_type<std::pair<iterator, bool> >::equals(r.insert(t));
    test::check_return_type<std::pair<iterator, bool> >::equals(r.emplace(t));
}

template <class X, class T>
void unordered_equivalent_test(X& r, T const& t)
{
    typedef BOOST_DEDUCED_TYPENAME X::iterator iterator;
    test::check_return_type<iterator>::equals(r.insert(t));
    test::check_return_type<iterator>::equals(r.emplace(t));
}

template <class X, class Key, class T>
void unordered_map_functions(X&, Key const& k, T const&)
{
    typedef BOOST_DEDUCED_TYPENAME X::mapped_type mapped_type;

    X a;
    test::check_return_type<mapped_type>::equals_ref(a[k]);
    test::check_return_type<mapped_type>::equals_ref(a.at(k));

    X const b = a;
    test::check_return_type<mapped_type const>::equals_ref(b.at(k));
}

template <class X, class Key, class T, class Hash, class Pred>
void unordered_test(X&, Key& k, T& t, Hash& hf, Pred& eq)
{
    typedef BOOST_DEDUCED_TYPENAME X::key_type key_type;
    typedef BOOST_DEDUCED_TYPENAME X::hasher hasher;
    typedef BOOST_DEDUCED_TYPENAME X::key_equal key_equal;
    typedef BOOST_DEDUCED_TYPENAME X::size_type size_type;

    typedef BOOST_DEDUCED_TYPENAME X::iterator iterator;
    typedef BOOST_DEDUCED_TYPENAME X::const_iterator const_iterator;
    typedef BOOST_DEDUCED_TYPENAME X::local_iterator local_iterator;
    typedef BOOST_DEDUCED_TYPENAME X::const_local_iterator const_local_iterator;

    typedef BOOST_DEDUCED_TYPENAME boost::BOOST_ITERATOR_CATEGORY<iterator>::type iterator_category;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<iterator>::type iterator_difference;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_pointer<iterator>::type iterator_pointer;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_reference<iterator>::type iterator_reference;

    typedef BOOST_DEDUCED_TYPENAME boost::BOOST_ITERATOR_CATEGORY<local_iterator>::type local_iterator_category;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<local_iterator>::type local_iterator_difference;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_pointer<local_iterator>::type local_iterator_pointer;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_reference<local_iterator>::type local_iterator_reference;

    typedef BOOST_DEDUCED_TYPENAME boost::BOOST_ITERATOR_CATEGORY<const_iterator>::type const_iterator_category;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<const_iterator>::type const_iterator_difference;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_pointer<const_iterator>::type const_iterator_pointer;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_reference<const_iterator>::type const_iterator_reference;

    typedef BOOST_DEDUCED_TYPENAME boost::BOOST_ITERATOR_CATEGORY<const_local_iterator>::type const_local_iterator_category;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_difference<const_local_iterator>::type const_local_iterator_difference;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_pointer<const_local_iterator>::type const_local_iterator_pointer;
    typedef BOOST_DEDUCED_TYPENAME boost::iterator_reference<const_local_iterator>::type const_local_iterator_reference;

    BOOST_MPL_ASSERT((boost::is_same<Key, key_type>));
    boost::function_requires<boost::CopyConstructibleConcept<key_type> >();
    boost::function_requires<boost::AssignableConcept<key_type> >();

    BOOST_MPL_ASSERT((boost::is_same<Hash, hasher>));
    test::check_return_type<std::size_t>::equals(hf(k));

    BOOST_MPL_ASSERT((boost::is_same<Pred, key_equal>));
    test::check_return_type<bool>::convertible(eq(k, k));

    boost::function_requires<boost::InputIteratorConcept<local_iterator> >();
    BOOST_MPL_ASSERT((boost::is_same<local_iterator_category, iterator_category>));
    BOOST_MPL_ASSERT((boost::is_same<local_iterator_difference, iterator_difference>));
    BOOST_MPL_ASSERT((boost::is_same<local_iterator_pointer, iterator_pointer>));
    BOOST_MPL_ASSERT((boost::is_same<local_iterator_reference, iterator_reference>));

    boost::function_requires<boost::InputIteratorConcept<const_local_iterator> >();
    BOOST_MPL_ASSERT((boost::is_same<const_local_iterator_category, const_iterator_category>));
    BOOST_MPL_ASSERT((boost::is_same<const_local_iterator_difference, const_iterator_difference>));
    BOOST_MPL_ASSERT((boost::is_same<const_local_iterator_pointer, const_iterator_pointer>));
    BOOST_MPL_ASSERT((boost::is_same<const_local_iterator_reference, const_iterator_reference>));

    X(10, hf, eq);
    X a(10, hf, eq);
    X(10, hf);
    X a2(10, hf);
    X(10);
    X a3(10);
    X();
    X a4;

    BOOST_DEDUCED_TYPENAME X::value_type* i = 0;
    BOOST_DEDUCED_TYPENAME X::value_type* j = 0;

    X(i, j, 10, hf, eq);
    X a5(i, j, 10, hf, eq);
    X(i, j, 10, hf);
    X a6(i, j, 10, hf);
    X(i, j, 10);
    X a7(i, j, 10);
    X(i, j);
    X a8(i, j);

    X const b;
    sink(X(b));
    X a9(b);
    a = b;

    test::check_return_type<hasher>::equals(b.hash_function());
    test::check_return_type<key_equal>::equals(b.key_eq());

    const_iterator q = a.cbegin();
    test::check_return_type<iterator>::equals(a.insert(q, t));
    test::check_return_type<iterator>::equals(a.emplace_hint(q, t));

    a.insert(i, j);
    test::check_return_type<size_type>::equals(a.erase(k));

    BOOST_CHECK(a.empty());
    if(a.empty()) {
        a.insert(t);
        q = a.cbegin();
        test::check_return_type<iterator>::equals(a.erase(q));
    }

    const_iterator q1 = a.cbegin(), q2 = a.cend();
    test::check_return_type<iterator>::equals(a.erase(q1, q2));

    a.clear();

    test::check_return_type<iterator>::equals(a.find(k));
    test::check_return_type<const_iterator>::equals(b.find(k));
    test::check_return_type<size_type>::equals(b.count(k));
    test::check_return_type<std::pair<iterator, iterator> >::equals(
            a.equal_range(k));
    test::check_return_type<std::pair<const_iterator, const_iterator> >::equals(
            b.equal_range(k));
    test::check_return_type<size_type>::equals(b.bucket_count());
    test::check_return_type<size_type>::equals(b.max_bucket_count());
    test::check_return_type<size_type>::equals(b.bucket(k));
    test::check_return_type<size_type>::equals(b.bucket_size(0));

    test::check_return_type<local_iterator>::equals(a.begin(0));
    test::check_return_type<const_local_iterator>::equals(b.begin(0));
    test::check_return_type<local_iterator>::equals(a.end(0));
    test::check_return_type<const_local_iterator>::equals(b.end(0));

    test::check_return_type<const_local_iterator>::equals(a.cbegin(0));
    test::check_return_type<const_local_iterator>::equals(b.cbegin(0));
    test::check_return_type<const_local_iterator>::equals(a.cend(0));
    test::check_return_type<const_local_iterator>::equals(b.cend(0));

    test::check_return_type<float>::equals(b.load_factor());
    test::check_return_type<float>::equals(b.max_load_factor());
    a.max_load_factor((float) 2.0);
    a.rehash(100);
}
