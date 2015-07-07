/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <string>
#include "../json/value.hpp"
#include "../json/parser/grammar_def.hpp"
#include <boost/spirit/include/support_istream_iterator.hpp>

typedef std::string::const_iterator string_iter_t;

template struct ciere::json::parser::grammar<string_iter_t>;
template struct ciere::json::parser::grammar<boost::spirit::istream_iterator>;
