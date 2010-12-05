//  multi-protocol endpoint class
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef SERVER_DYNAMIC_ENDPOINT_HPP
#define SERVER_DYNAMIC_ENDPOINT_HPP

#include <cassert>

#include <boost/throw_exception.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>

namespace nova
{

struct nova_protocol
{
    typedef struct nova_endpoint endpoint;

    int family(void) const
    {
        return family_;
    }

    int type(void) const
    {
        return type_;
    }

    int protocol(void) const
    {
        return protocol_;
    }

    nova_protocol(int family, int type, int protocol):
        family_(family), type_(type), protocol_(protocol)
    {}

private:
    int family_;
    int type_;
    int protocol_;

    friend class nova_endpoint;
};

class nova_endpoint
{
    typedef boost::asio::detail::socket_addr_type data_type;

public:
    nova_endpoint(void):
        type_(AF_INET, SOCK_DGRAM, IPPROTO_UDP)
    {
        data_.v4.sin_family = AF_INET;
        data_.v4.sin_port = 0;
        data_.v4.sin_addr.s_addr = INADDR_ANY;
    }

    nova_endpoint(nova_endpoint const & other):
        type_(other.type_), data_(other.data_)
    {}

    template <typename Protocol>
    nova_endpoint(boost::asio::ip::basic_endpoint<Protocol> const & other):
        type_(other.protocol().family(), other.protocol().type(), other.protocol().protocol())
    {
        assert(capacity() >= other.capacity());
        std::memcpy(&data_.base, other.data(), other.capacity());
    }

    typedef nova_protocol protocol_type;

    protocol_type const & protocol(void) const
    {
        return type_;
    }

    data_type * data()
    {
        return &data_.base;
    }

    const data_type * data() const
    {
        return &data_.base;
    }

    std::size_t size() const
    {
        if (type_.type_ == AF_INET)
            return sizeof(boost::asio::detail::sockaddr_in4_type);
        else
            return sizeof(boost::asio::detail::sockaddr_in6_type);
    }

    void resize(std::size_t size)
    {
        if (size > sizeof(boost::asio::detail::sockaddr_storage_type))
        {
            boost::system::system_error e(boost::asio::error::invalid_argument);
            boost::throw_exception(e);
        }
    }

    std::size_t capacity() const
    {
        return sizeof(boost::asio::detail::sockaddr_storage_type);
    }

    boost::asio::ip::address address() const
    {
        using namespace std; // For memcpy.
        if (type_.family_ == AF_INET)
        {
            return boost::asio::ip::address_v4(
                boost::asio::detail::socket_ops::network_to_host_long(
                    data_.v4.sin_addr.s_addr));
        }
        else
        {
            boost::asio::ip::address_v6::bytes_type bytes;
            memcpy(bytes.elems, data_.v6.sin6_addr.s6_addr, 16);
            return boost::asio::ip::address_v6(bytes, data_.v6.sin6_scope_id);
        }
    }

    unsigned short port() const
    {
        if (type_.family_ == AF_INET)
        {
            return boost::asio::detail::socket_ops::network_to_host_short(
                data_.v4.sin_port);
        }
        else
        {
            return boost::asio::detail::socket_ops::network_to_host_short(
                data_.v6.sin6_port);
        }
    }

    friend bool operator==(const nova_endpoint & e1,
                           const nova_endpoint & e2)
    {
        return e1.address() == e2.address() && e1.port() == e2.port();
    }

    friend bool operator!=(const nova_endpoint & e1,
        const nova_endpoint & e2)
    {
        return e1.address() != e2.address() || e1.port() != e2.port();
    }

    friend bool operator<(const nova_endpoint & e1,
        const nova_endpoint & e2)
    {
        if (e1.address() < e2.address())
            return true;
        if (e1.address() != e2.address())
            return false;
        return e1.port() < e2.port();
    }

private:
    protocol_type type_;

    union data_union
    {
        boost::asio::detail::socket_addr_type base;
        boost::asio::detail::sockaddr_storage_type storage;
        boost::asio::detail::sockaddr_in4_type v4;
        boost::asio::detail::sockaddr_in6_type v6;
    } data_;
};

} /* namespace nova */

#endif /* SERVER_DYNAMIC_ENDPOINT_HPP */
