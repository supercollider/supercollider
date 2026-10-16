//  Copyright (C) 2011-2012 Tim Blechmann
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

#include <shared_mutex>
#include <string>

#include <boost/intrusive/unordered_set_hook.hpp>
#include <boost/unordered_set.hpp>

#include "utils.hpp"

#include "malloc_aligned.hpp"

namespace nova {
namespace bi = boost::intrusive;

PURE inline bool strequal(const char* lhs, const char* rhs) {
    for (;; ++lhs, ++rhs) {
        if (*lhs == 0) {
            if (*rhs == 0)
                return true;
            else
                return false;
        }
        if (*rhs == 0)
            return false;
        if (*lhs != *rhs)
            return false;
    }
}

// thread-safe symbol
// warning: don't use from different DSOs
struct symbol {
private:
    static const char* duplicate_string(const char* str, std::size_t length) {
        assert(strlen(str) == length);

        length += 1; // for terminating \0
        char* string = malloc_aligned<char>(length);

        strncpy(string, str, length);
        return string;
    }

    struct symbol_data {
        explicit symbol_data(const char* str): str(str), hash(string_hash(str)) {}

        explicit symbol_data(const char* str, size_t hash): str(str), hash(hash) {}

        symbol_data(symbol_data const& rhs) = default;
        symbol_data& operator=(symbol_data const& rhs) = default;

        friend size_t hash_value(symbol_data const& value) { return value.hash; }

        bool operator==(symbol_data const& rhs) const { return str == rhs.str; }

        const char* str;
        size_t hash;
    };

    struct hash_by_string {
        size_t operator()(const char* str) const { return string_hash(str); }
        size_t operator()(symbol_data const& data) const { return data.hash; }
    };

    struct equal_by_string {
        static const char* get_string(const char* arg) { return arg; }

        static const char* get_string(symbol_data const& arg) { return arg.str; }

        template <typename T1, typename T2> bool operator()(T1 const& lhs, T2 const& rhs) const {
            return strequal(get_string(lhs), get_string(rhs));
        }
    };

    class symbol_table {
        static constexpr size_t initial_table_size = 16384;

        boost::unordered_set<symbol_data> table;
        std::shared_mutex mutex;

        auto symbol_lookup(const char* str) const {
            auto it = table.find(str, hash_by_string(), equal_by_string());
            return std::make_pair(it, it != table.end());
        }

    public:
        symbol_table(): table(initial_table_size) {}

        symbol_data const& find(const char* str, size_t strlen) {
            // double-checked locking
            {
                std::shared_lock lock(mutex);
                if (auto [it, success] = symbol_lookup(str); success)
                    return *it;
            }

            std::lock_guard lock(mutex);
            if (auto [it, success] = symbol_lookup(str); success)
                return *it;

            auto [it, inserted] = table.emplace(duplicate_string(str, strlen));
            assert(inserted);
            return *it;
        }
    };

    symbol_data lookup_string(const char* str, std::size_t length) { return table.find(str, length); }

    symbol_data lookup_string(const char* str) { return lookup_string(str, strlen(str)); }

    inline static symbol_table table;

public:
    symbol() {}

    explicit symbol(const char* str): data(lookup_string(str)) {}

    explicit symbol(std::string const& str): data(lookup_string(str.c_str(), str.size())) {}

    symbol(const char* str, std::size_t length): data(lookup_string(str, length)) {}

    symbol(symbol const& rhs) = default;
    symbol& operator=(symbol const& rhs) = default;

    const char* c_str() const { return data.str; }

    friend std::size_t hash_value(symbol const& value) { return value.data.hash; }

    friend bool operator==(symbol const& lhs, symbol const& rhs) { return lhs.data == rhs.data; }

    friend bool operator<(symbol const& lhs, symbol const& rhs) { return lhs.data.str < rhs.data.str; }

    symbol_data data = symbol_data(nullptr, 0);
};

class named_hash_entry : public bi::unordered_set_base_hook<> {
    const symbol name_;

public:
    named_hash_entry(const char* name): name_(name) {}

    named_hash_entry(symbol const& name): name_(name) {}

    named_hash_entry(std::string const& name): name_(name.c_str(), name.size()) {}

    const char* name() const { return name_.c_str(); }

    friend std::size_t hash_value(named_hash_entry const& that) { return that.hash(); }

    std::size_t hash() const { return name_.data.hash; }

    friend bool operator==(named_hash_entry const& a, named_hash_entry const& b) { return a.name_ == b.name_; }
};

struct named_hash_equal {
    template <typename def> bool operator()(def const& lhs, std::string const& rhs) const {
        return operator()(lhs.name(), rhs.c_str());
    }

    template <typename def> bool operator()(std::string const& lhs, def const& rhs) const {
        return operator()(lhs.c_str(), rhs.name());
    }

    template <typename def> bool operator()(def const& lhs, symbol const& rhs) const {
        return operator()(lhs.name(), rhs.c_str());
    }

    template <typename def> bool operator()(symbol const& lhs, def const& rhs) const {
        return operator()(lhs.c_str(), rhs.name());
    }

    template <typename def> bool operator()(const char* lhs, def const& rhs) const {
        return operator()(lhs, rhs.name());
    }

    template <typename def> bool operator()(def const& lhs, const char* rhs) const {
        return operator()(lhs.name(), rhs);
    }

    bool operator()(const char* lhs, const char* rhs) const { return strequal(rhs, lhs); }
};

struct named_hash_hash {
    template <typename def> std::size_t operator()(def const& arg) { return hash_value(arg); }

    std::size_t operator()(const char* str) { return string_hash(str); }

    std::size_t operator()(std::string const& str) { return string_hash(str.c_str()); }
};

}
