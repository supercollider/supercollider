#include <cstring>
#include <thread>
#include <chrono>
#include <filesystem>
#include <iostream>

#include "GluonFFI.hpp"
#include "VMGlobals.h"
#include "GC.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrSignal.h"
#include "PyrSched.h"
#include "PyrInterpreter.h"
#include "PyrSlot.h"
#include "PyrSymbolTable.h"

#include "cpp/sc_gluon_v1_util.hpp"


#ifdef _WIN32
#    include "SC_Win32Utils.h"
#    include "SC_Codecvt.hpp"
#else
#    include <dlfcn.h>
#endif

using namespace sc_gluon;

sc_gluon_param_v1_t slot_to_param(PyrSlot slot);
PyrSlot move_param_to_slot(sc_gluon_param_v1_t&& param, VMGlobals* g) noexcept;

void do_callback(void* callback_data, sc_gluon_param_v1_t* params, uint32_t num_params) {
    assert(num_params > 0 ? (params != nullptr) : params == nullptr);
    if (callback_data == nullptr)
        return;

    extern bool compiledOK;

    using namespace std::chrono_literals;
    const bool locked = gLangMutex.try_lock_for(10s);
    if (locked) {
        VMGlobals* g { gMainVMGlobals };
        auto* obj { reinterpret_cast<PyrObject*>(callback_data) };
        if (compiledOK) {
            ++g->sp;
            SetObject(g->sp, obj);
            for (uint32_t i { 0 }; i < num_params; ++i) {
                PyrSlot slot = move_param_to_slot(std::move(params[i]), g);
                ++g->sp;
                *g->sp = slot;
            }
            runInterpreter(g, s_value, 1 + num_params);
        }
        gLangMutex.unlock();
    } else {
        std::cerr << "Gluon ffi library could not establish language lock, ensure callbacks are only triggered on a "
                     "separate thread to the main callback function.";
    }
}

void release_callback(void* callback_data) {
    extern bool compiledOK;

    using namespace std::chrono_literals;
    const bool locked = gLangMutex.try_lock_for(10s);
    if (locked) {
        VMGlobals* g { gMainVMGlobals };
        auto* obj { reinterpret_cast<PyrObject*>(callback_data) };
        g->gc->RemoveExternalObject(obj);
        gLangMutex.unlock();
    } else {
        std::cerr << "Gluon ffi library could not establish language lock, ensure callbacks are only release on a "
                     "separate thread to the main callback function.";
    }
}

sc_gluon_param_v1_t slot_to_param(PyrSlot slot) {
    if (IsNil(&slot)) {
        return v1::create_param();
    } else if (IsInt(&slot)) {
        return v1::create_param(slotRawInt(&slot));
    } else if (IsFloat(&slot)) {
        return v1::create_param(slotRawFloat(&slot));
    } else if (IsChar(&slot)) {
        return v1::create_param(static_cast<char>(slotRawChar(&slot)));
    } else if (IsPtr(&slot)) {
        return v1::create_param(slotRawPtr(&slot));
    } else if (IsTrue(&slot)) {
        return v1::create_param(true);
    } else if (IsFalse(&slot)) {
        return v1::create_param(false);
    } else if (IsSym(&slot)) {
        const auto i = reinterpret_cast<intptr_t>(slotRawSymbol(&slot));
        return v1::create_param(static_cast<uint64_t>(i));
    } else if (IsObj(&slot)) {
        PyrObject* obj { slotRawObject(&slot) };
        if (obj->classptr == class_string) {
            if (obj->IsImmutable()) {
                auto* str = reinterpret_cast<PyrString*>(obj);
                return v1::create_param(str->s, static_cast<uint32_t>(str->size), false);
            } else {
                auto* str = reinterpret_cast<PyrString*>(obj);
                char* ptr = (char*)malloc(str->size * sizeof(char));
                std::memcpy(ptr, str->s, str->size * sizeof(char));
                return v1::create_param(ptr, static_cast<uint32_t>(str->size), true);
            }
        } else if (obj->classptr == class_int8array) {
            auto* u8array = reinterpret_cast<PyrInt8Array*>(obj);
            return v1::create_param(u8array->b, static_cast<uint32_t>(u8array->size), false);
        } else if (obj->classptr == class_doublearray) {
            auto* f64array = reinterpret_cast<PyrDoubleArray*>(obj);
            return v1::create_param(f64array->d, static_cast<uint32_t>(f64array->size), false);
        } else if (obj->classptr == class_floatarray) {
            auto* f32array = reinterpret_cast<PyrFloatArray*>(obj);
            return v1::create_param(f32array->f, static_cast<uint32_t>(f32array->size), false);
        } else if (obj->classptr == class_array) {
            sc_gluon_param_v1_t* param_array = (sc_gluon_param_v1_t*)malloc(sizeof(sc_gluon_param_v1_t) * obj->size);
            if (param_array == nullptr)
                throw std::runtime_error { "malloc failed" };

            try {
                for (int i { 0 }; i < obj->size; ++i) {
                    param_array[i] = slot_to_param(obj->slots[i]);
                }
            } catch (...) {
                free(param_array);
                throw;
            }

            return v1::create_param(param_array, static_cast<uint32_t>(obj->size), true);
        } else {
            throw std::runtime_error { "Cannot convert slot to sc_gluon_param_v1_t" };
        }
    } else {
        throw std::runtime_error { "Cannot convert slot to sc_gluon_param_v1_t" };
    }
}

