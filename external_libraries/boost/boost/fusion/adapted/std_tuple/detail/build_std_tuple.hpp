/*=============================================================================
    Copyright (c) 2014 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_BUILD_STD_TUPLE_05292014_0100)
#define BOOST_FUSION_BUILD_STD_TUPLE_05292014_0100

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/support/config.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/value_of.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <tuple>

namespace boost { namespace fusion { namespace detail
{
    template <typename First, typename Last
      , bool is_empty = result_of::equal_to<First, Last>::value
    >
    struct build_std_tuple;

    template <typename First, typename Last>
    struct build_std_tuple<First, Last, true>
    {
        typedef std::tuple<> type;
        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        static type
        call(First const&, Last const&)
        {
            return type();
        }
    };

    template <int ...> struct indexed_tuple { };

    template <int, typename = indexed_tuple<>>
    struct make_indexed_tuple;

    template <int Head, int ...Tail>
    struct make_indexed_tuple<Head, indexed_tuple<Tail...>>
    {
        typedef typename
            boost::mpl::eval_if_c<
                (Head == 0),
                boost::mpl::identity<indexed_tuple<Tail...>>,
                make_indexed_tuple<Head - 1, indexed_tuple<Head - 1, Tail...>>
            >::type
        type;
    };

    template <typename T, typename Rest>
    struct push_front_std_tuple;

    template <typename T, typename ...Rest>
    struct push_front_std_tuple<T, std::tuple<Rest...>>
    {
        typedef std::tuple<T, Rest...> type;

        template <int ...I>
        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        static type
        indexed_call(T const& first, std::tuple<Rest...> const& rest, indexed_tuple<I...>)
        {
            return type(first, std::get<I>(rest)...);
        }

        BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        static type
        call(T const& first, std::tuple<Rest...> const& rest)
        {
            typedef typename make_indexed_tuple<sizeof...(Rest)>::type gen;
            return indexed_call(first, rest, gen());
        }
    };

    template <typename First, typename Last>
    struct build_std_tuple<First, Last, false>
    {
        typedef
            build_std_tuple<typename result_of::next<First>::type, Last>
        next_build_std_tuple;

        typedef push_front_std_tuple<
            typename result_of::value_of<First>::type
          , typename next_build_std_tuple::type>
        push_front;

        typedef typename push_front::type type;

        BOOST_CXX14_CONSTEXPR BOOST_FUSION_GPU_ENABLED
        static type
        call(First const& f, Last const& l)
        {
            typename result_of::value_of<First>::type v = *f;
            return push_front::call(
                v, next_build_std_tuple::call(fusion::next(f), l));
        }
    };
}}}

#endif
