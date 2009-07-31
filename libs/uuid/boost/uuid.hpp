// Boost uuid.hpp header file  ----------------------------------------------//

// Copyright 2006 Andy Tompkins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Revision History
//  06 Feb 2006 - Initial Revision
//  09 Nov 2006 - fixed variant and version bits for v4 guids
//  13 Nov 2006 - added serialization
//  17 Nov 2006 - added name-based guid creation
//  20 Nov 2006 - add fixes for gcc (from Tim Blechmann)
//  07 Mar 2007 - converted to header only
//  10 May 2007 - removed need for Boost.Thread
//              - added better seed - thanks Peter Dimov
//              - removed null()
//              - replaced byte_count() and output_bytes() with size() and begin() and end()
//  11 May 2007 - fixed guid(ByteInputIterator first, ByteInputIterator last)
//              - optimized operator>>
//  14 May 2007 - converted from guid to uuid
//  29 May 2007 - uses new implementation of sha1
//  01 Jun 2007 - removed using namespace directives

#ifndef BOOST_UUID_HPP
#define BOOST_UUID_HPP

#include <boost/config.hpp>
#include <boost/operators.hpp>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/throw_exception.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iosfwd>
#include <stdexcept>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdio>

#include <boost/static_assert.hpp>

#include <boost/random.hpp>

#include <boost/uuid/sha1.hpp>

#ifdef BOOST_UUID_ENABLE_SERIALIZATION
#include <boost/serialization/level.hpp>
#include <boost/serialization/access.hpp>
#endif

namespace boost {

class uuid : boost::totally_ordered<uuid>
{
    typedef array<uint8_t, 16> data_type;

public:
    typedef data_type::value_type value_type;
    //typedef void iterator;
    //typedef void reference;
    //typedef data_type::const_reference const_reference;
    typedef data_type::const_iterator const_iterator;
    typedef data_type::difference_type difference_type;
    typedef data_type::size_type size_type;

public:
    uuid() /* throw() */
    {
        for (data_type::iterator i=data_.begin(); i!=data_.end(); ++i) {
            *i = 0;
        }
    }

    explicit uuid(char const*const str)
    {
        if (str == NULL) throw_invalid_argument();
        construct(std::string(str));
    }

    explicit uuid(wchar_t const*const str)
    {
        if (str == NULL) throw_invalid_argument();
        
        construct(std::wstring(str));
    }

    template <typename ch, typename char_traits, typename alloc>
        explicit uuid(std::basic_string<ch, char_traits, alloc> const& str)
    {
        construct(str);
    }

    template <typename ByteInputIterator>
        uuid(ByteInputIterator first, ByteInputIterator last)
    {
        data_type::iterator i_data = data_.begin();
        size_t i;
        for (i=0; i<16 && first != last; ++i) {
            *i_data++ = numeric_cast<uint8_t>(*first++);
        }
        if (i != 16) {
            boost::throw_exception(std::invalid_argument("invalid input iterator pair, must span 16 bytes"));
        }
    }

    uuid(uuid const& rhs) /* throw() */
        : data_(rhs.data_)
    {}

    ~uuid() /* throw() */
    {}

    uuid& operator=(uuid const& rhs) /* throw() */
    {
        data_ = rhs.data_;
        return *this;
    }

    bool operator==(uuid const& rhs) const /* throw() */
    {
        return (data_ == rhs.data_);
    }

    bool operator<(uuid const& rhs) const /* throw() */
    {
        return (data_ < rhs.data_);
    }

    bool is_null() const /* throw() */
    {
        for (data_type::const_iterator i=data_.begin(); i!=data_.end(); ++i) {
            if (*i != 0) {
                return false;
            }
        }

        return true;
    }

    std::string to_string() const
    {
        return to_basic_string<std::string::value_type, std::string::traits_type, std::string::allocator_type>();
    }

    std::wstring to_wstring() const
    {
        return to_basic_string<std::wstring::value_type, std::wstring::traits_type, std::wstring::allocator_type>();
    }

    template <typename ch, typename char_traits, typename alloc>
        std::basic_string<ch, char_traits, alloc> to_basic_string() const
    {
        std::basic_string<ch, char_traits, alloc> s;
        std::basic_stringstream<ch, char_traits, alloc> ss;
        if (!(ss << *this) || !(ss >> s)) {
            boost::throw_exception(std::runtime_error("failed to convert uuid to string"));
        }
        return s;
    }

    size_type size() const { return data_.size(); } /* throw() */
    const_iterator begin() const { return data_.begin(); } /* throw() */
    const_iterator end() const { return data_.end(); } /* throw() */

    //size_type max_size() const { return data_.max_size(); } /* throw() */
    //bool empty() const { return is_null(); } /* throw() */

