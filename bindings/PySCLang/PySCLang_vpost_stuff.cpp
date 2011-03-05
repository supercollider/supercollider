/*
 * File: PYSCLang_Module_GUIStuff.cpp
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

#ifndef SC_WIN32
#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"
#else
#include "stdafx.h"
#endif

#include "PySCLang_Module.h"

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

const int kPostBufferSize = 1024;

static char* _postBuffer = NULL;
using std::cout;
using std::flush;

int PySCLang_vpost(const char *fmt, va_list ap)
{
  if(_postBuffer == NULL)
    _postBuffer = new char[kPostBufferSize];
#ifdef SC3_WIN
  int size = _vscprintf(fmt, ap);
#else
  int size = vprintf(fmt, ap);
#endif

  if (size < kPostBufferSize) {
    vsprintf(_postBuffer, fmt, ap);
    //_txtCtrl->AppendText(wxString(_postBuffer));

	if( PySCLang_Module::scLogSink_s != NULL) {
		/* make the Python call thread safe (global interpreter clock) */
		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("(s)", _postBuffer);
		PyEval_CallObject(PySCLang_Module::scLogSink_s, arglist);

		PyGILState_Release(gstate);
	}
    else { // no log sink callable..
      cout << _postBuffer;
    }

  }
  else {
    char* tmpPostBuffer = new char[size+1]; // +1 : NULL ending
    vsprintf(tmpPostBuffer, fmt, ap);
    cout << tmpPostBuffer << flush; //implement in python !!!!!!
    delete [] tmpPostBuffer;
  }
  return 0;
}

void setPostFile(FILE* file)
{
  // we're not using that , are we ?
}

#ifndef SC3_WIN
extern "C" {
	int vpost(const char *fmt, va_list ap);
}
#endif
int vpost(const char *fmt, va_list ap)
{
	PySCLang_vpost(fmt, ap);
	return 0;
}

void post(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  PySCLang_vpost(fmt, ap);
}

void postfl(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  PySCLang_vpost(fmt, ap);
}

void postText(const char *str, long len)
{
  char* txt = new char[len+1];
  memcpy(txt,str,len);
  txt[len] = 0;
  post("%s",txt);
  delete [] txt;
}

void postChar(char c)
{
  char txt[2];
  txt[0] = c;
  txt[1] = 0;
  post("%s",txt);
}

void error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  PySCLang_vpost(fmt, ap);
}


void flushPostBuf(void)
{
}

void closeAllGUIScreens()
{
}

void initGUI()
{
}


void initSCViewPrimitives()
{
}

void initCocoaFilePrimitives()
{
}

void initRendezvousPrimitives()
{
}


#ifdef SC3_WIN
void initSpeechPrimitives()
{
}
#endif

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2)
{
    return 0;
}
