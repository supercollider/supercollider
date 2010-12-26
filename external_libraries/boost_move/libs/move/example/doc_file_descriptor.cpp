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

//[file_descriptor_def

#include <boost/move/move.hpp>
#include <stdexcept>

class file_descriptor
{
   //<-
   int operating_system_open_file(const char *)
   {
      return 1;
   }

   void operating_system_close_file(int)
   {}
   //->
   int os_descr_;

   file_descriptor(file_descriptor &);
   file_descriptor & operator=(file_descriptor &);

   public:
   explicit file_descriptor(const char *filename = 0)
      : os_descr_(filename ? operating_system_open_file(filename) : 0)
   {  if(!os_descr_) throw std::runtime_error("file not found");  }

   ~file_descriptor()
   {  if(!os_descr_)  operating_system_close_file(os_descr_);  }

   // move semantics
   BOOST_ENABLE_MOVE_EMULATION(file_descriptor)

   file_descriptor(BOOST_RV_REF(file_descriptor) x)            // Move ctor
      :  os_descr_(x.os_descr_)
   {  x.os_descr_ = 0;  }      

   file_descriptor& operator=(BOOST_RV_REF(file_descriptor) x) // Move assign
   {
      if(!os_descr_) operating_system_close_file(os_descr_);
      os_descr_   = x.os_descr_;
      x.os_descr_ = 0;
      return *this;
   }

   bool empty() const   {  return os_descr_ == 0;  }
};

//]

//[file_descriptor_example
#include <boost/container/vector.hpp>
#include <cassert>

file_descriptor create_file_descriptor(const char *filename)
{  return file_descriptor(filename);  }

int main()
{
   //Open a file obtaining its descriptor. The file_descriptor
   //can be returned from factory functions without any copy
   file_descriptor fd = create_file_descriptor("filename");
   assert(!fd.empty());

   //Move into a vector
   boost::container::vector<file_descriptor> v;
   v.push_back(boost::move(fd));

   //Check ownership has been transferred
   assert(fd.empty());
   assert(!v[0].empty());

   //Compilation error if uncommented, file_descriptor is not copyable:
   //boost::container::vector<file_descriptor> v2(v);
   return 0;
}
//]
