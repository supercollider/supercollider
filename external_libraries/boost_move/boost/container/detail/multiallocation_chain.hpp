//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINERS_DETAIL_MULTIALLOCATION_CHAIN_HPP
#define BOOST_CONTAINERS_DETAIL_MULTIALLOCATION_CHAIN_HPP

#include <boost/container/detail/config_begin.hpp>
#include <boost/container/detail/utilities.hpp>
#include <boost/container/detail/type_traits.hpp>
#include <boost/container/detail/transform_iterator.hpp>

namespace boost {
namespace container {
namespace containers_detail {

template<class VoidPointer>
class basic_multiallocation_slist
{
public:
   typedef VoidPointer                                   void_pointer;

private:
   static VoidPointer &priv_get_ref(const VoidPointer &p)
   {  return *static_cast<void_pointer*>(containers_detail::get_pointer(p));  }

   basic_multiallocation_slist(basic_multiallocation_slist &);
   basic_multiallocation_slist &operator=(basic_multiallocation_slist &);

public:
   BOOST_ENABLE_MOVE_EMULATION(basic_multiallocation_slist)

      //!This iterator is returned by "allocate_many" functions so that
      //!the user can access the multiple buffers allocated in a single call
   class iterator
      :  public std::iterator<std::input_iterator_tag, char>
   {
      friend class basic_multiallocation_slist<void_pointer>;
      void unspecified_bool_type_func() const {}
      typedef void (iterator::*unspecified_bool_type)() const;

      iterator(void_pointer node_range)
         : next_node_(node_range)
      {}

   public:
      typedef char         value_type;
      typedef value_type & reference;
      typedef value_type * pointer;

      iterator()
         : next_node_(0)
      {}

      iterator &operator=(const iterator &other)
      {  next_node_ = other.next_node_;  return *this;  }

   public:
      iterator& operator++() 
      {
         next_node_ = *static_cast<void_pointer*>(containers_detail::get_pointer(next_node_));
         return *this;
      }

      iterator operator++(int)
      {
         iterator result(*this);
         ++*this;
         return result;
      }

      bool operator== (const iterator& other) const
      { return next_node_ == other.next_node_; }

      bool operator!= (const iterator& other) const
      { return !operator== (other); }

      reference operator*() const
      {  return *static_cast<char*>(containers_detail::get_pointer(next_node_)); }

      operator unspecified_bool_type() const  
      {  return next_node_? &iterator::unspecified_bool_type_func : 0;   }

      pointer operator->() const
      { return &(*(*this)); }

   private:
      void_pointer next_node_;
   };

private:
   iterator it_;

public:
   basic_multiallocation_slist()
      :  it_(iterator())
   {}

   basic_multiallocation_slist(void_pointer p)
      :  it_(p ? iterator_to(p) : iterator())
   {}

   basic_multiallocation_slist(BOOST_RV_REF(basic_multiallocation_slist) other)
      :  it_(iterator())
   {  this->swap(other); }

   basic_multiallocation_slist& operator=(BOOST_RV_REF(basic_multiallocation_slist) other)
   {
      basic_multiallocation_slist tmp(boost::move(other));
      this->swap(tmp);
      return *this;
   }

   bool empty() const
   {  return !it_; }

   iterator before_begin() const
   {  return iterator(void_pointer(const_cast<void*>(static_cast<const void*>(&it_.next_node_)))); }

   iterator begin() const
   {  return it_; }

   iterator end() const
   {  return iterator();  }

   void clear()
   {  this->it_.next_node_ = void_pointer(0); }

   iterator insert_after(iterator it, void_pointer m)
   {
      priv_get_ref(m) = priv_get_ref(it.next_node_);
      priv_get_ref(it.next_node_) = m;
      return iterator(m);
   }

   void push_front(void_pointer m)
   {
      priv_get_ref(m) = this->it_.next_node_;
      this->it_.next_node_ = m;
   }

   void pop_front()
   {  ++it_; }

   void *front() const
   {  return containers_detail::get_pointer(it_.next_node_); }