    void swap(uuid &rhs) /* throw() */
    {
        std::swap(data_, rhs.data_);
    }

public:
    static uuid create();

    template <typename Engine>
    static uuid create(Engine& engine)
    {
        return create_random_based(engine);
    }

    static uuid create(uuid const& namespace_uuid, char const* name, int name_length)
    {
        return create_name_based(namespace_uuid, name, name_length);
    }

    static inline bool get_showbraces(std::ios_base & iosbase) {
        return (iosbase.iword(get_showbraces_index()) != 0);
    }
    static inline void set_showbraces(std::ios_base & iosbase, bool showbraces) {
        iosbase.iword(get_showbraces_index()) = showbraces;
    }

    static inline std::ios_base& showbraces(std::ios_base& iosbase)
    {
        set_showbraces(iosbase, true);
        return iosbase;
    }
    static inline std::ios_base& noshowbraces(std::ios_base& iosbase)
    {
        set_showbraces(iosbase, false);
        return iosbase;
    }

private:
    template <typename ch, typename char_traits, typename alloc>
        void construct(std::basic_string<ch, char_traits, alloc> const& str)
    {
        std::basic_stringstream<ch, char_traits, alloc> ss;
        if (!(ss << str) || !(ss >> *this)) {
            throw_invalid_argument();
        }
    }

    void throw_invalid_argument() const
    {
        boost::throw_exception(std::invalid_argument("invalid uuid string"));
    }

private:
    //random number based
    template <typename Engine>
    static uuid create_random_based(Engine& engine);

    // name based
    static uuid create_name_based(uuid const& namespace_uuid, char const* name, int name_length)
    {
		detail::sha1 sha;
        sha.process_bytes(namespace_uuid.data_.data(), namespace_uuid.data_.static_size);
        sha.process_bytes(name, name_length);
        unsigned int digest[5];

        sha.get_digest(digest);
        //if (sha.get_digest(digest) == false) {
        //    boost::throw_exception(std::runtime_error("create error"));
        //}

        unsigned char data[16];
        for (int i=0; i<4; ++i) {
            data[i*4+0] = ((digest[i] >> 24) & 0xFF);
            data[i*4+1] = ((digest[i] >> 16) & 0xFF);
            data[i*4+2] = ((digest[i] >> 8) & 0xFF);
            data[i*4+3] = ((digest[i] >> 0) & 0xFF);
        }

        // set variant
        // should be 0b10xxxxxx
        data[8] &= 0xBF;
        data[8] |= 0x80;

        // set version
        // should be 0b0101xxxx
        data[6] &= 0x5F; //0b01011111
        data[6] |= 0x50; //0b01010000

        return uuid(data, data+16);
    }

    static int get_showbraces_index()
    {
        static int index = std::ios_base::xalloc();
        return index;
    }

#ifdef BOOST_UUID_ENABLE_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & data_;
    }
#endif

private:
    data_type data_;
};

namespace detail {

inline unsigned * sha1_random_digest_state()
{
    static unsigned state[ 5 ];
    return state;
}

inline void sha1_random_digest( unsigned (&rd) [ 5 ] )
{
    sha1 sha;

    unsigned * ps = sha1_random_digest_state();

    unsigned state[ 5 ];
    std::memcpy( state, ps, sizeof( state ) ); // harmless data race

    sha.process_bytes( (unsigned char const*)state, sizeof( state ) );
    sha.process_bytes( (unsigned char const*)&ps, sizeof( ps ) );

    {
        std::time_t tm = std::time( 0 );
        sha.process_bytes( (unsigned char const*)&tm, sizeof( tm ) );
    }

    {
        std::clock_t ck = std::clock();
        sha.process_bytes( (unsigned char const*)&ck, sizeof( ck ) );
    }

    {
        unsigned rn[] = { std::rand(), std::rand(), std::rand() };
        sha.process_bytes( (unsigned char const*)rn, sizeof( rn ) );
    }

    {
        unsigned char buffer[ 20 ];

        if( std::FILE * f = std::fopen( "/dev/urandom", "rb" ) )
        {
            std::fread( buffer, 1, 20, f );
            std::fclose( f );
        }

        // using an uninitialized buffer[] if fopen fails
        // intentional, we rely on its contents being random
        sha.process_bytes( buffer, sizeof( buffer ) );
    }

    {
        unsigned * p = new unsigned;

        sha.process_bytes( (unsigned char const*)p, sizeof( *p ) );
        sha.process_bytes( (unsigned char const*)&p, sizeof( p ) );

        delete p;
    }

    sha.process_bytes( (unsigned char const*)rd, sizeof( rd ) );

    unsigned digest[ 5 ];
    sha.get_digest( digest );

    for( int i = 0; i < 5; ++i )
    {
        // harmless data race
        ps[ i ] ^= digest[ i ];
        rd[ i ] ^= digest[ i ];
    }
}

template <typename Engine>
inline unsigned long random_number(Engine& engine)
{
    typedef uniform_int<unsigned long> Distribution;
    typedef variate_generator<Engine&, Distribution> Generator;

    Distribution distribution( (std::numeric_limits<unsigned long>::min)(), (std::numeric_limits<unsigned long>::max)() );
    Generator generator(engine, distribution);
    return generator();
}

} // namespace detail

