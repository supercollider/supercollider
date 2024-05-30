//
// detail/initiate_post.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_INITIATE_POST_HPP
#define BOOST_ASIO_DETAIL_INITIATE_POST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/detail/work_dispatcher.hpp>
#include <boost/asio/execution/allocator.hpp>
#include <boost/asio/execution/blocking.hpp>
#include <boost/asio/execution/relationship.hpp>
#include <boost/asio/prefer.hpp>
#include <boost/asio/require.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {

class initiate_post
{
public:
  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        execution::is_executor<
          typename associated_executor<
            typename decay<CompletionHandler>::type
          >::type
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typename associated_executor<handler_t>::type ex(
        (get_associated_executor)(handler));

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

#if defined(BOOST_ASIO_NO_DEPRECATED)
    boost::asio::prefer(
        boost::asio::require(ex, execution::blocking.never),
        execution::relationship.fork,
        execution::allocator(alloc)
      ).execute(
        boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)));
#else // defined(BOOST_ASIO_NO_DEPRECATED)
    execution::execute(
        boost::asio::prefer(
          boost::asio::require(ex, execution::blocking.never),
          execution::relationship.fork,
          execution::allocator(alloc)),
        boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)));
#endif // defined(BOOST_ASIO_NO_DEPRECATED)
  }

  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        !execution::is_executor<
          typename associated_executor<
            typename decay<CompletionHandler>::type
          >::type
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typename associated_executor<handler_t>::type ex(
        (get_associated_executor)(handler));

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

    ex.post(boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)), alloc);
  }
};

template <typename Executor>
class initiate_post_with_executor
{
public:
  typedef Executor executor_type;

  explicit initiate_post_with_executor(const Executor& ex)
    : ex_(ex)
  {
  }

  executor_type get_executor() const BOOST_ASIO_NOEXCEPT
  {
    return ex_;
  }

  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        execution::is_executor<
          typename conditional<true, executor_type, CompletionHandler>::type
        >::value
      >::type* = 0,
      typename enable_if<
        !detail::is_work_dispatcher_required<
          typename decay<CompletionHandler>::type,
          Executor
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

#if defined(BOOST_ASIO_NO_DEPRECATED)
    boost::asio::prefer(
        boost::asio::require(ex_, execution::blocking.never),
        execution::relationship.fork,
        execution::allocator(alloc)
      ).execute(
        boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)));
#else // defined(BOOST_ASIO_NO_DEPRECATED)
    execution::execute(
        boost::asio::prefer(
          boost::asio::require(ex_, execution::blocking.never),
          execution::relationship.fork,
          execution::allocator(alloc)),
        boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)));
#endif // defined(BOOST_ASIO_NO_DEPRECATED)
  }

  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        execution::is_executor<
          typename conditional<true, executor_type, CompletionHandler>::type
        >::value
      >::type* = 0,
      typename enable_if<
        detail::is_work_dispatcher_required<
          typename decay<CompletionHandler>::type,
          Executor
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typedef typename associated_executor<
      handler_t, Executor>::type handler_ex_t;
    handler_ex_t handler_ex((get_associated_executor)(handler, ex_));

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

#if defined(BOOST_ASIO_NO_DEPRECATED)
    boost::asio::prefer(
        boost::asio::require(ex_, execution::blocking.never),
        execution::relationship.fork,
        execution::allocator(alloc)
      ).execute(
        detail::work_dispatcher<handler_t, handler_ex_t>(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler), handler_ex));
#else // defined(BOOST_ASIO_NO_DEPRECATED)
    execution::execute(
        boost::asio::prefer(
          boost::asio::require(ex_, execution::blocking.never),
          execution::relationship.fork,
          execution::allocator(alloc)),
        detail::work_dispatcher<handler_t, handler_ex_t>(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler), handler_ex));
#endif // defined(BOOST_ASIO_NO_DEPRECATED)
  }

  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        !execution::is_executor<
          typename conditional<true, executor_type, CompletionHandler>::type
        >::value
      >::type* = 0,
      typename enable_if<
        !detail::is_work_dispatcher_required<
          typename decay<CompletionHandler>::type,
          Executor
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

    ex_.post(boost::asio::detail::bind_handler(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler)), alloc);
  }

  template <typename CompletionHandler>
  void operator()(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler,
      typename enable_if<
        !execution::is_executor<
          typename conditional<true, executor_type, CompletionHandler>::type
        >::value
      >::type* = 0,
      typename enable_if<
        detail::is_work_dispatcher_required<
          typename decay<CompletionHandler>::type,
          Executor
        >::value
      >::type* = 0) const
  {
    typedef typename decay<CompletionHandler>::type handler_t;

    typedef typename associated_executor<
      handler_t, Executor>::type handler_ex_t;
    handler_ex_t handler_ex((get_associated_executor)(handler, ex_));

    typename associated_allocator<handler_t>::type alloc(
        (get_associated_allocator)(handler));

    ex_.post(detail::work_dispatcher<handler_t, handler_ex_t>(
          BOOST_ASIO_MOVE_CAST(CompletionHandler)(handler),
          handler_ex), alloc);
  }

private:
  Executor ex_;
};

} // namespace detail
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_INITIATE_POST_HPP
