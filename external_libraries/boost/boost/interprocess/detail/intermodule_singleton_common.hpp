//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_INTERMODULE_SINGLETON_COMMON_HPP
#define BOOST_INTERPROCESS_INTERMODULE_SINGLETON_COMMON_HPP

#if defined(_MSC_VER)&&(_MSC_VER>=1200)
#pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <boost/interprocess/detail/atomic.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include <boost/assert.hpp>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>

namespace boost{
namespace interprocess{
namespace ipcdetail{

namespace intermodule_singleton_helpers {

inline void get_pid_creation_time_str(std::string &s)
{
   std::stringstream stream;
   stream << get_current_process_id() << '_';
   stream.precision(6);
   stream << std::fixed << get_current_process_creation_time();
   s = stream.str();
}

inline const char *get_shm_base_name()
{  return "bip.gmem.shm.";  }

inline void get_shm_name(std::string &shm_name)
{
   get_pid_creation_time_str(shm_name);
   shm_name.insert(0, get_shm_base_name());
}

inline std::size_t get_shm_size()
{  return 65536;  }

template<class ManagedGlobalMemory>
struct managed_sh_dependant;

}  //namespace intermodule_singleton_helpers {

//This class contains common code for all singleton types, so that we instantiate this
//code just once per module. This class also holds a shared memory manager
//to be used by all instances protected with a reference count
template<class ManagedGlobalMemory>
class intermodule_singleton_common
{
   public:
   typedef void*(singleton_constructor_t)(ManagedGlobalMemory &);
   typedef void (singleton_destructor_t)(void *, ManagedGlobalMemory &);

   static const ::boost::uint32_t Uninitialized       = 0u;
   static const ::boost::uint32_t Initializing        = 1u;
   static const ::boost::uint32_t Initialized         = 2u;
   static const ::boost::uint32_t Broken              = 3u;
   static const ::boost::uint32_t Destroyed           = 4u;

   //Initialize this_module_singleton_ptr, creates the shared memory if needed and also creates an unique
   //opaque type in shared memory through a singleton_constructor_t function call,
   //initializing the passed pointer to that unique instance.
   //
   //We have two concurrency types here. a)the shared memory/singleton creation must
   //be safe between threads of this process but in different modules/dlls. b)
   //the pointer to the singleton is per-module, so we have to protect this
   //initization between threads of the same module.
   //
   //All static variables declared here are shared between inside a module
   //so atomic operations will synchronize only threads of the same module.
   static void initialize_singleton_logic
      (void *&ptr, volatile boost::uint32_t &this_module_singleton_initialized, singleton_constructor_t constructor, bool phoenix)
   {
      //If current module is not initialized enter to lock free logic
      if(atomic_read32(&this_module_singleton_initialized) != Initialized){
         //Now a single thread of the module will succeed in this CAS.
         //trying to pass from Uninitialized to Initializing
         ::boost::uint32_t previous_module_singleton_initialized = atomic_cas32
            (&this_module_singleton_initialized, Initializing, Uninitialized);
         //If the thread succeeded the CAS (winner) it will compete with other
         //winner threads from other modules to create the shared memory
         if(previous_module_singleton_initialized == Destroyed){
            //Trying to resurrect a dead Phoenix singleton. Just try to
            //mark it as uninitialized and start again
            if(phoenix){
               atomic_cas32(&this_module_singleton_initialized, Uninitialized, Destroyed);
               previous_module_singleton_initialized = atomic_cas32
                  (&this_module_singleton_initialized, Initializing, Uninitialized);
            }
            //Trying to resurrect a non-Phoenix dead singleton is an error
            else{
               throw interprocess_exception("Boost.Interprocess: Dead reference on non-Phoenix singleton of type");
            }
         }
         if(previous_module_singleton_initialized == Uninitialized){
            try{
               //Now initialize shm, this function solves concurrency issues
               //between threads of several modules
               initialize_shm();
               //Increment the module reference count that reflects how many
               //singletons this module holds, so that we can safely destroy
               //module shared memory object when no singleton is left
               atomic_inc32(&this_module_singleton_count);
               //Now try to create the singleton in shared memory.
               //This function solves concurrency issues
               //between threads of several modules
               void *tmp = constructor(get_shm());
               //Insert a barrier before assigning the pointer to
               //make sure this assignment comes after the initialization
               atomic_write32(&this_module_singleton_initialized, Initializing);
               //Assign the singleton address to the module-local pointer
               ptr = tmp;
               //Memory barrier inserted, all previous operations should complete
               //before this one. Now marked as initialized
               atomic_write32(&this_module_singleton_initialized, Initialized);
            }
            catch(...){
               //Mark singleton failed to initialize
               atomic_write32(&this_module_singleton_initialized, Broken);
               throw;
            }
         }
         //If previous state was initializing, this means that another winner thread is
         //trying to initialize the singleton. Just wait until completes its work.
         else if(previous_module_singleton_initialized == Initializing){
            while(1){
               previous_module_singleton_initialized = atomic_read32(&this_module_singleton_initialized);
               if(previous_module_singleton_initialized >= Initialized){
                  //Already initialized, or exception thrown by initializer thread
                  break;
               }
               else if(previous_module_singleton_initialized == Initializing){
                  thread_yield();
               }
               else{
                  //This can't be happening!
                  BOOST_ASSERT(0);
               }
            }
         }
         else if(previous_module_singleton_initialized == Initialized){
            //Nothing to do here, the singleton is ready
         }
         //If previous state was greater than initialized, then memory is broken
         //trying to initialize the singleton.
         else{//(previous_module_singleton_initialized > Initialized)
            throw interprocess_exception("boost::interprocess::intermodule_singleton initialization failed");
         }
      }
      BOOST_ASSERT(ptr != 0);
   }

