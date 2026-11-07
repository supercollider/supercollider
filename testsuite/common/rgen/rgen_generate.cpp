#include <iostream>
#include <iomanip>
#include <fstream>
#include <functional>

#include "SC_RGen.h"
#include "SC_Version.hpp"

/// @attention Whenever you make changes to this file, you must regenerate the
/// rgen_output_<version>.hpp headers. See the README.md for instructions.

// Unfortunately, SC does not provide the version number as preprocessor defines,
// so we have to pass them to the compiler. This is done automatically by CMake.
#define CHECK_MIN_VERSION(major, minor, patch)                                                                         \
    (SC_VERSION_MAJOR >= major && SC_VERSION_MINOR >= minor && SC_VERSION_PATCH >= patch)

// Make sure that the defines match the constants in SC_Version.hpp.
static_assert(SC_VERSION_MAJOR == SC_VersionMajor, "wrong major version");
static_assert(SC_VERSION_MINOR == SC_VersionMinor, "wrong minor version");
static_assert(SC_VERSION_PATCH == SC_VersionPatch, "wrong patch version");

static constexpr uint32 seed = 1000;
static constexpr double scale = 10;
static constexpr double low = 1;
static constexpr double high = 10;
static constexpr size_t sample_count = 64;
static constexpr size_t samples_per_line = 8;

template <typename Func> std::string array_decl() { return "std::array<void, " + std::to_string(sample_count) + ">"; }

template <> std::string array_decl<uint32_t>() { return "std::array<uint32_t, " + std::to_string(sample_count) + ">"; }

template <> std::string array_decl<int32_t>() { return "std::array<int32_t, " + std::to_string(sample_count) + ">"; }

template <> std::string array_decl<float>() { return "std::array<float, " + std::to_string(sample_count) + ">"; }

template <> std::string array_decl<double>() { return "std::array<double, " + std::to_string(sample_count) + ">"; }

std::string version_namespace() {
    std::stringstream ss;
    ss << "sc_" << SC_VersionMajor << "_" << SC_VersionMinor << "_" << SC_VersionPatch;
    return ss.str();
}

template <typename Func> void gen_free_function(std::ostream& ofs, const char* name, Func func) {
    using T = std::invoke_result_t<Func, uint32_t&, uint32_t&, uint32_t&>;

    if constexpr (std::is_floating_point_v<T>) {
        constexpr auto max_precision = std::numeric_limits<T>::max_digits10;
        ofs << std::setprecision(max_precision);
    }

    RGen rgen;
    rgen.init(seed);

    ofs << "// " << name << "()\n";
    ofs << array_decl<T>() << " " << name << "_output = {";
    for (size_t i = 0; i < sample_count; ++i) {
        if ((i % samples_per_line) == 0)
            ofs << "\n    ";
        ofs << func(rgen.s1, rgen.s2, rgen.s3);
        if (i != sample_count - 1)
            ofs << ", ";
    }
    ofs << "\n};\n\n";
}

template <typename Func, typename... Args>
void gen_member_function(std::ostream& ofs, const char* name, Func memfn, Args... args) {
    using T = std::invoke_result_t<Func, RGen, Args...>;

    if constexpr (std::is_floating_point_v<T>) {
        constexpr auto max_precision = std::numeric_limits<T>::max_digits10;
        ofs << std::setprecision(max_precision);
    }

    RGen rgen;
    rgen.init(seed);

    ofs << "// RGen::" << name << "()\n";
    ofs << array_decl<T>() << " rgen_" << name << "_output = {";
    for (size_t i = 0; i < sample_count; ++i) {
        if ((i % samples_per_line) == 0)
            ofs << "\n    ";
        ofs << std::invoke(memfn, rgen, args...);
        if (i != sample_count - 1)
            ofs << ", ";
    }
    ofs << "\n};\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "missing argument" << std::endl;
        return 1;
    }

    std::ofstream ofs(argv[1]);

    ofs << "// This file has been generated with rgen_generate.cpp "
        << "using SC_RGen.h from SC " << SC_VersionString() << "\n"
        << "\n"
        << "#include <array>\n"
        << "#include <cstdint>\n"
        << "\n"
        << "namespace " << version_namespace() << " {\n"
        << "\n"
        << "static constexpr uint32_t seed = " << seed << ";\n"
        << "static constexpr double scale = " << scale << ";\n"
        << "static constexpr double low = " << low << ";\n"
        << "static constexpr double high = " << high << ";\n"
        << "\n"
        << "// clang-format off\n"
        << "\n";

    // member functions

    gen_member_function(ofs, "trand", &RGen::trand);
    gen_member_function(ofs, "irand", &RGen::irand, scale);
    gen_member_function(ofs, "irand2", &RGen::irand2, scale);
    gen_member_function(ofs, "ilinrand", &RGen::ilinrand, scale);
    gen_member_function(ofs, "ibilinrand", &RGen::ibilinrand, scale);

    gen_member_function(ofs, "fcoin", &RGen::fcoin);
    gen_member_function(ofs, "frand", &RGen::frand);
    gen_member_function(ofs, "frand2", &RGen::frand2);
    gen_member_function(ofs, "frand0", &RGen::frand0);
    gen_member_function(ofs, "frand8", &RGen::frand8);
    gen_member_function(ofs, "flinrand", &RGen::flinrand);
    gen_member_function(ofs, "fbilinrand", &RGen::fbilinrand);
    gen_member_function(ofs, "fsum3rand", &RGen::fsum3rand);

    gen_member_function(ofs, "drand", &RGen::drand);
#if CHECK_MIN_VERSION(3, 15, 0)
    gen_member_function(ofs, "drand2", &RGen::drand2);
#endif
    gen_member_function(ofs, "linrand", &RGen::linrand, scale);
    gen_member_function(ofs, "bilinrand", &RGen::bilinrand, scale);
    gen_member_function(ofs, "exprandrng", &RGen::exprandrng, low, high);
    gen_member_function(ofs, "exprand", &RGen::exprand, scale);
#if CHECK_MIN_VERSION(3, 15, 0)
    gen_member_function(ofs, "biexprand", &RGen::biexprand, scale);
#endif
    gen_member_function(ofs, "sum3rand", &RGen::sum3rand, scale);

    // free functions

    gen_free_function(ofs, "trand", trand);

    gen_free_function(ofs, "fcoin", fcoin);
    gen_free_function(ofs, "frand", frand);
    gen_free_function(ofs, "frand2", frand2);
    gen_free_function(ofs, "frand0", frand0);
    gen_free_function(ofs, "frand8", frand8);

    gen_free_function(ofs, "drand", drand);
#if CHECK_MIN_VERSION(3, 15, 0)
    gen_free_function(ofs, "drand2", drand2);
#endif

    ofs << "// clang-format on\n"
        << "\n"
        << "} // namespace " << version_namespace() << "\n";
}
