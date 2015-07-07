// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ITERATORS_DETAIL_SEGMENT_ITERATOR_VALUE_TYPE_HPP
#define BOOST_GEOMETRY_ITERATORS_DETAIL_SEGMENT_ITERATOR_VALUE_TYPE_HPP

#include <boost/geometry/iterators/detail/point_iterator/value_type.hpp>
#include <boost/geometry/geometries/pointing_segment.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace segment_iterator
{

template <typename Geometry>
struct value_type
{
    typedef typename detail::point_iterator::value_type
        <
            Geometry
        >::type point_iterator_value_type;

    typedef geometry::model::pointing_segment
        <
            point_iterator_value_type
        > type;
};

}} // namespace detail::segment_iterator
#endif // DOXYGEN_NO_DETAIL

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ITERATORS_DETAIL_SEGMENT_ITERATOR_VALUE_TYPE_HPP
