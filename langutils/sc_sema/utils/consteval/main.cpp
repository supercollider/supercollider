#include "consteval/consteval.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#    include <windows.h>
#endif // _MSC_VER

// Evaluates a piece of interpreted code as a constant, printing the result as json, see consteval.hpp.
// Exits with failure, printing the diagnostics, if it isn't a constant.

#ifdef _MSC_VER
// utility function to convert default Windows stdin encoding to UTF-8
// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
std::string utf8_encode(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    std::string strTo(static_cast<size_t>(size_needed), 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), strTo.data(), size_needed, NULL, NULL);
    return strTo;
}


// need wmain for Windows default stdin encoding
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif // _MSC_VER
    if (argc != 2) {
        std::cerr << "ERROR: incorrect number of arguments to sc_consteval, expected only supercollider source code.\n"
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }

    const std::string source = utf8_encode(std::wstring(argv[1]));
#else
    const std::string source = argv[1];
#endif // _MSC_VER

    const auto evaluated = sc::const_eval::evaluate(source, "consteval.scd");
    for (const auto& d : evaluated->diagnostics())
        std::cerr << d;
    const auto* value = evaluated->value();
    if (!value) {
        std::cerr << "Not a constant.\n";
        return EXIT_FAILURE;
    }
    value->to_json(std::cout, evaluated->symbols(), evaluated->constants());
    std::cout << std::endl;
    return 0;
}
