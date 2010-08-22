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
#include <boost/container/vector.hpp>
#include <boost/container/list.hpp>
#include <boost/container/stable_vector.hpp>
#include "../example/movable.hpp"

template<class Container>
int move_test()
{
   //Default construct 10 movable objects
   Container v(10);

   //Test default constructed value
   if(v.begin()->moved()){
      return 1;
   }

   //Move values
   Container v2;
   std::copy(v.begin(), v.end(), boost::back_move_inserter(v2));

   //Test values have been moved
   if(!v.begin()->moved()){
      return 1;
   }

   if(v2.size() != 10){
      return 1;
   }

   if(v2.begin()->moved()){
      return 1;
   }
   return 0;
}

int main()
{
   namespace bc = ::boost::container;

   if(move_test< bc::vector<movable> >()){
      return 1;
   }
   if(move_test< bc::list<movable> >()){
      return 1;
   }
   if(move_test< bc::stable_vector<movable> >()){
      return 1;
   }
   return 0;
}
