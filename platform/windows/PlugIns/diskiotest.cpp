#include <windows.h>
struct InterfaceTable;

extern "C" {

void load(InterfaceTable* inTable) {
    ::MessageBox(NULL, "void load(InterfaceTable *inTable)", "void load(InterfaceTable *inTable)", MB_OK);
}
}
