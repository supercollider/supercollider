
// Copyright 2008-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or move at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"
#include "../objects/test.hpp"
#include "../helpers/random_values.hpp"
#include "../helpers/tracker.hpp"
#include "../helpers/equivalent.hpp"
#include "../helpers/invariants.hpp"

namespace move_tests
{
    test::seed_t seed(98624);

    template<class T>
    T empty(T*) {
        return T();
    }

    template<class T>
    T create(test::random_values<T> const& v,
            test::object_count& count) {
        T x(v.begin(), v.end());
        count = test::global_object_count;
        return x;
    }

    template<class T>
    T create(test::random_values<T> const& v,
            test::object_count& count,
            BOOST_DEDUCED_TYPENAME T::hasher hf,
            BOOST_DEDUCED_TYPENAME T::key_equal eq,
            BOOST_DEDUCED_TYPENAME T::allocator_type al,
            float mlf) {
        T x(0, hf, eq, al);
        x.max_load_factor(mlf);
        x.insert(v.begin(), v.end());
        count = test::global_object_count;
        return x;
    }

    template <class T>
    void move_construct_tests1(T* ptr, test::random_generator const& generator = test::default_generator)
    {
        BOOST_DEDUCED_TYPENAME T::hasher hf;
        BOOST_DEDUCED_TYPENAME T::key_equal eq;
        BOOST_DEDUCED_TYPENAME T::allocator_type al;

        {
            T y(empty(ptr));
            BOOST_CHECK(y.empty());
            BOOST_CHECK(test::equivalent(y.hash_function(), hf));
            BOOST_CHECK(test::equivalent(y.key_eq(), eq));
            BOOST_CHECK(test::equivalent(y.get_allocator(), al));
            BOOST_CHECK(y.max_load_factor() == 1.0);
            test::check_equivalent_keys(y);
        }

        {
            test::random_values<T> v(1000, generator);
            test::object_count count;
            T y(create(v, count));
            BOOST_CHECK(count == test::global_object_count);
            test::check_container(y, v);
            test::check_equivalent_keys(y);
        }
    }

    template <class T>
    void move_assign_tests1(T*, test::random_generator const& generator = test::default_generator)
    {
        {
            test::random_values<T> v(500, generator);
            test::object_count count;
            T y;
            y = create(v, count);
            BOOST_CHECK(count == test::global_object_count);
            test::check_container(y, v);
            test::check_equivalent_keys(y);
        }
    }

    template <class T>
    void move_construct_tests2(T*,
            test::random_generator const& generator = test::default_generator)
    {
        BOOST_DEDUCED_TYPENAME T::hasher hf(1);
        BOOST_DEDUCED_TYPENAME T::key_equal eq(1);
        BOOST_DEDUCED_TYPENAME T::allocator_type al(1);
        BOOST_DEDUCED_TYPENAME T::allocator_type al2(2);

        test::object_count count;

        {
            test::random_values<T> v(500, generator);
            T y(create(v, count, hf, eq, al, 0.5));
            BOOST_CHECK(count == test::global_object_count);
            test::check_container(y, v);
            BOOST_CHECK(test::equivalent(y.hash_function(), hf));
            BOOST_CHECK(test::equivalent(y.key_eq(), eq));
            BOOST_CHECK(test::equivalent(y.get_allocator(), al));
            BOOST_CHECK(y.max_load_factor() == 0.5); // Not necessarily required.
            test::check_equivalent_keys(y);
        }

        {
            // TODO: To do this correctly requires the fancy new allocator stuff.
            test::random_values<T> v(500, generator);
            T y(create(v, count, hf, eq, al, 2.0), al2);
            BOOST_CHECK(count != test::global_object_count);
            test::check_container(y, v);
            BOOST_CHECK(test::equivalent(y.hash_function(), hf));
            BOOST_CHECK(test::equivalent(y.key_eq(), eq));
            BOOST_CHECK(test::equivalent(y.get_allocator(), al2));
            BOOST_CHECK(y.max_load_factor() == 2.0); // Not necessarily required.
            test::check_equivalent_keys(y);
        }

        {
            test::random_values<T> v(25, generator);
            T y(create(v, count, hf, eq, al, 1.0), al);
#if defined(BOOST_HAS_RVALUE_REFS)
            BOOST_CHECK(count == test::global_object_count);
#else
            BOOST_CHECK(test::global_object_count.constructions - count.constructions <=
                (test::is_map<T>::value ? 50 : 25));
            BOOST_CHECK(count.instances == test::global_object_count.instances);
#endif
            test::check_container(y, v);
            BOOST_CHECK(test::equivalent(y.hash_function(), hf));
            BOOST_CHECK(test::equivalent(y.key_eq(), eq));
            BOOST_CHECK(test::equivalent(y.get_allocator(), al));
            BOOST_CHECK(y.max_load_factor() == 1.0); // Not necessarily required.
            test::check_equivalent_keys(y);
        }
    }

    boost::unordered_set<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_set;
    boost::unordered_multiset<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multiset;
    boost::unordered_map<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_map;
    boost::unordered_multimap<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multimap;

    using test::default_generator;
    using test::generate_collisions;

    UNORDERED_TEST(move_construct_tests1,
        ((test_set)(test_multiset)(test_map)(test_multimap))
    )
    UNORDERED_TEST(move_assign_tests1,
        ((test_set)(test_multiset)(test_map)(test_multimap))
    )
    UNORDERED_TEST(move_construct_tests2,
        ((test_set)(test_multiset)(test_map)(test_multimap))
        ((default_generator)(generate_collisions))
    )
}

RUN_TESTS()
