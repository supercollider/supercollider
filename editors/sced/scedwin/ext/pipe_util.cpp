/*
 sced (SuperCollider mode for gedit)

 Copyright 2012 Jakob Leben

 sced is free software:
 you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Python.h>
#include <windows.h>
#include <stdio.h>
#include <string>

// DECLARATION

static PyObject* start_reader(PyObject*, PyObject*);

static PyObject* stop_reader(PyObject*, PyObject*);

static PyObject* get_buffer(PyObject*, PyObject*);

static PyMethodDef sced_methods[] = {
    { "start_reader", start_reader, METH_O, "Start reading a stream." },
    { "stop_reader", stop_reader, METH_NOARGS, "Stop reading." },
    { "get_buffer", get_buffer, METH_NOARGS, "Get the reading buffer." },
    { NULL, NULL, 0, NULL } /* Sentinel */
};

PyMODINIT_FUNC initpipe_util(void) { (void)Py_InitModule("pipe_util", sced_methods); }

// IMPLEMENTATION

using std::string;
#define BUFSIZE 256

struct sced_reader {
    sced_reader(): done(false) {}
    PyObject* py_file;
    FILE* stream;
    HANDLE thread;
    HANDLE mutex;
    string buffer;
    bool done;
};

static DWORD WINAPI reader_func(LPVOID arg);
static sced_reader* g_reader = NULL;

static PyObject* start_reader(PyObject* self, PyObject* obj) {
    if (g_reader != NULL) {
        printf("ERROR: Already reading!\n");
        return NULL;
    }

    if (!PyFile_Check(obj)) {
        printf("Argument needs a 'file'\n");
        return NULL;
    }

    g_reader = new sced_reader;
    g_reader->stream = PyFile_AsFile(obj);
    g_reader->py_file = obj;
    g_reader->mutex = CreateMutex(NULL, FALSE, NULL);

    Py_INCREF(obj);
    PyFile_IncUseCount((PyFileObject*)obj);

    g_reader->thread = CreateThread(NULL, 0, reader_func, g_reader, 0, NULL);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* stop_reader(PyObject* self, PyObject* obj) {
    if (!g_reader) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    WaitForSingleObject(g_reader->thread, INFINITE);

    CloseHandle(g_reader->thread);
    CloseHandle(g_reader->mutex);

    PyFile_DecUseCount((PyFileObject*)g_reader->py_file);
    Py_DECREF(g_reader->py_file);

    delete g_reader;
    g_reader = 0;

    Py_INCREF(Py_None);
    return Py_None;
}

static DWORD WINAPI reader_func(LPVOID arg) {
    sced_reader* reader = (sced_reader*)arg;
    FILE* stream = reader->stream;
    char buf[BUFSIZE];

    while (1) {
        char* res = fgets(buf, BUFSIZE, stream);
        if (res == buf) {
            WaitForSingleObject(reader->mutex, INFINITE);
            reader->buffer.append(buf);
            ReleaseMutex(reader->mutex);
        } else if (feof(stream)) {
            printf("EOF\n");
            break;
        } else if (ferror(stream)) {
            printf("ERROR: %i\n", errno);
            break;
        } else {
            printf("Unknown fatal condition\n");
            break;
        }
    }

    WaitForSingleObject(reader->mutex, INFINITE);
    reader->done = true;
    ReleaseMutex(reader->mutex);

    return 0;
}

static PyObject* get_buffer(PyObject* self, PyObject* arg) {
    if (!g_reader)
        return NULL;

    sced_reader* reader = g_reader;

    DWORD waitRes = WaitForSingleObject(reader->mutex, INFINITE);

    string& buf = reader->buffer;
    PyObject* ret;

    if (buf.size() > 0) {
        PyObject* py_str = PyString_FromString(buf.c_str());
        buf = string();
        ret = py_str;
    } else if (reader->done) {
        Py_INCREF(Py_False);
        ret = Py_False;
    } else {
        Py_INCREF(Py_None);
        ret = Py_None;
    }

    ReleaseMutex(reader->mutex);

    return ret;
}
