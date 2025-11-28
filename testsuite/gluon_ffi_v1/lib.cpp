#include "sc_gluon_v1_types.h"
#include "sc_gluon_v1_entry_points.h"

#include "cpp/sc_gluon_v1_util.hpp"
#include <thread>
#include <iostream>
#include <chrono>

using namespace sc_gluon::v1;
struct LibraryData {
    sc_gluon_do_callback_v1_f callback_f;
    sc_gluon_release_callback_object_v1_f release_callback_f;
};

sc_gluon_out_param_tag_v1 add_f64(sc_gluon_library_data_v1_t library_data,
                                  sc_gluon_callback_object_v1_t maybe_callback_data, sc_gluon_param_v1_t* in_params,
                                  uint32_t num_in_params, sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    if (num_in_params != 2) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    const sc_gluon_param_v1_t& p1 = in_params[0];
    const sc_gluon_param_v1_t& p2 = in_params[1];

    if (p1.tag != sc_gluon_f64) {
        out_param->maybe_diagnostic = "param 1 is not an f64";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    if (p2.tag != sc_gluon_f64) {
        out_param->maybe_diagnostic = "param 2 is not an f64";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    const auto r = p1.data.f64 + p2.data.f64;

    out_param->out_param.data.f64 = r;
    out_param->out_param.tag = sc_gluon_f64;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}

sc_gluon_out_param_tag_v1 add_many_f64(sc_gluon_library_data_v1_t library_data,
                                       sc_gluon_callback_object_v1_t maybe_callback_data,
                                       sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                       sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    double rolling { 0.0 };

    for (size_t i { 0 }; i < num_in_params; ++i) {
        if (in_params[i].tag != sc_gluon_f64) {
            out_param->maybe_diagnostic = "all parameters should be doubles";
            return sc_gluon_error_with_non_owned_diagnostic;
        }

        rolling += in_params[i].data.f64;
    }

    out_param->out_param.data.f64 = rolling;
    out_param->out_param.tag = sc_gluon_f64;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}

sc_gluon_out_param_tag_v1 with_call_back(sc_gluon_library_data_v1_t library_data,
                                         sc_gluon_callback_object_v1_t maybe_callback_data,
                                         sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                         sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    auto* lib = static_cast<LibraryData*>(library_data);

    if (num_in_params != 1) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    std::thread([=, param_copy = sc_gluon_copy_param_v1(in_params[0])]() mutable {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        if (maybe_callback_data) {
            param_t ps[2] = { create_param(), sc_gluon_copy_param_v1(param_copy) };
            lib->callback_f(maybe_callback_data, ps, 2);
        }

        std::this_thread::sleep_for(1s);

        if (maybe_callback_data) {
            param_t ps[2] = { create_param(), sc_gluon_copy_param_v1(param_copy) };
            lib->callback_f(maybe_callback_data, ps, 2);
        }

        sc_gluon_free_param_if_owned_v1(param_copy);

        if (maybe_callback_data)
            lib->release_callback_f(maybe_callback_data);

        return;
    }).detach();

    out_param->out_param = create_param(1);
    return sc_gluon_produced_param;
}


SC_GLUON_EXPORT uint32_t sc_gluon_version() { return 1; }

std::array<sc_gluon_function_declarations_v1_t, 3> decls {
    sc_gluon_function_declarations_v1_t { "add_f64", add_f64, 2, 0 },
    sc_gluon_function_declarations_v1_t { "add_many_f64", add_many_f64, -1, 0 },
    sc_gluon_function_declarations_v1_t { "with_call_back", with_call_back, 1, 1 },
};


SC_GLUON_EXPORT uint8_t sc_gluon_load_library(sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                              sc_gluon_do_callback_v1_f do_callback,
                                              sc_gluon_release_callback_object_v1_f release_callback,
                                              sc_gluon_function_declarations_v1_t** const out_decls,
                                              uint32_t* out_decls_size, sc_gluon_library_data_v1_t* out_library_data) {
    *out_decls = decls.data();
    *out_decls_size = decls.size();
    *out_library_data = new LibraryData { do_callback, release_callback };
    return 0;
}
