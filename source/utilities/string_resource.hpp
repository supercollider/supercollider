//  string resource template class
//  Copyright (C) 2007, 2009 Tim Blechmann
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

#ifndef __STRING_RESOURCE_HPP
#define __STRING_RESOURCE_HPP

#include "utilities/rw_mutex.hpp"
#include "boost/detail/atomic_count.hpp"
#include "boost/intrusive_ptr.hpp"

#include "boost/intrusive/set.hpp"
#include "boost/intrusive/set_hook.hpp"

#include <functional>
#include <cassert>

namespace nova
{
namespace detail
{

struct tag
{};

namespace bi = boost::intrusive;

typedef boost::intrusive::set_base_hook<bi::link_mode<bi::auto_unlink> > string_resource_data_base;

template <class string_type>
class string_resource_data:
    public string_resource_data_base
{
    string_resource_data(string_type const & data):
        str(data), ref_count(0), del_ref(0)
    {}

public:
    bool operator < (string_resource_data const & rhs) const
    {
        return str < rhs.str;
    }

    operator string_type const & (void)
    {
        return str;
    }

private:
    const string_type str;
    boost::detail::atomic_count ref_count, del_ref;

    template <class string_type2>
    friend class string_resource_container;

    template <class string_type2>
    friend void intrusive_ptr_add_ref(string_resource_data<string_type2> * p);

    template <class string_type2>
    friend void intrusive_ptr_release(string_resource_data<string_type2> * p);

    template <typename string_type2>
    friend struct srd_string_compare;
};

template <typename string_type>
inline void intrusive_ptr_add_ref(string_resource_data<string_type> * p)
{
    ++(p->ref_count);
}

template <typename string_type>
inline void intrusive_ptr_release(string_resource_data<string_type> * p);


template <typename string_type>
struct srd_string_compare
{
    inline bool operator()(string_resource_data<string_type> const & lhs, string_type const & rhs) const
    {
        return lhs.str < rhs;
    }

    inline bool operator()(string_type const & lhs, string_resource_data<string_type> const & rhs) const
    {
        return lhs < rhs.str;
    }
};


template <class string_type>
class string_resource_container:
    boost::noncopyable
{
    typedef string_resource_data<string_type> set_value_type;
    typedef boost::intrusive_ptr<set_value_type> string_resource_data_ptr;

    typedef boost::intrusive::set<set_value_type,
                                  boost::intrusive::compare<std::less<set_value_type> >,
                                  boost::intrusive::constant_time_size<false> >
    container_type;

    typedef nonrecursive_rw_mutex::shared_lock shared_lock;
    typedef nonrecursive_rw_mutex::unique_lock unique_lock;

public:
    ~string_resource_container(void)
    {
        nonrecursive_scoped_write_lock lock(mutex);
        assert(container.empty());
    }

    static inline string_resource_data_ptr &
    handle_ctor(string_resource_data_ptr & ptr)
    {
        switch (ptr->ref_count) {
        case 1:
            ++ptr->ref_count;
            break;
        case 2:
            ++ptr->del_ref;
        }
        return ptr;
    }

protected:
    string_resource_data_ptr insert(string_type const & str)
    {
        /*
         * - if string has been registered, return it
         */
        {
            shared_lock lock(mutex);
            typename container_type::iterator it = container.find(str, srd_string_compare<string_type>());

            if (it != container.end()) {
                string_resource_data_ptr ret(&(*it));
                return handle_ctor(ret);
            }
        }

        /*
         * - now we really have to insert a new string
         */
        unique_lock lock(mutex);
        typename container_type::iterator it = container.find(str, srd_string_compare<string_type>());
        if (likely(it == container.end()))
        {
            /* add new string_resource_data */
            string_resource_data_ptr ret (new set_value_type(str), true);
            container.insert(*ret);

            assert(ret->ref_count);
            return handle_ctor(ret);
        }
        else {
            string_resource_data_ptr ret(&(*it));
            return handle_ctor(ret);
        }
    }

    void remove(set_value_type * ptr)
    {
        unique_lock lock(mutex);

        if (ptr->del_ref) {
            --(ptr->del_ref);
            return;
        }

        if (ptr->is_linked())
            ptr->unlink();

        delete ptr;
    }

private:

    container_type container;
    nonrecursive_rw_mutex mutex;

    template <class string_type2>
    friend class string_resource;

    template <typename string_type2>
    friend void intrusive_ptr_release(string_resource_data<string_type2> * p);
};


} /* namespace detail */

template <class string_type>
class string_resource
{
public:
    typedef detail::string_resource_container<string_type> container_type;
    typedef int foo;

private:
    static container_type container;

public:
    string_resource(void):
        string_ptr(container.insert(""))
    {}

    string_resource(string_type const & str):
        string_ptr(container.insert(str))
    {}

    string_resource(string_resource const & str):
        string_ptr(str.string_ptr)
    {}

    void reset(string_type const & str)
    {
        string_ptr = container.insert(str);
    }

    void reset(string_resource const & str)
    {
        string_ptr = str.string_ptr;
    }

    /** this is only valid, as long as this object exists */
    operator string_type const & (void) const
    {
        return *string_ptr;
    }

    string_type const & as_str(void) const
    {
        return *string_ptr;
    }


    /** \brief some operators */
    /* @{ */
    bool operator == (string_type const & rhs) const
    {
        return string_ptr->operator string_type const &() == rhs;
    }

    bool operator == (string_resource const & rhs) const
    {
        return string_ptr == rhs.string_ptr;
    }

    template <typename rhs_type>
    bool operator != (rhs_type const & rhs) const
    {
        return not operator==(rhs);
    }

    bool operator < (string_resource const & rhs) const
    {
        return string_ptr < rhs.string_ptr;
    }
    /* @} */

private:
    boost::intrusive_ptr<detail::string_resource_data<string_type> > string_ptr;

    template <typename string_type2>
    friend void detail::intrusive_ptr_release(detail::string_resource_data<string_type2> * p);

    template <typename ostream, class string_type2>
    friend ostream & operator<<(ostream& stream, string_resource<string_type2> const & sr);
};

template <typename ostream, class string_type>
ostream & operator<<(ostream& stream, string_resource<string_type> const & sr)
{
    string_type const & str = sr;
    stream << str;
    return stream;
}

namespace detail
{
    template <typename string_type>
    inline void intrusive_ptr_release(string_resource_data<string_type> * p)
    {
        int new_val = --(p->ref_count);

        if (new_val == 1)
            string_resource<string_type>::container.remove(p);
    }
}

template <typename string> typename string_resource<string>::container_type string_resource<string>::container;


} /* namespace nova */

#endif /* __STRING_RESOURCE_HPP */