   void splice_after(iterator after_this, iterator before_begin, iterator before_end)
   {
      if (after_this != before_begin && after_this != before_end && before_begin != before_end) {
         void_pointer next_b = priv_get_ref(before_begin.next_node_);
         void_pointer next_e = priv_get_ref(before_end.next_node_);
         void_pointer next_p = priv_get_ref(after_this.next_node_);
         priv_get_ref(before_begin.next_node_) = next_e;
         priv_get_ref(before_end.next_node_)   = next_p;
         priv_get_ref(after_this.next_node_)   = next_b;
      }
   }

   void swap(basic_multiallocation_slist &other_chain)
   {
      std::swap(this->it_, other_chain.it_);
   }

   static iterator iterator_to(void_pointer p)
   {  return iterator(p);  }

   void_pointer extract_data()
   {
      void_pointer ret = empty() ? void_pointer(0) : void_pointer(&*it_);
      it_ = iterator();
      return ret;
   }
};

template<class VoidPointer>
class basic_multiallocation_cached_slist
{
private:
   basic_multiallocation_slist<VoidPointer>  slist_;
   typename basic_multiallocation_slist<VoidPointer>::iterator last_;

   basic_multiallocation_cached_slist(basic_multiallocation_cached_slist &);
   basic_multiallocation_cached_slist &operator=(basic_multiallocation_cached_slist &);

public:
   BOOST_ENABLE_MOVE_EMULATION(basic_multiallocation_cached_slist)

      typedef typename basic_multiallocation_slist<VoidPointer>::void_pointer  void_pointer;
   typedef typename basic_multiallocation_slist<VoidPointer>::iterator      iterator;

   basic_multiallocation_cached_slist()
      :  slist_(), last_(slist_.before_begin())
   {}
   /*
   basic_multiallocation_cached_slist(iterator first_node)
   :  slist_(first_node), last_(slist_.before_begin())
   {
   iterator end;
   while(first_node != end){
   ++last_;
   }
   }*/

   basic_multiallocation_cached_slist(void_pointer p1, void_pointer p2)
      :  slist_(p1), last_(p2 ? iterator_to(p2) : slist_.before_begin())
   {}

   basic_multiallocation_cached_slist(BOOST_RV_REF(basic_multiallocation_cached_slist) other)
      :  slist_(), last_(slist_.before_begin())
   {  this->swap(other); }

   basic_multiallocation_cached_slist& operator=(BOOST_RV_REF(basic_multiallocation_cached_slist) other)
   {
      basic_multiallocation_cached_slist tmp(boost::move(other));
      this->swap(tmp);
      return *this;
   }

   bool empty() const
   {  return slist_.empty(); }

   iterator before_begin() const
   {  return slist_.before_begin(); }

   iterator begin() const
   {  return slist_.begin(); }

   iterator end() const
   {  return slist_.end(); }

   iterator last() const
   {  return last_; }

   void clear()
   {
      slist_.clear();
      last_ = slist_.before_begin();
   }

   iterator insert_after(iterator it, void_pointer m)
   {
      slist_.insert_after(it, m);
      if(it == last_){
         last_ = slist_.iterator_to(m);
      }
      return iterator_to(m);
   }

   void push_front(void_pointer m)
   {  this->insert_after(this->before_begin(), m);   }

   void push_back(void_pointer m)
   {  this->insert_after(last_, m);   }

   void pop_front()
   {
      if(last_ == slist_.begin()){
         last_ = slist_.before_begin();
      }
      slist_.pop_front();
   }

   void *front() const
   {  return slist_.front(); }

   void splice_after(iterator after_this, iterator before_begin, iterator before_end)
   {
      if(before_begin == before_end)
         return;
      if(after_this == last_){
         last_ = before_end;
      }
      slist_.splice_after(after_this, before_begin, before_end);
   }

   void swap(basic_multiallocation_cached_slist &x)
   {
      slist_.swap(x.slist_);
      using std::swap;
      swap(last_, x.last_);
      if(last_ == x.before_begin()){
         last_ = this->before_begin();
      }
      if(x.last_ == this->before_begin()){
         x.last_ = x.before_begin();
      }
   }

