/*
 * File: PYSCLang_Module.h
 * Project : Psycollider
 *
 * by:
 * Benjamin Golinvaux
 * benjamin.golinvaux@euresys.com
 * messenger: bgolinvaux@hotmail.com
 *
 * currently maintained by:
 * Christopher Frauenberger
 * frauenberger@iem.at
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 *
 */

#ifndef _PYSCLANG_MODULE_
#define _PYSCLANG_MODULE_

using namespace Py;

#include "SCBase.h"
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
  Py::Object sendMain(const Py::Tuple &a);
  Py::Object compileLibrary(const Py::Tuple &a);
  Py::Object setCmdLine(const Py::Tuple &a);
  Py::Object start(const Py::Tuple &a);
  Py::Object compiledOK__(const Py::Tuple &a);
  Py::Object setSCLogSink(const Py::Tuple &a);
  Py::Object setPyPrOpenWinTextFile(const Py::Tuple &a);
};

#endif