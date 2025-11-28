// Author Jordan Henderson - JordanHendersonMusic
#pragma once
#include <memory>
#include <limits>
#include <unordered_map>
#include <optional>
#include <vector>
#include <variant>

#include "sc_gluon_v1_types.h"

#include "PyrSlot.h"

struct PyrObject;
struct VMGlobals;
struct PyrSymbol;

namespace sc_gluon {

using LibraryID = int32_t;

namespace details {
struct FunctionDataV1 {
    sc_gluon_function_v1_f ptr;
    int32_t num_params;
    bool accepts_callback;
    bool accepts_variable_params() const { return num_params < 0; }
};
struct LibraryV1 {
    void* library_handle { nullptr };
    sc_gluon_library_data_v1_t library_data { nullptr };
    sc_gluon_unload_library_v1_f unloader { nullptr };

    std::vector<PyrSymbol*> function_names {};
    std::vector<FunctionDataV1> function_data {};

    void evaluate(PyrSymbol* function_name, VMGlobals* g, PyrObject* maybe_callback, PyrSlot* return_slot,
                  PyrSlot* first_argument_slot, int num_args_given) const;

    void* prepare_for_unload_return_handle() const;
};
}

class GluonManager {
public:
    GluonManager() = default;
    GluonManager(GluonManager&&) noexcept = delete;
    GluonManager& operator=(GluonManager&&) noexcept = delete;
    GluonManager(const GluonManager&) = delete;
    GluonManager& operator=(const GluonManager&) = delete;

    void reset_or_prep_for_close();
    void create_testing_library();

    // Callback only works if the ffi functions expects a callback.
    void evaluate_function(LibraryID library_id, PyrSymbol* function_name, VMGlobals* g, PyrObject* maybe_callback,
                           PyrSlot* return_slot, PyrSlot* first_argument_slot, int num_args_given) const
        noexcept(false);

    [[nodiscard]] LibraryID register_library(const char* path, PyrSlot* first_parameter_slot,
                                             int num_parameters) noexcept(false);
    void unregister_library(LibraryID lib) noexcept(false) { unregister_library_maybe_close(lib, true); };

    [[nodiscard]] LibraryID get_inbuilt_library_id(PyrSymbol*) const noexcept(false);

private: // create tests for all versions
    void create_testing_library_v1();

private: // register libary versions
    [[nodiscard]] LibraryID register_library_v1(void* library, PyrSlot* first_parameter_slot,
                                                int num_parameters) noexcept(false);

private: // helpers
    void unregister_library_maybe_close(LibraryID, bool) noexcept(false);

private: // private members
    using LibraryVariant = std::variant<details::LibraryV1>;
    std::unordered_map<LibraryID, LibraryVariant> libraries {};
    std::unordered_map<PyrSymbol*, LibraryID> inbuilt_test_to_library_id;
    LibraryID library_counter = std::numeric_limits<LibraryID>::min();
};

}
