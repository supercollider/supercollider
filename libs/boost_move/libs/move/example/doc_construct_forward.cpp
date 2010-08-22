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

//[construct_forward_example
#include <boost/move/move.hpp>
#include <iostream>

class non_movable_test
{
   public:
   non_movable_test()
   {  std::cout << "non_movable_test()" << std::endl;   }

   non_movable_test(const non_movable_test&)
   {  std::cout << "non_movable_test(const non_movable_test&)" << std::endl;   }

   non_movable_test(int)
   {  std::cout << "non_movable_test(int)" << std::endl;   }

   non_movable_test(int, double)
   {  std::cout << "non_movable_test(int, double)" << std::endl;   }
};

class movable_test
{
   public:
   BOOST_ENABLE_MOVE_EMULATION(movable_test)

   movable_test()
   {  std::cout << "movable_test()" << std::endl;   }

   movable_test(int)
   {  std::cout << "movable_test(int)" << std::endl;   }

   movable_test(BOOST_RV_REF(movable_test))
   {  std::cout << "movable_test(BOOST_RV_REF(movable_test))" << std::endl;   }

   movable_test(const movable_test &)
   {  std::cout << "movable_test(const movable_test &)" << std::endl;   }

   movable_test(BOOST_RV_REF(movable_test), BOOST_RV_REF(movable_test))
   {  std::cout << "movable_test(BOOST_RV_REF(movable_test), BOOST_RV_REF(movable_test))" << std::endl;   }
};

//1 argument
template<class MaybeMovable, class MaybeRv>
void function_construct(BOOST_FWD_REF(MaybeRv) x)
{  MaybeMovable m(boost::forward<MaybeRv>(x));   }

//2 argument
template<class MaybeMovable, class MaybeRv, class MaybeRv2>
void function_construct(BOOST_FWD_REF(MaybeRv) x, BOOST_FWD_REF(MaybeRv2) x2)
{
   MaybeMovable m(boost::forward<MaybeRv>(x), boost::forward<MaybeRv2>(x2));
}

int main()
{
   movable_test m;
   //move constructor
   function_construct<movable_test>(boost::move(m));
   //copy constructor
   function_construct<movable_test>(movable_test());
   //two rvalue constructor
   function_construct<movable_test>(boost::move(m), boost::move(m));

   non_movable_test nm;
   //copy constructor (non_movable_test has no move ctor.)
   function_construct<non_movable_test>(boost::move(nm));
   //copy constructor
   function_construct<non_movable_test>(nm);
   //int constructor
   function_construct<non_movable_test>(int(0));
   //int, double constructor
   function_construct<non_movable_test>(int(0), double(0.0));

   //Output is:
   //movable_test()
   //movable_test(BOOST_RV_REF(movable_test))
   //movable_test()
   //movable_test(const movable_test &)
   //movable_test(BOOST_RV_REF(movable_test), BOOST_RV_REF(movable_test))
   //non_movable_test()
   //non_movable_test(const non_movable_test&)
   //non_movable_test(const non_movable_test&)
   //non_movable_test(int)
   //non_movable_test(int, double)
   return 0;
}
//]
