/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://boost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_ALIGNOF_HELPER_HPP
#define BOOST_ALIGN_DETAIL_ALIGNOF_HELPER_HPP

namespace boost {
namespace alignment {
namespace detail {

template<class T>
struct alignof_helper {
    char value;
    T object;
};

} /* .detail */
} /* .alignment */
} /* .boost */

#endif
