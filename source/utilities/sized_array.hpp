//  templated sized array class
//  Copyright (C) 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef UTILITIES_SIZED_ARRAY_HPP
#define UTILITIES_SIZED_ARRAY_HPP

#include <cassert>
#include <memory>               /* std::allocator */

#include <stdint.h>

#include <boost/move/move.hpp>

namespace nova
{

/** dynamically sized array
 *
 *  array class, inspired by boost.array, where the size is specified
 *  during the time of the construction.
 *  in contrary to std::vector, it is guaranteed to use continuous
 *  memory. the memory is allocated in the constructor an freed in the
 *  destructor, these are the only places, where the allocator
 *  functions are called.
 *
 * */
template<typename T,
         class Alloc = std::allocator<T> >
class sized_array:
    private Alloc::template rebind<T>::other
{
    typedef typename Alloc::template rebind<T>::other Allocator;

    BOOST_COPYABLE_AND_MOVABLE(sized_array);

public:
    // types
    typedef T                                     value_type;
    typedef T*                                    iterator;
    typedef const T*                              const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef T&                                    reference;
    typedef const T&                              const_reference;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;

    // construct/copy/destruct
    explicit sized_array(size_type size, T const & def):
        data_(Allocator::allocate(size)), size_(size)
    {
        for (size_type i = 0; i != size; ++i)
            Allocator::construct(data_ + i, def);
    }

#define EXPLICIT_CONSTRUCTOR(INT_TYPE)                  \
    explicit sized_array(INT_TYPE size):                \
        data_(Allocator::allocate(size)), size_(size)   \
    {                                                   \
        for (size_type i = 0; i != size_type(size); ++i)\
            Alloc::construct(data_ + i, T());           \
    }

    EXPLICIT_CONSTRUCTOR(int32_t);
    EXPLICIT_CONSTRUCTOR(uint32_t);
    EXPLICIT_CONSTRUCTOR(int64_t);
    EXPLICIT_CONSTRUCTOR(uint64_t);

#undef EXPLICIT_CONSTRUCTOR

    template<typename Container>
    explicit sized_array(Container const & container):
        data_(Allocator::allocate(container.size())), size_(container.size())
    {
        size_type index = 0;
        typedef typename Container::const_iterator iterator;
        for (iterator it = container.begin(); it != container.end(); ++it)
            Allocator::construct(data_ + index++, *it);
        assert(index == size());
    }

    explicit sized_array(BOOST_RV_REF(sized_array) arg)
    {
        operator=(arg);
    }

    /** move assignment */
    sized_array & operator=(BOOST_RV_REF(sized_array) arg)
    {
        data_ = arg.data_;
        size_ = arg.size();
        arg.data_ = 0;
        arg.size_ = 0;
        return *this;
    }

    ~sized_array(void)
    {
        for (size_type i = 0; i != size(); ++i)
            Allocator::destroy(data_ + i);
        if (size())
            Allocator::deallocate(data_, size());
    }

    // iterator support
    iterator begin()
    {
        return data_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size();
    }

    const_iterator end() const
    {
        return data_ + size();
    }

    // reverse iterator support
    reverse_iterator rbegin()
    {
        return reverse_iterator(data_ + size());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(data_ + size());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(data_);
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(data_);
    }

    // capacity
    size_type size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    size_type max_size() const
    {
        return size_;
    }

    // element access
    reference operator[](size_type i)
    {
        return data_[i];
    }

    const_reference operator[](size_type i) const
    {
        return data_[i];
    }

    reference at(size_type i)
    {
        return data_[i];
    }

    const_reference at(size_type i) const
    {
        return data_[i];
    }

    reference front()
    {
        return data_[0];
    }

    const_reference front() const
    {
        return data_[0];
    }

    reference back()
    {
        return data_[size_ - 1];
    }

    const_reference back() const
    {
        return data_[size_ - 1];
    }

    const T* data() const
    {
        return data_;
    }

    T* c_array()
    {
        return data_;
    }

    // modifiers
    void assign(const T& t)
    {
        for (size_type i = 0; i != size_; ++i)
            data_[i] = t;
    }

private:
    T * data_;
    size_type size_;
};

} /* namespace nova */

#endif /* UTILITIES_SIZED_ARRAY_HPP */
