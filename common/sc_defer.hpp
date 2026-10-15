#pragma once

#include <utility>

template <typename T> struct defer {
    defer(T&& t): t(std::move(t)) {}
    defer(defer&&) = delete;
    defer(const defer&) = delete;
    defer& operator=(defer&&) = delete;
    defer& operator=(const defer&) = delete;
    ~defer() { t(); }

private:
    T t;
};