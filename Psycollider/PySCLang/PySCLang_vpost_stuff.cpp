#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "PySCLang_Module.h"

const int kPostBufferSize = 1024;

static char* _postBuffer = NULL;
using std::cout;
using std::flush;

int PySCLang_vpost(const char *fmt, va_list ap)
{
  if(_postBuffer == NULL)
    _postBuffer = new char[kPostBufferSize];
  int size = _vscprintf(fmt, ap);
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

void initSpeechPrimitives()
{
}

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2)
{
    return 0;
}


