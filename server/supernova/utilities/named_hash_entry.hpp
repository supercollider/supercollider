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
#include <string_view>

#include <boost/intrusive/unordered_set_hook.hpp>
#include <boost/unordered_set.hpp>

#include "utils.hpp"

#include "malloc_aligned.hpp"

namespace nova {
namespace bi = boost::intrusive;

// thread-safe symbol
// warning: don't use from different DSOs
struct symbol {
private:
    static const char* duplicate_string(std::string_view str) {
        // add null-terminator!
        char* string = malloc_aligned<char>(str.size() + 1);
        memcpy(string, str.data(), str.size());
        string[str.size()] = '\0';

        return string;
    }

    struct symbol_data {
        symbol_data(): str(nullptr), size(0), hash(0) {}

        symbol_data(std::string_view str): symbol_data(str, string_hash(str)) {}

        symbol_data(std::string_view str, size_t hash): str(str.data()), size(str.size()), hash(hash) {}

        symbol_data(symbol_data const& rhs) = default;
        symbol_data& operator=(symbol_data const& rhs) = default;

        friend size_t hash_value(symbol_data const& value) { return value.hash; }

        bool operator==(symbol_data const& rhs) const { return str == rhs.str; }

        const char* str; // null-terminated!
        size_t size;
        size_t hash;
    };

    struct hash_by_string {
        size_t operator()(std::string_view str) const { return string_hash(str); }
        size_t operator()(symbol_data const& data) const { return data.hash; }
    };

    struct equal_by_string {
        template <typename T1, typename T2> bool operator()(T1 const& lhs, T2 const& rhs) const {
            return get_string(lhs) == get_string(rhs);
        }

    private:
        static std::string_view get_string(std::string_view arg) { return arg; }

        static std::string_view get_string(symbol_data const& arg) { return { arg.str, arg.size }; }
    };

    class symbol_table {
        static constexpr size_t initial_table_size = 16384;

        using table_type = boost::unordered_set<symbol_data>;
        // with C++20 we can just use auto as the return type.
        using lookup_result = std::pair<table_type::const_iterator, bool>;

    public:
        symbol_table(): table(initial_table_size) {}

        symbol_data const& find(std::string_view str) {
            // double-checked locking
            {
                std::shared_lock lock(mutex);
                if (auto [it, success] = symbol_lookup(str); success)
                    return *it;
            }

            std::lock_guard lock(mutex);
            if (auto [it, success] = symbol_lookup(str); success)
                return *it;

            auto [it, inserted] = table.emplace(duplicate_string(str));
            assert(inserted);
            return *it;
        }

    private:
        lookup_result symbol_lookup(std::string_view str) const {
            auto it = table.find(str, hash_by_string(), equal_by_string());
            return std::make_pair(it, it != table.end());
        }

        table_type table;
        std::shared_mutex mutex;
    };

    symbol_data lookup_string(std::string_view str) const {
        static symbol_table table;
        return table.find(str);
    }

public:
    symbol() {}

    explicit symbol(std::string_view str): data(lookup_string(str)) {}

    symbol(symbol const& rhs) = default;
    symbol& operator=(symbol const& rhs) = default;

    const char* c_str() const { return data.str; }

    size_t size() const { return data.size; }

    friend std::size_t hash_value(symbol const& value) { return value.data.hash; }

    friend bool operator==(symbol const& lhs, symbol const& rhs) { return lhs.data == rhs.data; }

    friend bool operator<(symbol const& lhs, symbol const& rhs) { return lhs.data.str < rhs.data.str; }

    symbol_data data;
};

class named_hash_entry : public bi::unordered_set_base_hook<> {
    const symbol name_;

public:
    explicit named_hash_entry(symbol const& name): name_(name) {}

    explicit named_hash_entry(std::string_view name): name_(name) {}

    const char* name() const { return name_.c_str(); }

    const symbol& name_symbol() const { return name_; }

    friend std::size_t hash_value(named_hash_entry const& that) { return that.hash(); }

    std::size_t hash() const { return name_.data.hash; }

    // compare by pointer equality!
    friend bool operator==(named_hash_entry const& a, named_hash_entry const& b) { return a.name_ == b.name_; }
};

struct named_hash_equal {
    template <typename T1, typename T2> bool operator()(T1 const& lhs, T2 const& rhs) const {
        return get_string(lhs) == get_string(rhs);
    }

private:
    static std::string_view get_string(symbol const& arg) { return { arg.c_str(), arg.size() }; }

    static std::string_view get_string(named_hash_entry const& arg) { return get_string(arg.name_symbol()); }

    static std::string_view get_string(std::string_view arg) { return arg; }
};

struct named_hash_hash {
    // look up hash value via ADL
    template <typename def> std::size_t operator()(def const& arg) { return hash_value(arg); }

    std::size_t operator()(std::string_view str) { return string_hash(str); }

    // need const char* overload
    std::size_t operator()(const char* str) { return string_hash(str); }
};

}
