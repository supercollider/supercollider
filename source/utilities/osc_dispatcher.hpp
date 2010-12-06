//  osc responder class
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef OSC_RESPONDER_HPP
#define OSC_RESPONDER_HPP

#include <string>
#include <map>
#include "exists.hpp"
#include "branch_hints.hpp"

#include <boost/tuple/tuple.hpp>

#include "osc/OscReceivedElements.h"

namespace nova
{


class osc_responder
{
public:
    virtual ~osc_responder()
    {}

    virtual void run(osc::ReceivedMessageArgumentIterator begin,
                     osc::ReceivedMessageArgumentIterator const & end) = 0;
};


class osc_dispatcher
{
    typedef std::string string;
    typedef std::size_t size_t;
    typedef std::multimap<string, osc_responder*> responder_map_t;

public:
    static bool is_pattern(const char * pattern)
    {
        for(;;)
        {
            if (unlikely(*pattern == 0))
                return false;
            if (unlikely((*pattern == '?') or (*pattern == '*')))
                return true;
            ++pattern;
        }
    }

    static unsigned int first_pattern_char(const char * pattern)
    {
        assert(is_pattern(pattern));

        unsigned int ret = 0;
        for(;;)
        {
            if (unlikely((*pattern == '?') or (*pattern == '*')))
                return ret;
            ++ret;
            ++pattern;
        }
        assert(false);
    }

    static bool match_pattern(const char * pattern, const char * address)
    {
        const char pc = *pattern;
        const char ac = *address;

        if ((pc == 0) and (ac == 0))
            return true;

        if ((pc == 0) or (ac == 0))
            return false;

        if (pc == '?')
            return match_pattern(++pattern, ++address);

        if (pc == '*')
        {
            for(;;)
            {
                if (match_pattern(pattern+1, address+1))
                    return true;
                ++address;
                if (*address == 0)
                    return *(pattern+1) == 0;
            }
        }

        if (pc != ac)
            return false;
        else
            return match_pattern(++pattern, ++address);
    }

    void add_responder(string const & address, osc_responder * resp)
    {
        responder_map.insert(std::make_pair(address, resp));
    }

    void remove_responder(string const & address, osc_responder * resp)
    {
        assert(exists(responder_map, address));

        responder_map_t::iterator it, end;
        boost::tie(it, end) = responder_map.equal_range(address);

        for(; it != end; ++it)
        {
            if (it->second == resp)
            {
                responder_map.erase(it);
                return;
            }
        }
        assert(false);
    }

    void handle_packet(const char * data, size_t length)
    {
        assert(length);
        if (data[0] == '#')
            handle_bundle(data, length);
        else
            handle_message(data, length);
    }

private:
    void handle_message(const char * data, size_t length)
    {
        osc::ReceivedMessage msg(osc::ReceivedPacket(data, length));
        handle_message(msg.AddressPattern(), msg.ArgumentsBegin(), msg.ArgumentsEnd());
    }

    void handle_bundle(const char * data, size_t length)
    {
        osc::ReceivedBundle bundle(osc::ReceivedPacket(data, length));
        handle_bundle(bundle.ElementsBegin(), bundle.ElementsEnd());
    }

    void handle_bundle(osc::ReceivedBundleElementIterator const & begin,
                       osc::ReceivedBundleElementIterator const & end)
    {
        for (osc::ReceivedBundleElementIterator it = begin; it != end; ++it)
        {
            if (it->IsBundle())
            {
                osc::ReceivedBundle bundle(*it);
                handle_bundle(bundle.ElementsBegin(), bundle.ElementsEnd());
            }
            else
            {
                osc::ReceivedMessage msg(*it);
                handle_message(msg.AddressPattern(), msg.ArgumentsBegin(), msg.ArgumentsEnd());
            }
        }
    }

    void handle_message(char const * pattern, osc::ReceivedMessageArgumentIterator const & begin,
                        osc::ReceivedMessageArgumentIterator const & end)
    {
        if (is_pattern(pattern))
            handle_pattern(pattern, begin, end);
        else
            handle_address(pattern, begin, end);
    }


    void handle_pattern(const char * pattern, osc::ReceivedMessageArgumentIterator const & begin,
                        osc::ReceivedMessageArgumentIterator const & end)
    {
        responder_map_t::iterator it;
        const unsigned int pattern_begin = first_pattern_char(pattern);

        const string pattern_root(pattern, pattern_begin);

        it = responder_map.lower_bound(pattern_root);

        for(; it != responder_map.end(); ++it)
        {
            if (match_pattern(pattern, it->first.c_str()))
            {
                osc_responder * resp = it->second;
                resp->run(begin, end);
                continue;
            }

            if (it->first.compare(0, pattern_begin, pattern_root))
                return;
        }
    }

    void handle_address(const char * address, osc::ReceivedMessageArgumentIterator const & begin,
                        osc::ReceivedMessageArgumentIterator const & end)
    {
        responder_map_t::iterator it, it_end;
        boost::tie(it, it_end) = responder_map.equal_range(address);

        for(; it != it_end; ++it)
        {
            osc_responder * resp = it->second;
            resp->run(begin, end);
        }
    }

    responder_map_t responder_map;
};

} /* namespace nova */

#endif /* OSC_RESPONDER_HPP */
