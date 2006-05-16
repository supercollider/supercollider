#ifndef SC_WIN32
#include "pycxx/cxx/Objects.hxx"
#include "pycxx/cxx/Extensions.hxx"
#else
#include "stdafx.h"
#endif

#include "PySCLang_Module.h"

extern "C" {
  void initPySCLang()
	{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
	Py::InitialisePythonIndirectInterface();
#endif
  static PySCLang_Module* thePySCLang_Module = new PySCLang_Module;
	}
}

ChangeCounter gUIChangeCounter;

extern bool compiledOK;
extern pthread_mutex_t gLangMutex;
extern PyrSymbol *s_tick;

PyrString* newPyrStringN(class PyrGC *gc, long length, long flags, bool collect);
void dumpByteCodes(PyrBlock *theBlock);

void flushPostBuf();
// void SetupHomeDirectory();
double elapsedTime();

extern char *gHomePath;

#ifdef SC_WIN32
void PySCLang_InitNetwork( )
{
#ifdef SC_WIN32_STATIC_PTHREADS
  // initialize statically linked pthreads library
  pthread_win32_process_attach_np();
#endif

  // initialize winsock
  WSAData wsaData;
  int nCode;

  if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
    char msg[1024];
    sprintf(msg, "WSAStartup() failed with error code %d.\n", nCode );
    ::MessageBox(NULL,msg,"Error",MB_OK);
  }
}
#endif

// triggered for app clock ticks
extern "C" void *appClockTimer(void * pymod) {
	((PySCLang_Module *)pymod)->appClock();
	return NULL;
}

void PySCLang_Module::appClock() {
	while(true) {
		pthread_mutex_lock(&gLangMutex);
		runLibrary(getsym("tick"));
		pthread_mutex_unlock(&gLangMutex);
	
#ifdef SC_WIN32
		Sleep(20);
#else
		sleep(20);
#endif
	}
}

  PySCLang_Module::PySCLang_Module() : ExtensionModule<PySCLang_Module>( "PySCLang" )
  {
#ifdef SC_WIN32
    PySCLang_InitNetwork( );
#endif
    add_varargs_method("sum", &PySCLang_Module::ex_sum, "DEBUG STUFF : sum(arglist) = sum of arguments");
    add_varargs_method("sendMain", &PySCLang_Module::sendMain, "sendMain");
    add_varargs_method("compileLibrary", &PySCLang_Module::compileLibrary, "compileLibrary");
    add_varargs_method("setCmdLine", &PySCLang_Module::setCmdLine, "setCmdLine");
    add_varargs_method("start", &PySCLang_Module::start, "start");
    add_varargs_method("setSCLogSink", &PySCLang_Module::setSCLogSink, "setSCLogSink");
    add_varargs_method("compiledOK", &PySCLang_Module::compiledOK__, "compiledOK");
    add_varargs_method("Rtf2Ascii", &PySCLang_Module::Rtf2Ascii, "Rtf2Ascii");
    add_varargs_method("setPyPrOpenWinTextFile", &PySCLang_Module::setPyPrOpenWinTextFile, "setPyPrOpenWinTextFile callable with (path,startRange,rangeSize)");
    
    initialize( "<documentation for the PySCLang_Module forthcoming>" );
  }



// symbol required for the debug version
extern "C" {
  void initPySCLang_d() {
    initPySCLang(); 
  }
}

Py::Object PySCLang_Module::ex_sum(const Py::Tuple &a)
{
  // this is just to test the function verify_length:
  try
  {
    a.verify_length(0);
    std::cout << "I see that you refuse to give me any work to do." << std::endl;
  }
  catch (Exception& e)
  {
    e.clear();
    std::cout << "I will now add up your elements, oh great one." << std::endl;
  }

  Float f(0.0);
  for( Sequence::size_type i = 0; i < a.length(); i++ )
  {    
    Float g (a[i]);
    f = f + g;
  }
  return f;
}

Py::Object PySCLang_Module::sendMain(const Py::Tuple &a)
{
  if(a.size() != 1) {
    PyErr_SetString(PyExc_IndexError,"requires 1 string argument");
    return Py::Object(Py::Null());
  }
  Py::String pystr(a[0]);
  std::string str = pystr;
  const char* methodName = str.c_str();
  pthread_mutex_lock(&gLangMutex);
  runLibrary(getsym(methodName));
  pthread_mutex_unlock(&gLangMutex);
  if (PyErr_Occurred( ) != NULL) // there might be python calls from within sclang
    return Py::Object(Py::Null());
  return Py::Nothing();
}

