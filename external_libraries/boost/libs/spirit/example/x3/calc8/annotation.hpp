/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_ANNOTATION_HPP)
#define BOOST_SPIRIT_X3_CALC8_ANNOTATION_HPP

#include <map>
#include "ast.hpp"
#include <boost/spirit/home/x3/support/utility/lambda_visitor.hpp>

namespace client
{
    // tag used to get our error handler from the context
    struct error_handler_tag;
}

namespace client { namespace parser
{
    struct annotation_base
    {
        ////////////////////////////////////////////////////////////////////////
        //  The on_success handler links the AST to a vector of iterator
        //  positions for the purpose of subsequent semantic error handling
        //  when the program is being compiled. See x3::position_cache in
        //  x3/support/ast
        ////////////////////////////////////////////////////////////////////////

        template <typename Iterator, typename Context>
        inline void
        on_success(Iterator const& first, Iterator const& last
          , ast::operand& ast, Context const& context)
        {
            auto& error_handler = x3::get<error_handler_tag>(context).get();
            auto annotate = [&](auto& node)
            {
                error_handler.tag(node, first, last);
            };
            ast.apply_visitor(x3::make_lambda_visitor<void>(annotate));
        }

        template <typename T, typename Iterator, typename Context>
        inline void
        on_success(Iterator const& first, Iterator const& last
          , T& ast, Context const& context)
        {
            auto& error_handler = x3::get<error_handler_tag>(context).get();
            error_handler.tag(ast, first, last);
        }
    };
}}

#endif