   static iterator iterator_to(void_pointer p)
   {  return basic_multiallocation_slist<VoidPointer>::iterator_to(p);  }

   std::pair<void_pointer, void_pointer> extract_data()
   {
      if(this->empty()){
         return std::pair<void_pointer, void_pointer>(void_pointer(0), void_pointer(0));
      }
      else{
         void_pointer p1 = slist_.extract_data();
         void_pointer p2 = void_pointer(&*last_);
         last_ = iterator();
         return std::pair<void_pointer, void_pointer>(p1, p2);
      }
   }
};

template<class MultiallocatorCachedSlist>
class basic_multiallocation_cached_counted_slist
{
private:
   MultiallocatorCachedSlist  cached_slist_;
   std::size_t size_;

   basic_multiallocation_cached_counted_slist(basic_multiallocation_cached_counted_slist &);
   basic_multiallocation_cached_counted_slist &operator=(basic_multiallocation_cached_counted_slist &);

public:
   BOOST_ENABLE_MOVE_EMULATION(basic_multiallocation_cached_counted_slist)

      typedef typename MultiallocatorCachedSlist::void_pointer  void_pointer;
   typedef typename MultiallocatorCachedSlist::iterator      iterator;

   basic_multiallocation_cached_counted_slist()
      :  cached_slist_(), size_(0)
   {}

   basic_multiallocation_cached_counted_slist(void_pointer p1, void_pointer p2, std::size_t n)
      :  cached_slist_(p1, p2), size_(n)
   {}

   basic_multiallocation_cached_counted_slist(BOOST_RV_REF(basic_multiallocation_cached_counted_slist) other)
      :  cached_slist_(), size_(0)
   {  this->swap(other); }

   basic_multiallocation_cached_counted_slist& operator=(BOOST_RV_REF(basic_multiallocation_cached_counted_slist) other)
   {
      basic_multiallocation_cached_counted_slist tmp(boost::move(other));
      this->swap(tmp);
      return *this;
   }

   basic_multiallocation_cached_counted_slist (MultiallocatorCachedSlist mem, std::size_t n)
      :  cached_slist_(boost::move(mem)), size_(n)
   {}

   bool empty() const
   {  return cached_slist_.empty(); }

   std::size_t size() const
   {  return size_;  }

   iterator before_begin() const
   {  return cached_slist_.before_begin(); }

   iterator begin() const
   {  return cached_slist_.begin(); }

   iterator end() const
   {  return cached_slist_.end(); }

   iterator last() const
   {  return cached_slist_.last(); }

   void clear()
   {
      cached_slist_.clear();
      size_ = 0;
   }

   iterator insert_after(iterator it, void_pointer m)
   {
      iterator ret = cached_slist_.insert_after(it, m);
      ++size_;
      return ret;
   }

   void push_front(void_pointer m)
   {  this->insert_after(this->before_begin(), m);   }

   void push_back(void_pointer m)
   {  this->insert_after(this->before_begin(), m);   }

   void pop_front()
   {
      cached_slist_.pop_front();
      --size_;
   }

   void *front() const
   {  return cached_slist_.front(); }

   void splice_after(iterator after_this, basic_multiallocation_cached_counted_slist &x, iterator before_begin, iterator before_end)
   {
      std::size_t n = static_cast<std::size_t>(std::distance(before_begin, before_end));
      this->splice_after(after_this, x, before_begin, before_end, n);
   }

   void splice_after(iterator after_this, basic_multiallocation_cached_counted_slist &x, iterator before_begin, iterator before_end, std::size_t n)
   {
      cached_slist_.splice_after(after_this, before_begin, before_end);
      size_ += n;
      x.size_ -= n;
   }

   void splice_after(iterator after_this, basic_multiallocation_cached_counted_slist &x)
   {
      cached_slist_.splice_after(after_this, x.before_begin(), x.last());
      size_ += x.size_;
      x.size_ = 0;
   }