Py::Object PySCLang_Module::compileLibrary(const Py::Tuple &a)
{
  if(a.size() != 0) {
    PyErr_SetString(PyExc_IndexError,"requires 0 args");
    return Py::Object(Py::Null());
  }
  ::compileLibrary( );
  return Py::Nothing();
}

Py::Object PySCLang_Module::setCmdLine(const Py::Tuple &a)
{
  if(a.size() != 1) {
    PyErr_SetString(PyExc_IndexError,"requires 1 string argument");
    return Py::Object(Py::Null());
  }
  Py::String pystr(a[0]);
  const char* text = ::PyString_AsString(pystr.ptr());
  int length = strlen(text);
	
  if (!compiledOK) {
    PyErr_SetString(PyExc_RuntimeError,"The library has not been compiled successfully");
    return Py::Object(Py::Null());
	}
	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals *g = gMainVMGlobals;
		
			int textlen = length;
		PyrString* strobj = newPyrStringN(g->gc, textlen, 0, true);
		memcpy(strobj->s, (char*)text, textlen);
	
		SetObject(&g->process->interpreter.uoi->cmdLine, strobj);
		g->gc->GCWrite(g->process->interpreter.uo, strobj);
	}
	pthread_mutex_unlock(&gLangMutex);
  return Py::Nothing();
}

Py::Object PySCLang_Module::compiledOK__(const Py::Tuple &a)
{
  if(a.size() != 0) {
    PyErr_SetString(PyExc_IndexError,"requires 0 args");
    return Py::Object(Py::Null());
  }
  if( compiledOK ) 
    return Py::Int(1);
  else
    return Py::Int(0);
}

Py::Object PySCLang_Module::start(const Py::Tuple &a)
{
	if(a.size() != 0) {
		PyErr_SetString(PyExc_IndexError,"requires 0 args");
		return Py::Object(Py::Null());
	}
	pyr_init_mem_pools( 2*1024*1024, 256*1024 );
	init_OSC(57120);
	schedInit();
	::compileLibrary();

  	// appClock timer
	pthread_t t;
	pthread_create(&t, NULL, &appClockTimer, (void *)this);
	
	// deferred task timer still missing... (cf, 16 May 2006)

//!!!
// Rendezvous is broken in a way that makes SC become unusable
//	// CR ADDED
//	[[RendezvousClient sharedClient] findOSCServices];
//!!!
  return Py::Nothing();
}


int rtf2txt(char* txt);
int stripNonAscii(char *txt);
Py::Object PySCLang_Module::Rtf2Ascii(const Py::Tuple &a)
{
  if(a.size() != 1) {
    PyErr_SetString(PyExc_IndexError,"requires 1 string argument");
    return Py::Object(Py::Null());
  }
  Py::String pystr(a[0]);
  std::string str = pystr;
  const char* rtfContent = str.c_str();
  pthread_mutex_lock(&gLangMutex);
  int rtfContentLen = strlen(rtfContent);
  char* inplaceTxt = reinterpret_cast<char*>(malloc(sizeof(char)*(rtfContentLen+1)));
  strcpy(inplaceTxt,rtfContent);
  int processedLen = rtf2txt(inplaceTxt);
  if(processedLen != 0) {
    inplaceTxt[processedLen+1] = 0;
    processedLen = stripNonAscii(inplaceTxt);
    inplaceTxt[processedLen+1] = 0;
  }
  pthread_mutex_unlock(&gLangMutex);
  Py::String pyRetStr(inplaceTxt);
  free(inplaceTxt);
  return pyRetStr;
}

PyObject* PySCLang_Module::scLogSink_s = NULL;
PyObject* PySCLang_Module::PyPrOpenWinTextFile_s = NULL;

Py::Object PySCLang_Module::setPyPrOpenWinTextFile(const Py::Tuple &a)
{
  if(a.size() != 1) {
    PyErr_SetString(PyExc_IndexError,"requires 1 callable argument");
    return Py::Object(Py::Null());
  }
  PyObject* logSink = a[0].ptr();
  if( PyCallable_Check( logSink ) == 0)
    PyErr_SetString(PyExc_TypeError,"object must be callable");
  else {
    PyPrOpenWinTextFile_s = logSink;
    Py_INCREF(logSink);
  }
  return Py::Nothing();
}


Py::Object PySCLang_Module::setSCLogSink(const Py::Tuple &a)
{
  if(a.size() != 1) {
    PyErr_SetString(PyExc_IndexError,"requires 1 callable argument");
    return Py::Object(Py::Null());
  }
  PyObject* logSink = a[0].ptr();
  if( PyCallable_Check( logSink ) == 0)
    PyErr_SetString(PyExc_TypeError,"object must be callable");
  else {
    scLogSink_s = logSink;
    Py_INCREF(logSink);
  }
  return Py::Nothing();
}


