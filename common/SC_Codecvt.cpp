#include "SC_Codecvt.hpp"
#ifdef _WIN32
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h> // MultiByteToWideChar, WideCharToMultiByte
#endif

namespace SC_Codecvt {

#ifdef _WIN32

std::wstring utf8_to_utf16_str(std::string_view s) {
    if (s.empty())
        return {};

    const int str_length = static_cast<int>(s.size());

    int required_size = MultiByteToWideChar(CP_UTF8, 0, s.data(), str_length, nullptr, 0);

    if (required_size <= 0)
        return {};

    std::wstring result(static_cast<std::size_t>(required_size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), str_length, result.data(), required_size);
    return result;
}

std::string utf16_to_utf8_str(std::wstring_view s) {
    if (s.empty())
        return {};

    const int str_length = static_cast<int>(s.size());

    int required_size = WideCharToMultiByte(CP_UTF8, 0, s.data(), str_length, nullptr, 0, nullptr, nullptr);

    if (required_size <= 0)
        return {};

    std::string result(static_cast<std::size_t>(required_size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.data(), str_length, result.data(), required_size, nullptr, nullptr);
    return result;
}

#endif // _WIN32

} // SC_Codecvt