   static void finalize_singleton_logic(void *&ptr, volatile boost::uint32_t &this_module_singleton_initialized, singleton_destructor_t destructor)
   {
      //Protect destruction against lazy singletons not initialized in this execution
      if(ptr){
         //Note: this destructor might provoke a Phoenix singleton
         //resurrection. This means that this_module_singleton_count
         //might change after this call.
         destructor(ptr, get_shm());
         ptr = 0;

         //Memory barrier to make sure pointer is nulled.
         //Mark this singleton as destroyed.
         atomic_write32(&this_module_singleton_initialized, Destroyed);

         //If this is the last singleton of this module
         //apply shm destruction.
         //Note: singletons are destroyed when the module is unloaded
         //so no threads should be executing or holding references
         //to this module
         if(1 == atomic_dec32(&this_module_singleton_count)){
            destroy_shm();
         }
      }
   }

   private:
   static ManagedGlobalMemory &get_shm()
   {
      return *static_cast<ManagedGlobalMemory *>(static_cast<void *>(&mem_holder.shm_mem));
   }

   static void initialize_shm()
   {
      //Obtain unique shm name and size
      std::string s;
      while(1){
         //Try to pass shm state to initializing
         ::boost::uint32_t tmp = atomic_cas32(&this_module_shm_initialized, Initializing, Uninitialized);
         if(tmp >= Initialized){
            break;
         }
         //If some other thread is doing the work wait
         else if(tmp == Initializing){
            thread_yield();
         }
         else{ //(tmp == Uninitialized)
            //If not initialized try it again?
            try{
               //Remove old shared memory from the system
               intermodule_singleton_helpers::managed_sh_dependant<ManagedGlobalMemory>::remove_old_gmem();
               //
               if(s.empty()){
                  intermodule_singleton_helpers::get_shm_name(s);
               }
               const char *ShmName = s.c_str();
               const std::size_t ShmSize = intermodule_singleton_helpers::get_shm_size();;

               //in-place construction of the shared memory class
               ::new (&get_shm())ManagedGlobalMemory(open_or_create, ShmName, ShmSize);
               //Use shared memory internal lock to initialize the lock file
               //that will mark this gmem as "in use".
               typename intermodule_singleton_helpers::managed_sh_dependant<ManagedGlobalMemory>::
                  lock_file_logic f(get_shm());
               //If function failed (maybe a competing process has erased the shared
               //memory between creation and file locking), retry with a new instance.
               if(f.retry_with_new_shm){
                  get_shm().~ManagedGlobalMemory();
                  atomic_write32(&this_module_shm_initialized, Destroyed);
               }
               else{
                  //Locking succeeded, so this shared memory module-instance is ready
                  atomic_write32(&this_module_shm_initialized, Initialized);
                  break;
               }
            }
            catch(...){
               //
               throw;
            }
         }
      }
   }

