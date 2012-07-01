//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_WINDOWS_INTERMODULE_SINGLETON_HPP
#define BOOST_INTERPROCESS_WINDOWS_INTERMODULE_SINGLETON_HPP

#if defined(_MSC_VER)&&(_MSC_VER>=1200)
#pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#if !defined(BOOST_INTERPROCESS_WINDOWS)
   #error "This header can't be included from non-windows operating systems"
#endif

#include <boost/interprocess/detail/managed_global_memory.hpp>
#include <boost/interprocess/detail/intermodule_singleton_common.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/assert.hpp>

namespace boost{
namespace interprocess{
namespace ipcdetail{

typedef basic_managed_global_memory<windows_shared_memory, false>  windows_managed_global_memory;

namespace intermodule_singleton_helpers {

template<>
struct managed_sh_dependant<windows_managed_global_memory>
{
   static void apply_gmem_erase_logic(const char *, const char *){}

   static bool remove_old_gmem()
   { return true; }

   struct lock_file_logic
   {
      lock_file_logic(windows_managed_global_memory &)
         : retry_with_new_shm(false)
      {}

      void operator()(void){}
      const bool retry_with_new_shm;
   };

   struct unlink_shmlogic
   {
      unlink_shmlogic(windows_managed_global_memory &)
      {}
      void operator()(){}
   };
};

}  //namespace intermodule_singleton_helpers {

template<typename C, bool LazyInit = true, bool Phoenix = true>
class windows_intermodule_singleton
   : public intermodule_singleton_impl
      < C
      , LazyInit
      , Phoenix
      , windows_managed_global_memory
      >
{};

}  //namespace ipcdetail{
}  //namespace interprocess{
}  //namespace boost{

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_WINDOWS_INTERMODULE_SINGLETON_HPP
