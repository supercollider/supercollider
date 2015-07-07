/*
 (c) 2014 Glen Joseph Fernandes
 glenjofe at gmail dot com

 Distributed under the Boost Software
 License, Version 1.0.
 http://boost.org/LICENSE_1_0.txt
*/
#ifndef MAKE_ALIGNED_HPP
#define MAKE_ALIGNED_HPP

#include <boost/align/aligned_alloc.hpp>
#include "aligned_ptr.hpp"

template<class T, class... Args>
inline aligned_ptr<T> make_aligned(Args&&... args)
{
    auto p = boost::alignment::
        aligned_alloc(alignof(T), sizeof(T));
    if (!p) {
        throw std::bad_alloc();
    }
    try {
        auto q = ::new(p) T(std::forward<Args>(args)...);
        return aligned_ptr<T>(q);
    } catch (...) {
        boost::alignment::aligned_free(p);
        throw;
    }
}

#endif
