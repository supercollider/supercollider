// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2014 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2014 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2014 Mateusz Loskot, London, UK.
// Copyright (c) 2014 Adam Wulkiewicz, Lodz, Poland.

// This file was modified by Oracle on 2014.
// Modifications copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_GEOMETRIES_POINT_HPP
#define BOOST_GEOMETRY_GEOMETRIES_POINT_HPP

#include <cstddef>

#include <boost/mpl/int.hpp>
#include <boost/static_assert.hpp>

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>

namespace boost { namespace geometry
{

// Silence warning C4127: conditional expression is constant
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif


namespace model
{

namespace detail
{

template <std::size_t DimensionCount, std::size_t Index>
struct array_assign
{
    template <typename T>
    static inline void apply(T values[], T const& value)
    {
        values[Index] = value;
    }
};

// Specialization avoiding assigning element [2] for only 2 dimensions
template <> struct array_assign<2, 2>
{
    template <typename T> static inline void apply(T [], T const& ) {}
};

// Specialization avoiding assigning elements for (rarely used) points in 1 dim
template <> struct array_assign<1, 1>
{
    template <typename T> static inline void apply(T [], T const& ) {}
};

template <> struct array_assign<1, 2>
{
    template <typename T> static inline void apply(T [], T const& ) {}
};

}
/*!
\brief Basic point class, having coordinates defined in a neutral way
\details Defines a neutral point class, fulfilling the Point Concept.
    Library users can use this point class, or use their own point classes.
    This point class is used in most of the samples and tests of Boost.Geometry
    This point class is used occasionally within the library, where a temporary
    point class is necessary.
\ingroup geometries
\tparam CoordinateType \tparam_numeric
\tparam DimensionCount number of coordinates, usually 2 or 3
\tparam CoordinateSystem coordinate system, for example cs::cartesian

\qbk{[include reference/geometries/point.qbk]}
\qbk{before.synopsis, [heading Model of]}
\qbk{before.synopsis, [link geometry.reference.concepts.concept_point Point Concept]}


*/
template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem
>
class point
{
private:
    // The following enum is used to fully instantiate the
    // CoordinateSystem class and check the correctness of the units
    // passed for non-Cartesian coordinate systems.
    enum { cs_check = sizeof(CoordinateSystem) };

public:

    /// @brief Default constructor, no initialization
    inline point()
    {
        BOOST_STATIC_ASSERT(DimensionCount >= 1);
    }

    /// @brief Constructor to set one value
    explicit inline point(CoordinateType const& v0)
    {
        detail::array_assign<DimensionCount, 0>::apply(m_values, v0);
        detail::array_assign<DimensionCount, 1>::apply(m_values, CoordinateType());
        detail::array_assign<DimensionCount, 2>::apply(m_values, CoordinateType());
    }

    /// @brief Constructor to set two values
    inline point(CoordinateType const& v0, CoordinateType const& v1)
    {
        detail::array_assign<DimensionCount, 0>::apply(m_values, v0);
        detail::array_assign<DimensionCount, 1>::apply(m_values, v1);
        detail::array_assign<DimensionCount, 2>::apply(m_values, CoordinateType());
    }

    /// @brief Constructor to set three values
    inline point(CoordinateType const& v0, CoordinateType const& v1,
            CoordinateType const& v2)
    {
        detail::array_assign<DimensionCount, 0>::apply(m_values, v0);
        detail::array_assign<DimensionCount, 1>::apply(m_values, v1);
        detail::array_assign<DimensionCount, 2>::apply(m_values, v2);
    }

    /// @brief Get a coordinate
    /// @tparam K coordinate to get
    /// @return the coordinate
    template <std::size_t K>
    inline CoordinateType const& get() const
    {
        BOOST_STATIC_ASSERT(K < DimensionCount);
        return m_values[K];
    }

    /// @brief Set a coordinate
    /// @tparam K coordinate to set
    /// @param value value to set
    template <std::size_t K>
    inline void set(CoordinateType const& value)
    {
        BOOST_STATIC_ASSERT(K < DimensionCount);
        m_values[K] = value;
    }

private:

    CoordinateType m_values[DimensionCount];
};


} // namespace model

// Adapt the point to the concept
#ifndef DOXYGEN_NO_TRAITS_SPECIALIZATIONS
namespace traits
{
template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem
>
struct tag<model::point<CoordinateType, DimensionCount, CoordinateSystem> >
{
    typedef point_tag type;
};

template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem
>
struct coordinate_type<model::point<CoordinateType, DimensionCount, CoordinateSystem> >
{
    typedef CoordinateType type;
};

template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem
>
struct coordinate_system<model::point<CoordinateType, DimensionCount, CoordinateSystem> >
{
    typedef CoordinateSystem type;
};

template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem
>
struct dimension<model::point<CoordinateType, DimensionCount, CoordinateSystem> >
    : boost::mpl::int_<DimensionCount>
{};

template
<
    typename CoordinateType,
    std::size_t DimensionCount,
    typename CoordinateSystem,
    std::size_t Dimension
>
struct access<model::point<CoordinateType, DimensionCount, CoordinateSystem>, Dimension>
{
    static inline CoordinateType get(
        model::point<CoordinateType, DimensionCount, CoordinateSystem> const& p)
    {
        return p.template get<Dimension>();
    }

    static inline void set(
        model::point<CoordinateType, DimensionCount, CoordinateSystem>& p,
        CoordinateType const& value)
    {
        p.template set<Dimension>(value);
    }
};

} // namespace traits
#endif // DOXYGEN_NO_TRAITS_SPECIALIZATIONS

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_GEOMETRIES_POINT_HPP
