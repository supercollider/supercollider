//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright David Abrahams, Vicente Botet, Ion Gaztanaga 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/move/move.hpp>
#include "../example/movable.hpp"
#include <cstdio>

class non_movable
{
   public:
   non_movable()
   {}
};

template<class MaybeMovableOnly, class MaybeRvalue>
void function_construct(BOOST_FWD_REF(MaybeRvalue) x)
{
   //Moves in case Convertible is boost::rv<movable> copies otherwise
   //For C++0x perfect forwarding
   MaybeMovableOnly m(boost::forward<MaybeRvalue>(x));
}

int main()
{
   movable m;
   function_construct<movable>(boost::move(m));
   non_movable nm;
   function_construct<non_movable>(boost::move(nm));
   const non_movable cnm;
   function_construct<non_movable>(cnm);
   return 0;
}