PyrSlot move_param_to_slot(sc_gluon_param_v1_t&& param, VMGlobals* g) noexcept {
    PyrSlot out;
    switch (param.tag) {
    case sc_gluon_nil: {
        SetNil(&out);
        break;
    }
    case sc_gluon_i32: {
        SetInt(&out, param.data.i32);
        break;
    }
    case sc_gluon_f64: {
        SetFloat(&out, param.data.f64);
        break;
    }
    case sc_gluon_char: {
        SetChar(&out, param.data.character);
        break;
    }
    case sc_gluon_bool: {
        if (param.data.boolean)
            SetTrue(&out);
        else
            SetFalse(&out);
        break;
    }
    case sc_gluon_raw_pointer: {
        SetPtr(&out, param.data.raw_pointer);
        break;
    }
    case sc_gluon_symbol_value: {
        const auto v = static_cast<intptr_t>(param.data.symbol_value);
        SetSymbol(&out, reinterpret_cast<PyrSymbol*>(v));
        break;
    }
    case sc_gluon_u8_array: {
        auto new_array = newPyrInt8Array(g->gc, param.size, 0, false);
        std::memcpy(new_array->b, param.data.u8_array, param.size * sizeof(uint8_t));
        if (param.owns_data)
            free(param.data.u8_array);
        SetObject(&out, new_array);
        break;
    }

    case sc_gluon_f64_array: {
        auto new_array = newPyrDoubleArray(g->gc, param.size, 0, false);
        std::memcpy(new_array->d, param.data.f64_array, param.size * sizeof(double));
        if (param.owns_data)
            free(param.data.f64_array);
        SetObject(&out, new_array);
        break;
    }
    case sc_gluon_f32_array: {
        auto new_array = newPyrSignal(g, param.size, false);
        std::memcpy((float*)new_array->slots, param.data.f32_array, param.size * sizeof(double));
        if (param.owns_data)
            free(param.data.f32_array);
        SetObject(&out, new_array);
        break;
    }
    case sc_gluon_char_array: {
        auto new_array = newPyrString(g->gc, param.data.character_array, 0, false);
        if (param.owns_data)
            free(param.data.character_array);
        SetObject(&out, new_array);
        break;
    }
    case sc_gluon_param_array: {
        auto array = newPyrArray(g->gc, param.size, 0, false);
        for (uint32_t i { 0 }; i < param.size; ++i)
            array->slots[i] = move_param_to_slot(std::move(param.data.param_array[i]), g);
        array->size = param.size;
        if (param.owns_data)
            free(param.data.param_array);
        SetObject(&out, array);
        break;
    }
    }
    return out;
}

