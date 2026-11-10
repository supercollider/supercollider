#include <iostream>

#ifdef _WIN32
__declspec(dllexport)
#endif
    void printGreeting(const char* who) {
    std::cout << "Hello " << who << "!" << std::endl;
}
