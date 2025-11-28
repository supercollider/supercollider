#include "../sc_gluon_v1_types.h"
#include <vector>
#include <array>
#include <cstring>
#include <stdexcept>

namespace sc_gluon::v1 {
using param_t = ::sc_gluon_param_v1_t;
using tag_t = ::sc_gluon_param_tag_v1;
using data_t = ::sc_gluon_data_v1;
using callback_obj_t = ::sc_gluon_callback_object_v1_t;
using callback_release_t = sc_gluon_release_callback_object_v1_f;
using do_callback_t = ::sc_gluon_do_callback_v1_f;

// MSVC doesn't support forward compatible constexpr union initialisation.
#if (_MSC_VER) && (_MSVC_LANG < 202002L)
#    define SC_GLUON_CONSTEXPR
#else
#    define SC_GLUON_CONSTEXPR constexpr
#endif

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t priv_basic(data_t data, tag_t tag) noexcept {
    return { data, 1, tag, false };
}
[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t priv_array(data_t data, tag_t tag, uint32_t size,
                                                           bool owns_data) noexcept {
    return { data, size, tag, owns_data };
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param() noexcept {
    data_t d {};
    d.nil_ = {};
    return priv_basic(d, sc_gluon_nil);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(int32_t i) noexcept {
    data_t d {};
    d.i32 = i;
    return priv_basic(d, sc_gluon_i32);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(double f) noexcept {
    data_t d {};
    d.f64 = f;
    return priv_basic(d, sc_gluon_f64);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(char c) noexcept {
    data_t d {};
    d.character = c;
    return priv_basic(d, sc_gluon_char);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(bool b) noexcept {
    data_t d {};
    d.boolean = b;
    return priv_basic(d, sc_gluon_bool);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(void* p) noexcept {
    data_t d {};
    d.raw_pointer = p;
    return priv_basic(d, sc_gluon_raw_pointer);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(uint64_t s) noexcept {
    data_t d {};
    d.symbol_value = s;
    return priv_basic(d, sc_gluon_symbol_value);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(uint8_t* a, uint32_t size, bool owns_data) noexcept {
    data_t d {};
    d.u8_array = a;
    return priv_array(d, sc_gluon_u8_array, size, owns_data ? 1 : 0);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(double* a, uint32_t size, bool owns_data) noexcept {
    data_t d {};
    d.f64_array = a;
    return priv_array(d, sc_gluon_f64_array, size, owns_data ? 1 : 0);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(float* a, uint32_t size, bool owns_data) noexcept {
    data_t d {};
    d.f32_array = a;
    return priv_array(d, sc_gluon_f32_array, size, owns_data ? 1 : 0);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(char* a, uint32_t size, bool owns_data) noexcept {
    data_t d {};
    d.character_array = a;
    return priv_array(d, sc_gluon_char_array, size, owns_data ? 1 : 0);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR param_t create_param(param_t* a, uint32_t size, bool owns_data) noexcept {
    data_t d {};
    d.param_array = a;
    return priv_array(d, sc_gluon_param_array, size, owns_data ? 1 : 0);
}

[[nodiscard]] inline SC_GLUON_CONSTEXPR bool has_heap_allocated_data(param_t p) noexcept {
    switch (p.tag) {
    case sc_gluon_u8_array:
    case sc_gluon_f64_array:
    case sc_gluon_f32_array:
    case sc_gluon_char_array:
    case sc_gluon_param_array:
        return true;
    default:
        return false;
    }
}

#undef SC_GLUON_CONSTEXPR

} // namespace sc_gluon::v1