LibraryID sc_gluon::GluonManager::register_library_v1(void* library_handle, PyrSlot* first_parameter_slot,
                                                      int num_parameters) noexcept(false) {
#ifndef _WIN32
    const void* loader_f = dlsym(library_handle, "sc_gluon_load_library");
    const char* load_error = dlerror();
    if (load_error != nullptr || loader_f == nullptr) {
        dlclose(library_handle);
        throw std::runtime_error { std::string { load_error } };
    }

    const void* unloader_f = dlsym(library_handle, "sc_gluon_unload_library");
    const void* post_load_p = dlsym(library_handle, "sc_gluon_post_load_library");

    const auto close_library = [=]() { dlclose(library_handle); };
#else
    const void* loader_f = (void*)GetProcAddress((HMODULE)library_handle, "sc_gluon_load_library");
    if (loader_f == nullptr) {
        FreeLibrary((HMODULE)library_handle);
        throw std::runtime_error { "Could not load sc_gluon_load_library" };
    }

    const void* unloader_f = (void*)GetProcAddress((HMODULE)library_handle, "sc_gluon_unload_library");
    const void* post_load_p = (void*)GetProcAddress((HMODULE)library_handle, "sc_gluon_post_load_library");

    const auto close_library = [=]() { FreeLibrary((HMODULE)library_handle); };

#endif
    const auto loader = (sc_gluon_load_library_v1_f)loader_f;
    const sc_gluon_unload_library_v1_f unloader = (sc_gluon_unload_library_v1_f)unloader_f;
    const sc_gluon_post_load_library_v1_f post_loader = (sc_gluon_post_load_library_v1_f)post_load_p;

    sc_gluon_function_declarations_v1_t* func_decls; // uninitialised
    uint32_t func_decls_size { 0 };

    std::vector<sc_gluon_param_v1_t> params;
    params.reserve(num_parameters);

    const auto destroy_params = [](std::vector<sc_gluon_param_v1_t>&& ps) {
        for (sc_gluon_param_v1_t p : ps)
            if (p.owns_data)
                sc_gluon_free_param_if_owned_v1(p);
    };

    for (uint32_t i { 0 }; i < (num_parameters); ++i) {
        params.push_back(slot_to_param(*first_parameter_slot));
        ++first_parameter_slot;
    }

    sc_gluon_library_data_v1_t library_data;
    const auto error_code = loader(params.data(), static_cast<uint32_t>(params.size()), do_callback, release_callback,
                                   &func_decls, &func_decls_size, &library_data);

    destroy_params(std::move(params));

    if (error_code) {
        close_library();
        throw std::runtime_error { "Failed to load functions from library" };
    }

    std::vector<PyrSymbol*> function_names;
    std::vector<details::FunctionDataV1> function_data;

    for (uint32_t i { 0 }; i < func_decls_size; ++i) {
        const auto decl = func_decls[i];
        function_names.push_back(getsym(decl.name));
        function_data.emplace_back(
            details::FunctionDataV1 { decl.ptr, decl.num_parms, static_cast<bool>(decl.accepts_callback) });
    }

    if (post_loader) {
        post_loader(library_data, func_decls, func_decls_size);
    }

    const LibraryID library_id = library_counter++;
    libraries.emplace(library_id,
                      LibraryVariant { details::LibraryV1 { library_handle, library_data, unloader,
                                                            std::move(function_names), std::move(function_data) } });

    return library_id;
}