inline uuid uuid::create()
{
    unsigned int rd[5]; //seed
    detail::sha1_random_digest(rd);
    mt19937 engine(static_cast<mt19937::result_type>(rd[0]));
    return create(engine);
}

//random number based
template <typename Engine>
uuid uuid::create_random_based(Engine& engine)
{
    BOOST_STATIC_ASSERT(16 % sizeof(unsigned long) == 0);
    unsigned char data[16];

    for (size_t i=0; i<16; i+=sizeof(unsigned long))
    {
        //unsigned long number = detail::random_number(engine);
        //memcpy(&data[i], &number, sizeof(unsigned long));
        *reinterpret_cast<unsigned long*>(&data[i]) = detail::random_number(engine);
    }

    // set variant
    // should be 0b10xxxxxx
    data[8] &= 0xBF;
    data[8] |= 0x80;

    // set version
    // should be 0b0100xxxx
    data[6] &= 0x4F; //0b01001111
    data[6] |= 0x40; //0b01000000

    return uuid(data, data+16);
}

inline void swap(uuid &x, uuid &y)
{
    x.swap(y);
}

template <typename ch, typename char_traits>
    std::basic_ostream<ch, char_traits>& operator<<(std::basic_ostream<ch, char_traits> &os, uuid const& u)
{
    io::ios_flags_saver flags_saver(os);
    io::ios_width_saver width_saver(os);
    io::basic_ios_fill_saver<ch, char_traits> fill_saver(os);

	const typename std::basic_ostream<ch, char_traits>::sentry ok(os);
    if (ok) {
        bool showbraces = uuid::get_showbraces(os);
        if (showbraces) {
            os << os.widen('{');
        }
		os << std::hex;
        os.fill(os.widen('0'));
        
        size_t i=0;
        for (uuid::const_iterator i_data = u.begin(); i_data!=u.end(); ++i_data, ++i) {
            os.width(2);
            os << static_cast<unsigned int>(*i_data);
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                os << os.widen('-');
            }
        }
        if (showbraces) {
            os << os.widen('}');
        }
    }
    return os;
}

template <typename ch, typename char_traits>
    std::basic_istream<ch, char_traits>& operator>>(std::basic_istream<ch, char_traits> &is, uuid &u)
{
	const typename std::basic_istream<ch, char_traits>::sentry ok(is);
    if (ok) {
        unsigned char data[16];

		typedef std::ctype<ch> ctype_t;
		ctype_t const& ctype = std::use_facet<ctype_t>(is.getloc());

        ch xdigits[16];
        {
            char szdigits[17] = "0123456789ABCDEF";
            ctype.widen(szdigits, szdigits+16, xdigits);
        }
        ch*const xdigits_end = xdigits+16;

        ch c;
        c = is.peek();
        bool bHaveBraces = false;
        if (c == is.widen('{')) {
            bHaveBraces = true;
            is >> c; // read brace
        }

        for (size_t i=0; i<u.size() && is; ++i) {
            is >> c;
            c = ctype.toupper(c);

			ch* f = std::find(xdigits, xdigits_end, c);
            if (f == xdigits_end) {
				is.setstate(std::ios_base::failbit);
                break;
            }

			unsigned char byte = static_cast<unsigned char>(std::distance(xdigits, f));

            is >> c;
            c = ctype.toupper(c);
			f = std::find(xdigits, xdigits_end, c);
            if (f == xdigits_end) {
				is.setstate(std::ios_base::failbit);
                break;
            }

            byte <<= 4;
			byte |= static_cast<unsigned char>(std::distance(xdigits, f));

            data[i] = byte;

            if (is) {
                if (i == 3 || i == 5 || i == 7 || i == 9) {
                    is >> c;
					if (c != is.widen('-')) is.setstate(std::ios_base::failbit);
                }
            }
        }

        if (bHaveBraces && is) {
            is >> c;
			if (c != is.widen('}')) is.setstate(std::ios_base::failbit);
        }
        if (is) {
            u = uuid(data, data+16);
        }
    }
    return is;
}

} //namespace boost

#ifdef BOOST_UUID_ENABLE_SERIALIZATION
BOOST_CLASS_IMPLEMENTATION(boost::uuid, boost::serialization::primitive_type)
#endif

#endif // BOOST_UUID_HPP