   static void destroy_shm()
   {
      if(!atomic_read32(&this_module_singleton_count)){
         //This module is being unloaded, so destroy
         //the shared memory object of this module
         //and unlink the shared memory if it's the last
         typename intermodule_singleton_helpers::managed_sh_dependant<ManagedGlobalMemory>::
            unlink_shmlogic f(get_shm());
         (get_shm()).~ManagedGlobalMemory();
         atomic_write32(&this_module_shm_initialized, Destroyed);
         //Do some cleanup for other processes old gmem instances
         intermodule_singleton_helpers::managed_sh_dependant<ManagedGlobalMemory>::remove_old_gmem();
      }
   }

   //Static data, zero-initalized without any dependencies
   //this_module_singleton_count is the number of singletons used by this module
   static volatile boost::uint32_t this_module_singleton_count;

   //this_module_shm_initialized is the state of this module's shm class object.
   //Values: Uninitialized, Initializing, Initialized, Broken
   static volatile boost::uint32_t this_module_shm_initialized;

   //Raw memory to construct the shared memory manager  
   static struct mem_holder_t
   {
      ::boost::detail::max_align aligner;
      char shm_mem [sizeof(ManagedGlobalMemory)];
   } mem_holder;
};

template<class ManagedGlobalMemory>
volatile boost::uint32_t intermodule_singleton_common<ManagedGlobalMemory>::this_module_singleton_count;

template<class ManagedGlobalMemory>
volatile boost::uint32_t intermodule_singleton_common<ManagedGlobalMemory>::this_module_shm_initialized;

template<class ManagedGlobalMemory>
typename intermodule_singleton_common<ManagedGlobalMemory>::mem_holder_t
   intermodule_singleton_common<ManagedGlobalMemory>::mem_holder;

//Now this class is a singleton, initializing the singleton in
//the first get() function call if LazyInit is false. If true
//then the singleton will be initialized when loading the module.
template<typename C, bool LazyInit, bool Phoenix, class ManagedGlobalMemory>
class intermodule_singleton_impl
{
   public:

   static C& get()   //Let's make inlining easy
   {
      if(!this_module_singleton_ptr){
         if(lifetime.dummy_function()){  //This forces lifetime instantiation, for reference counted destruction
            atentry_work();
         }
      }
      return *static_cast<C*>(this_module_singleton_ptr);
   }

   private:

   static void atentry_work()
   {
      intermodule_singleton_common<ManagedGlobalMemory>::initialize_singleton_logic
         (this_module_singleton_ptr, this_module_singleton_initialized, singleton_constructor, Phoenix);
   }

   static void atexit_work()
   {
      intermodule_singleton_common<ManagedGlobalMemory>::finalize_singleton_logic
         (this_module_singleton_ptr, this_module_singleton_initialized, singleton_destructor);
   }

   //These statics will be zero-initialized without any constructor call dependency
   //this_module_singleton_ptr will be a module-local pointer to the singleton
   static void*                      this_module_singleton_ptr;

   //this_module_singleton_count will be used to synchronize threads of the same module
   //for access to a singleton instance, and to flag the state of the
   //singleton.
   static volatile boost::uint32_t   this_module_singleton_initialized;

   //This class destructor will trigger singleton destruction
   struct lifetime_type_lazy
   {
      bool dummy_function()
      {  return m_dummy == 0; }

      ~lifetime_type_lazy()
      {
         if(!Phoenix){
            atexit_work();
         }
      }

      //Dummy volatile so that the compiler can't resolve its value at compile-time
      //and can't avoid lifetime_type instantiation if dummy_function() is called.
      static volatile int m_dummy;
   };