void details::LibraryV1::evaluate(PyrSymbol* function_name, VMGlobals* g, PyrObject* maybe_callback,
                                  PyrSlot* return_slot, PyrSlot* first_argument_slot, int num_args_given) const {
    size_t func_index { std::numeric_limits<size_t>::max() };
    for (size_t i { 0 }; i < function_names.size(); ++i) {
        if (function_names[i] == function_name) {
            func_index = i;
            break;
        }
    }
    if (func_index == std::numeric_limits<size_t>::max()) {
        throw std::runtime_error { "Could not find function name in library" };
    }

    const FunctionDataV1& data = function_data[func_index];
    if (!data.accepts_variable_params() && num_args_given != data.num_params) {
        throw std::runtime_error { "Incorrect number of arguments given to FFI function." };
    }

    std::vector<sc_gluon_param_v1_t> params;
    params.reserve(num_args_given);

    const auto destroy_params = [](std::vector<sc_gluon_param_v1_t>&& ps) {
        for (sc_gluon_param_v1_t p : ps)
            if (p.owns_data)
                sc_gluon_free_param_if_owned_v1(p);
    };

    for (uint32_t i { 0 }; i < (num_args_given); ++i) {
        params.push_back(slot_to_param(*first_argument_slot));
        ++first_argument_slot;
    }

    if (maybe_callback != nullptr) {
        if (!data.accepts_callback) {
            destroy_params(std::move(params));
            throw std::runtime_error { "A callback was given to an FFI function that doesn't accepts callbacks." };
        }
        g->gc->StoreExternalObject(maybe_callback);
    }

    sc_gluon_out_param_or_maybe_diagnostic_v1 result;
    sc_gluon_out_param_tag_v1 result_tag =
        data.ptr(library_data, (void*)maybe_callback, params.data(), num_args_given, &result);

    destroy_params(std::move(params));

    switch (result_tag) {
    case sc_gluon_produced_param: {
        *return_slot = move_param_to_slot(std::move(result.out_param), g);
        break;
    }
    case sc_gluon_error_with_owned_diagnostic: {
        SetNil(return_slot);
        std::string error { result.maybe_diagnostic };
        free((void*)result.maybe_diagnostic);
        throw std::runtime_error { error };
    }
    case sc_gluon_error_with_non_owned_diagnostic: {
        SetNil(return_slot);
        throw std::runtime_error { std::string { result.maybe_diagnostic } };
    }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TESTING

sc_gluon_out_param_tag_v1 test1(sc_gluon_library_data_v1_t library_data,
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

sc_gluon_out_param_tag_v1 callback_test(sc_gluon_library_data_v1_t library_data,
                                        sc_gluon_callback_object_v1_t maybe_callback_data,
                                        sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                        sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    if (num_in_params != 1) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }
    const sc_gluon_param_v1_t& p1 = in_params[0];

    double wait;
    if (p1.tag == sc_gluon_f64) {
        wait = p1.data.f64;
    } else if (p1.tag == sc_gluon_i32) {
        wait = static_cast<double>(p1.data.i32);
    } else {
        out_param->maybe_diagnostic = "Expected an f64 or i32 for the first argument";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::duration<double>(wait));
        if (maybe_callback_data) {
            do_callback(maybe_callback_data, nullptr, 0);
            release_callback(maybe_callback_data);
        }
    }).detach();


    out_param->out_param.data.nil_ = {};
    out_param->out_param.tag = sc_gluon_nil;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}

sc_gluon_out_param_tag_v1 many_callback_test(sc_gluon_library_data_v1_t library_data,
                                             sc_gluon_callback_object_v1_t maybe_callback_data,
                                             sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                             sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    if (num_in_params != 2) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }
    const sc_gluon_param_v1_t& time_param = in_params[0];
    const sc_gluon_param_v1_t& iteration_param = in_params[1];

    double wait;
    if (time_param.tag == sc_gluon_f64) {
        wait = time_param.data.f64;
    } else if (time_param.tag == sc_gluon_i32) {
        wait = static_cast<double>(time_param.data.i32);
    } else {
        out_param->maybe_diagnostic = "Expected an f64 or i32 for the first argument";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    if (iteration_param.tag != sc_gluon_i32) {
        out_param->maybe_diagnostic = "Expected an i32 for the second argument";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    const int32_t iter = iteration_param.data.i32;

    std::thread([=]() {
        for (size_t i { 0 }; i < iter; ++i) {
            std::this_thread::sleep_for(std::chrono::duration<double>(wait));
            if (maybe_callback_data) {
                do_callback(maybe_callback_data, nullptr, 0);
            }
        }
        release_callback(maybe_callback_data);
    }).detach();


    out_param->out_param.data.boolean = true;
    out_param->out_param.tag = sc_gluon_bool;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}


sc_gluon_out_param_tag_v1 callback_with_args_test(sc_gluon_library_data_v1_t library_data,
                                                  sc_gluon_callback_object_v1_t maybe_callback_data,
                                                  sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                                  sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    if (num_in_params != 1) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }
    const sc_gluon_param_v1_t& p1 = in_params[0];

    double wait;
    if (p1.tag == sc_gluon_f64) {
        wait = p1.data.f64;
    } else if (p1.tag == sc_gluon_i32) {
        wait = static_cast<double>(p1.data.i32);
    } else {
        out_param->maybe_diagnostic = "Expected an f64 or i32 for the first argument";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::duration<double>(wait));
        sc_gluon_param_v1_t ps[2];
        ps[0].data.f64 = 2.1;
        ps[0].owns_data = false;
        ps[0].size = 1;
        ps[0].tag = sc_gluon_f64;

        ps[1].data.boolean = false;
        ps[1].owns_data = false;
        ps[1].size = 1;
        ps[1].tag = sc_gluon_bool;
        if (maybe_callback_data) {
            do_callback(maybe_callback_data, ps, 2);
            release_callback(maybe_callback_data);
        }
    }).detach();


    out_param->out_param.data.nil_ = {};
    out_param->out_param.tag = sc_gluon_nil;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}

