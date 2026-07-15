
#pragma once

namespace sc::util {

template <typename... Ts> struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts> overload(Ts...) -> overload<Ts...>;

}


