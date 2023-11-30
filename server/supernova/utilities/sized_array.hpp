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

#pragma once

#include <algorithm>
#include <cassert>
#include <memory> /* std::allocator */
#include <type_traits>

#include <boost/mpl/if.hpp>

namespace nova {

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
template <typename T, class Alloc = std::allocator<T>> class sized_array : private Alloc::template rebind<T>::other {
    typedef typename Alloc::template rebind<T>::other Allocator;

public:
    // types
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    // construct/copy/destruct
    explicit sized_array(size_type size = 0, T const& def = T()):
        data_(size ? Allocator::allocate(size) : nullptr),
        size_(size) {
        for (size_type i = 0; i != size; ++i)
            Allocator::construct(data_ + i, def);
    }

    sized_array(sized_array& arg) = delete;
    sized_array& operator=(sized_array& arg) = delete;

private:
    template <typename int_type> void init_from_int(int_type size) {
        data_ = Allocator::allocate(size);
        size_ = size;
        for (size_type i = 0; i != size_; ++i)
            Alloc::construct(data_ + i, T());
    }

    template <typename Container> void init_from_container(Container const& container) {
        data_ = Allocator::allocate(container.size());
        size_ = container.size();

        size_type index = 0;
        typedef typename Container::const_iterator iterator;
        for (iterator it = container.begin(); it != container.end(); ++it)
            Allocator::construct(data_ + index++, *it);
        assert(index == size());
    }

    struct call_int_ctor {
        template <typename int_type> static void init(sized_array& array, int_type const& i) {
            array.init_from_int<int_type>(i);
        }
    };

    struct call_container_ctor {
        template <typename Container> static void init(sized_array& array, Container const& c) {
            array.init_from_container<Container>(c);
        }
    };

public:
    template <typename Constructor_arg> explicit sized_array(Constructor_arg const& arg) {
        typedef
            typename boost::mpl::if_<std::is_integral<Constructor_arg>, call_int_ctor, call_container_ctor>::type ctor;
        ctor::init(*this, arg);
    }

    explicit sized_array(sized_array&& arg) {
        data_ = arg.data_;
        size_ = arg.size();
        arg.data_ = 0;
        arg.size_ = 0;
    }

    /** move assignment */
    sized_array& operator=(sized_array&& arg) {
        data_ = arg.data_;
        size_ = arg.size();
        arg.data_ = 0;
        arg.size_ = 0;
        return *this;
    }

    ~sized_array(void) {
        for (size_type i = 0; i != size(); ++i)
            Allocator::destroy(data_ + i);
        if (size())
            Allocator::deallocate(data_, size());
    }

    // iterator support
    iterator begin() { return data_; }

    const_iterator begin() const { return data_; }

    iterator end() { return data_ + size(); }

    const_iterator end() const { return data_ + size(); }

    // reverse iterator support
    reverse_iterator rbegin() { return reverse_iterator(data_ + size()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(data_ + size()); }

    reverse_iterator rend() { return reverse_iterator(data_); }

    const_reverse_iterator rend() const { return const_reverse_iterator(data_); }

    // capacity
    size_type size() const { return size_; }

    bool empty() const { return size_ == 0; }

    size_type max_size() const { return size_; }

    // element access
    reference operator[](size_type i) {
        assert(i < size());
        return data_[i];
    }

    const_reference operator[](size_type i) const {
        assert(i < size());
        return data_[i];
    }

    reference at(size_type i) {
        assert(i < size());
        return data_[i];
    }

    const_reference at(size_type i) const {
        assert(i < size());
        return data_[i];
    }

    reference front() { return data_[0]; }

    const_reference front() const { return data_[0]; }

    reference back() { return data_[size_ - 1]; }

    const_reference back() const { return data_[size_ - 1]; }

    const T* data() const { return data_; }

    T* c_array() { return data_; }

    // modifiers
    void assign(const T& t) {
        for (size_type i = 0; i != size_; ++i)
            data_[i] = t;
    }

    void resize(size_type new_size, T const& t = T()) {
        T* new_data = Allocator::allocate(new_size);

        for (size_type i = 0; i != new_size; ++i)
            Allocator::construct(new_data + i, t);

        std::copy(data_, data_ + (std::min)(new_size, size_), new_data);

        T* old_data = data_;
        data_ = new_data;
        for (size_type i = 0; i != size_; ++i)
            Allocator::destroy(old_data + i);
        if (size_)
            Allocator::deallocate(old_data, size_);
        size_ = new_size;
    }

private:
    T* data_;
    size_type size_;
};

} /* namespace nova */