   struct lifetime_type_static
      : public lifetime_type_lazy
   {
      lifetime_type_static()
      {  atentry_work();  }
   };

   typedef typename if_c
      <LazyInit, lifetime_type_lazy, lifetime_type_static>::type lifetime_type;

   static lifetime_type lifetime;

   //A reference count to be stored in shared memory holding the number
   //of singletons (one per module) attached to the instance pointed by
   //the internal ptr.
   struct ref_count_ptr
   {
      ref_count_ptr(C *p, boost::uint32_t count)
         : ptr(p), singleton_ref_count(count)
      {}
      C *ptr;
      //This reference count serves to count the number of attached
      //modules to this singleton
      volatile boost::uint32_t singleton_ref_count;
   };

   //A functor to be executed inside shared memory lock that just
   //searches for the singleton in shm and if not present creates a new one.
   //If singleton constructor throws, the exception is propagated
   struct init_atomic_func
   {
      init_atomic_func(ManagedGlobalMemory &m)
         : mshm(m)
      {}

      void operator()()
      {
         ref_count_ptr *rcount = mshm.template find<ref_count_ptr>(unique_instance).first;
         if(!rcount){
            C *p = new C();
            try{
               rcount = mshm.template construct<ref_count_ptr>(unique_instance)(p, 0u);
            }
            catch(...){
               delete p;
               throw;
            }
         }
         if(Phoenix){
            std::atexit(&atexit_work);
         }
         atomic_inc32(&rcount->singleton_ref_count);
         ret_ptr = rcount->ptr;
      }
      ManagedGlobalMemory &mshm;
      void *ret_ptr;
   };

   //A functor to be executed inside shared memory lock that just
   //deletes the singleton in shm if the attached count reaches to zero
   struct fini_atomic_func
   {
      fini_atomic_func(ManagedGlobalMemory &m)
         : mshm(m)
      {}

      void operator()()
      {
         ref_count_ptr *rcount = mshm.template find<ref_count_ptr>(unique_instance).first;
            //The object must exist
         BOOST_ASSERT(rcount);
         //Check if last reference
         if(atomic_dec32(&rcount->singleton_ref_count) == 1){
            //If last, destroy the object
            BOOST_ASSERT(rcount->ptr != 0);
            delete rcount->ptr;
            //Now destroy shm entry
            bool destroyed = mshm.template destroy<ref_count_ptr>(unique_instance);
            (void)destroyed;  BOOST_ASSERT(destroyed == true);
         }
      }
      ManagedGlobalMemory &mshm;
      void *ret_ptr;
   };

   //A wrapper to execute init_atomic_func
   static void *singleton_constructor(ManagedGlobalMemory &mshm)
   {
      init_atomic_func f(mshm);
      mshm.atomic_func(f);
      return f.ret_ptr;
   }

   //A wrapper to execute fini_atomic_func
   static void singleton_destructor(void *p, ManagedGlobalMemory &mshm)
   {  (void)p;
      fini_atomic_func f(mshm);
      mshm.atomic_func(f);
   }
};

template <typename C, bool L, bool P, class ManagedGlobalMemory>
volatile int intermodule_singleton_impl<C, L, P, ManagedGlobalMemory>::lifetime_type_lazy::m_dummy = 0;

//These will be zero-initialized by the loader
template <typename C, bool L, bool P, class ManagedGlobalMemory>
void *intermodule_singleton_impl<C, L, P, ManagedGlobalMemory>::this_module_singleton_ptr = 0;

template <typename C, bool L, bool P, class ManagedGlobalMemory>
volatile boost::uint32_t intermodule_singleton_impl<C, L, P, ManagedGlobalMemory>::this_module_singleton_initialized = 0;

template <typename C, bool L, bool P, class ManagedGlobalMemory>
typename intermodule_singleton_impl<C, L, P, ManagedGlobalMemory>::lifetime_type
   intermodule_singleton_impl<C, L, P, ManagedGlobalMemory>::lifetime;

}  //namespace ipcdetail{
}  //namespace interprocess{
}  //namespace boost{

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_INTERMODULE_SINGLETON_COMMON_HPP
