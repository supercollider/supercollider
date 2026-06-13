#pragma once

#include <memory>

namespace nova { namespace detail {

struct nova_endpoint : public std::enable_shared_from_this<nova_endpoint> {
    virtual void send(const char* data, size_t length) = 0;
};

typedef std::shared_ptr<nova_endpoint> endpoint_ptr;

} /* namespace detail */ } /* namespace nova */
