// PySCLang_Module.h

using namespace Py;

#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
//#include "UserPanel.h"
#include "ChangeCounter.h"

#include <pthread.h>

class PySCLang_Module : public ExtensionModule<PySCLang_Module>
{
public:
            PySCLang_Module();
  virtual  ~PySCLang_Module() { }

  static PyObject* scLogSink_s;
  static PyObject* PyPrOpenWinTextFile_s;

	void appClock();
	
private:
  Py::Object ex_sum(const Py::Tuple &a);
  Py::Object sendMain(const Py::Tuple &a);
  Py::Object compileLibrary(const Py::Tuple &a);
  Py::Object setCmdLine(const Py::Tuple &a);
  Py::Object start(const Py::Tuple &a);
  Py::Object compiledOK__(const Py::Tuple &a);
  Py::Object Rtf2Ascii(const Py::Tuple &a);
  Py::Object setSCLogSink(const Py::Tuple &a);
  Py::Object setPyPrOpenWinTextFile(const Py::Tuple &a);
};