sc_gluon_out_param_tag_v1 array_sum(sc_gluon_library_data_v1_t library_data,
                                    sc_gluon_callback_object_v1_t maybe_callback_data, sc_gluon_param_v1_t* in_params,
                                    uint32_t num_in_params, sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    if (num_in_params != 1) {
        out_param->maybe_diagnostic = "wrong number of in params";
        return sc_gluon_error_with_non_owned_diagnostic;
    }
    const sc_gluon_param_v1_t& p1 = in_params[0];
    if (p1.tag != sc_gluon_param_array) {
        out_param->maybe_diagnostic = "Wrong parameter type expected a param array.";
        return sc_gluon_error_with_non_owned_diagnostic;
    }

    const auto size = p1.size;

    double sum { 0 };
    for (size_t i { 0 }; i < size; ++i) {
        const sc_gluon_param_v1_t& e = p1.data.param_array[i];
        if (e.tag == sc_gluon_f64) {
            sum += e.data.f64;
        } else if (e.tag == sc_gluon_i32) {
            sum += static_cast<double>(e.data.i32);
        } else {
            out_param->maybe_diagnostic = "Wrong parameter type inside of array, expected an f64";
            return sc_gluon_error_with_non_owned_diagnostic;
        }
    }

    out_param->out_param.data.f64 = sum;
    out_param->out_param.tag = sc_gluon_f64;
    out_param->out_param.owns_data = false;
    out_param->out_param.size = 1;

    return sc_gluon_produced_param;
}

sc_gluon_out_param_tag_v1 return_array_test(sc_gluon_library_data_v1_t library_data,
                                            sc_gluon_callback_object_v1_t maybe_callback_data,
                                            sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                            sc_gluon_out_param_or_maybe_diagnostic_v1* out_param) {
    out_param->out_param.data.param_array = new sc_gluon_param_v1_t[10] {};
    out_param->out_param.tag = sc_gluon_param_array;
    out_param->out_param.owns_data = true;
    out_param->out_param.size = 10;

    for (size_t i { 0 }; i < 10; ++i) {
        out_param->out_param.data.param_array[i].data.i32 = static_cast<int32_t>(i);
        out_param->out_param.data.param_array[i].owns_data = false;
        out_param->out_param.data.param_array[i].size = 1;
        out_param->out_param.data.param_array[i].tag = sc_gluon_i32;
    }

    return sc_gluon_produced_param;
}


void sc_gluon::GluonManager::create_testing_library_v1() {
    details::LibraryV1 lib {};

    lib.function_names.push_back(getsym("addition_test"));
    lib.function_data.emplace_back(details::FunctionDataV1 { test1, 2, false });

    lib.function_names.push_back(getsym("callback_test"));
    lib.function_data.emplace_back(details::FunctionDataV1 { callback_test, 1, true });

    lib.function_names.push_back(getsym("many_callback_test"));
    lib.function_data.emplace_back(details::FunctionDataV1 { many_callback_test, 2, true });

    lib.function_names.push_back(getsym("callback_with_args_test"));
    lib.function_data.emplace_back(details::FunctionDataV1 { callback_with_args_test, 1, true });

    lib.function_names.push_back(getsym("array_sum"));
    lib.function_data.emplace_back(details::FunctionDataV1 { array_sum, 1, false });

    lib.function_names.push_back(getsym("return_array"));
    lib.function_data.emplace_back(details::FunctionDataV1 { return_array_test, 0, false });

    const auto test_id { library_counter++ };

    libraries.emplace(test_id, LibraryVariant { std::move(lib) });
    inbuilt_test_to_library_id.emplace(getsym("gluonTestV1"), test_id);
}
