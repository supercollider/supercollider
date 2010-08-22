//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/move/move.hpp>
#include <utility>

//[clone_ptr_base_derived
class Base
{
   public:
   // Compiler-generated copy constructor and assignment 

   Base(){}

   // move semantics
   BOOST_ENABLE_MOVE_EMULATION(Base)

   Base(BOOST_RV_REF(Base) x) {/**/}      // Move ctor

   Base& operator=(BOOST_RV_REF(Base) x)
   {/**/ return *this;}                   // Move assign
   
   virtual Base *clone() const
   {  return new Base(*this);  }
};

class Member
{
   public:
   Member(){}

   // Compiler-generated copy constructor and assignment 

   // move semantics
   BOOST_ENABLE_MOVE_EMULATION(Member)

   Member(BOOST_RV_REF(Member))  {/**/}      // Move ctor

   Member &operator=(BOOST_RV_REF(Member))   // Move assign
   {/**/ return *this;  }
};

class Derived : public Base
{
   Member mem_;

   public:
   // Compiler-generated copy constructor and assignment 

   Derived(){}

   // move semantics
   BOOST_ENABLE_MOVE_EMULATION(Derived)

   Derived(BOOST_RV_REF(Derived) x)             // Move ctor
      : Base(boost::move(static_cast<Base&>(x))), 
        mem_(boost::move(x.mem_)) { }

   Derived& operator=(BOOST_RV_REF(Derived) x)  // Move assign
   {
      Base::operator=(boost::move(static_cast<Base&>(x)));
      mem_  = boost::move(x.mem_);
      return *this;
   }
   // ...
};
//]

//[clone_ptr_def
template <class T>
class clone_ptr
{
   //<-
   private:
   T* ptr;

   public:
   // construction
   explicit clone_ptr(T* p = 0) : ptr(p) {}

   // destruction
   ~clone_ptr() { delete ptr; }

   // copy semantics
   clone_ptr(const clone_ptr& p)
      : ptr(p.ptr ? p.ptr->clone() : 0) {}

   clone_ptr& operator=(const clone_ptr& p)
   {
      if (this != &p)
      {
         T *p = p.ptr ? p.ptr->clone() : 0;
         delete ptr;
         ptr = p;
      }
      return *this;
   }

   //->
   // Same as before ...

   // move semantics
   BOOST_ENABLE_MOVE_EMULATION(clone_ptr)
   
   clone_ptr(BOOST_RV_REF(clone_ptr) p)
      : ptr(p.ptr) { p.ptr = 0; }

   clone_ptr& operator=(BOOST_RV_REF(clone_ptr) p)
   {
      std::swap(ptr, p.ptr);
      delete p.ptr;
      p.ptr = 0;
      return *this;
   }
};
//]

int main()
{
   {
   //[copy_clone_ptr
   clone_ptr<Base> p1(new Derived());
   // ...
   clone_ptr<Base> p2 = p1;  // p2 and p1 each own their own pointer
   //]
   }
   {
   //[move_clone_ptr
   clone_ptr<Base> p1(new Derived());
   // ...
   clone_ptr<Base> p2 = boost::move(p1);  // p2 now owns the pointer instead of p1
   }
   //]
   //[clone_ptr_move_derived
   Derived d;
   Derived d2(boost::move(d));
   d2 = boost::move(d);
   //]
   return 0;
}
