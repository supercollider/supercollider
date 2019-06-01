

#include <Winsock2.h>

extern "C" {

int WSAGetLastError_Compat() { return WSAGetLastError(); }

void WSASetLastError_Compat(int a) { WSASetLastError(a); }
}
