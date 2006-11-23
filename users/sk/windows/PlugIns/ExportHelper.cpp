struct InterfaceTable;

extern "C"
{
	#ifdef SC_WIN32
  __declspec(dllexport)
  #endif //SC_WIN32
  void _load(InterfaceTable *inTable);
  void load(InterfaceTable *inTable);
}


void _load(InterfaceTable *inTable)
{
  load(inTable);
}