   void swap(basic_multiallocation_cached_counted_slist &x)
   {
      cached_slist_.swap(x.cached_slist_);
      using std::swap;
      swap(size_, x.size_);
   }

   static iterator iterator_to(void_pointer p)
   {  return MultiallocatorCachedSlist::iterator_to(p);  }

   std::pair<void_pointer, void_pointer> extract_data()
   {
      size_ = 0;
      return cached_slist_.extract_data();
   }
};

template<class T>
struct cast_functor
{
   typedef typename containers_detail::add_reference<T>::type result_type;
   result_type operator()(char &ptr) const
   {  return *static_cast<T*>(static_cast<void*>(&ptr));  }
};


template<class MultiallocationChain, class T>
class transform_multiallocation_chain
{
private:

   MultiallocationChain   holder_;
   typedef typename MultiallocationChain::void_pointer   void_pointer;
   typedef typename boost::pointer_to_other
      <void_pointer, T>::type                            pointer;

   transform_multiallocation_chain(transform_multiallocation_chain &);
   transform_multiallocation_chain &operator=(transform_multiallocation_chain &);

   static pointer cast(void_pointer p)
   {
      return pointer(static_cast<T*>(containers_detail::get_pointer(p)));
   }

public:
   BOOST_ENABLE_MOVE_EMULATION(transform_multiallocation_chain)

      typedef transform_iterator
      < typename MultiallocationChain::iterator
      , containers_detail::cast_functor <T> >                 iterator;

   transform_multiallocation_chain(void_pointer p1, void_pointer p2, std::size_t n)
      : holder_(p1, p2, n)
   {}

   transform_multiallocation_chain()
      : holder_()
   {}

   transform_multiallocation_chain(BOOST_RV_REF(transform_multiallocation_chain) other)
      : holder_()
   {  this->swap(other); }

   transform_multiallocation_chain(BOOST_RV_REF(MultiallocationChain) other)
      : holder_(boost::move(other))
   {}

   transform_multiallocation_chain& operator=(BOOST_RV_REF(transform_multiallocation_chain) other)
   {
      transform_multiallocation_chain tmp(boost::move(other));
      this->swap(tmp);
      return *this;
   }

   void push_front(pointer mem)
   {  holder_.push_front(mem);  }

   void swap(transform_multiallocation_chain &other_chain)
   {  holder_.swap(other_chain.holder_); }
   /*
   void splice_after(iterator after_this, iterator before_begin, iterator before_end)
   {  holder_.splice_after(after_this.base(), before_begin.base(), before_end.base());  }
   */
   void splice_after(iterator after_this, transform_multiallocation_chain &x, iterator before_begin, iterator before_end, std::size_t n)
   {  holder_.splice_after(after_this.base(), x.holder_, before_begin.base(), before_end.base(), n);  }

   void pop_front()
   {  holder_.pop_front();  }

   pointer front() const
   {  return cast(holder_.front());   }

   bool empty() const
   {  return holder_.empty(); }

   iterator before_begin() const
   {  return iterator(holder_.before_begin());   }

   iterator begin() const
   {  return iterator(holder_.begin());   }

   iterator end() const
   {  return iterator(holder_.end());   }

   iterator last() const
   {  return iterator(holder_.last());   }

   std::size_t size() const
   {  return holder_.size();  }

   void clear()
   {  holder_.clear(); }

   iterator insert_after(iterator it, pointer m)
   {  return iterator(holder_.insert_after(it.base(), m)); }

   static iterator iterator_to(pointer p)
   {  return iterator(MultiallocationChain::iterator_to(p));  }

   std::pair<void_pointer, void_pointer> extract_data()
   {  return holder_.extract_data();  }

   MultiallocationChain extract_multiallocation_chain()
   {
      return MultiallocationChain(boost::move(holder_));
   }
};

}}}

// namespace containers_detail {
// namespace container {
// namespace boost {

#include <boost/container/detail/config_end.hpp>

#endif   //BOOST_CONTAINERS_DETAIL_MULTIALLOCATION_CHAIN_HPP
