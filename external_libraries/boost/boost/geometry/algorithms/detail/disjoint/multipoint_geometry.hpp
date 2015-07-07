// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISJOINT_MULTIPOINT_GEOMETRY_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISJOINT_MULTIPOINT_GEOMETRY_HPP

#include <algorithm>
#include <vector>

#include <boost/assert.hpp>
#include <boost/range.hpp>

#include <boost/geometry/algorithms/detail/check_iterator_range.hpp>
#include <boost/geometry/algorithms/detail/disjoint/multirange_geometry.hpp>
#include <boost/geometry/algorithms/detail/disjoint/point_point.hpp>
#include <boost/geometry/algorithms/detail/disjoint/point_geometry.hpp>
#include <boost/geometry/algorithms/detail/relate/less.hpp>

#include <boost/geometry/algorithms/dispatch/disjoint.hpp>


namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace disjoint
{

template<typename MultiPoint1, typename MultiPoint2>
class multipoint_multipoint
{
private:
    template <typename Iterator>
    class unary_disjoint_predicate
        : detail::relate::less
    {
    private:
        typedef detail::relate::less base_type;

    public:
        unary_disjoint_predicate(Iterator first, Iterator last)
            : base_type(), m_first(first), m_last(last)
        {}

        template <typename Point>
        inline bool apply(Point const& point) const
        {
            return !std::binary_search(m_first,
                                       m_last,
                                       point,
                                       static_cast<base_type const&>(*this));
        }

    private:
        Iterator m_first, m_last;
    };

public:
    static inline bool apply(MultiPoint1 const& multipoint1,
                             MultiPoint2 const& multipoint2)
    {
        BOOST_ASSERT( boost::size(multipoint1) <= boost::size(multipoint2) );

        typedef typename boost::range_value<MultiPoint1>::type point1_type;

        std::vector<point1_type> points1(boost::begin(multipoint1),
                                         boost::end(multipoint1));

        std::sort(points1.begin(), points1.end(), detail::relate::less());

        typedef unary_disjoint_predicate
            <
                typename std::vector<point1_type>::const_iterator
            > predicate_type;

        return check_iterator_range
            <
                predicate_type
            >::apply(boost::begin(multipoint2),
                     boost::end(multipoint2),
                     predicate_type(points1.begin(), points1.end()));
    }
};


}} // namespace detail::disjoint
#endif // DOXYGEN_NO_DETAIL




#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template<typename Point, typename MultiPoint, std::size_t DimensionCount>
struct disjoint
    <
        Point, MultiPoint, DimensionCount, point_tag, multi_point_tag, false
    > : detail::disjoint::multirange_constant_size_geometry<MultiPoint, Point>
{};


template<typename MultiPoint, typename Segment, std::size_t DimensionCount>
struct disjoint
    <
        MultiPoint, Segment, DimensionCount, multi_point_tag, segment_tag, false
    > : detail::disjoint::multirange_constant_size_geometry<MultiPoint, Segment>
{};


template<typename MultiPoint, typename Box, std::size_t DimensionCount>
struct disjoint
    <
        MultiPoint, Box, DimensionCount, multi_point_tag, box_tag, false
    > : detail::disjoint::multirange_constant_size_geometry<MultiPoint, Box>
{};


template<typename MultiPoint1, typename MultiPoint2, std::size_t DimensionCount>
struct disjoint
    <
        MultiPoint1, MultiPoint2, DimensionCount,
        multi_point_tag, multi_point_tag, false
    >
{
    static inline bool apply(MultiPoint1 const& multipoint1,
                             MultiPoint2 const& multipoint2)
    {
        if ( boost::size(multipoint2) < boost::size(multipoint1) )
        {
            return detail::disjoint::multipoint_multipoint
                <
                    MultiPoint2, MultiPoint1
                >::apply(multipoint2, multipoint1);
        } 

        return detail::disjoint::multipoint_multipoint
            <
                MultiPoint1, MultiPoint2
            >::apply(multipoint1, multipoint2);
   }
};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


}} // namespace boost::geometry



#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISJOINT_MULTIPOINT_GEOMETRY_HPP
