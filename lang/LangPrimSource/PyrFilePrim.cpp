/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
/*

Primitives for File i/o.

*/

/* SuperCollider headers */
#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "PyrFilePrim.h"
#include "ReadWriteMacros.h"
#include "SCBase.h"
#include "sc_popen.h"

/* SuperCollider newer headers*/
#include "SC_SndFileHelpers.hpp"
#include "SC_Filesystem.hpp" // resolveIfAlias
#include "SC_Codecvt.hpp" // utf8_str_to_path, path_to_utf8_str

/* C stdlib headers */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cerrno>
#include <fcntl.h>
#include <math.h>
#include <sstream>

/* C++ stdlib headers */
#include <tuple>

/* boost headers */
#include <boost/filesystem.hpp>

/* system headers */
#ifndef _WIN32
#    include <unistd.h>
#else
#    include <direct.h>
#endif

#if defined(__APPLE__) || defined(SC_IPHONE)
#    include <CoreFoundation/CFString.h>
#    include <CoreFoundation/CFBundle.h>
#    ifndef SC_IPHONE
#        include <CoreServices/CoreServices.h>
#    endif
#endif

#define DELIMITOR ':'

namespace bfs = boost::filesystem;

int prFileDelete(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    boost::system::error_code error_code;
    bfs::remove(p, error_code);

    if (error_code)
        SetFalse(a);
    else
        SetTrue(a);

    return errNone;
}

int prFileDeleteAll(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    if (bfs::remove_all(p) > 0) {
        SetTrue(a);
    } else {
        SetFalse(a);
    }

    return errNone;
}

int prFileMTime(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    time_t mtime = bfs::last_write_time(p);
    SetInt(a, mtime);
    return errNone;
}

int prFileExists(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    bool res = bfs::exists(p);
    SetBool(a, res);
    return errNone;
}

int prFileRealPath(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char ipath[PATH_MAX];
    char opath[PATH_MAX];
    int err;

    err = slotStrVal(b, ipath, PATH_MAX);
    if (err)
        return err;

    bool isAlias = false;
    bfs::path p = SC_Codecvt::utf8_str_to_path(ipath);
    p = SC_Filesystem::resolveIfAlias(p, isAlias);
    if (p.empty())
        return errFailed;

    boost::system::error_code error_code;
    p = bfs::canonical(p, error_code).make_preferred();
    if (error_code) {
        SetNil(a);
        return errNone;
    }

    strncpy(opath, SC_Codecvt::path_to_utf8_str(p).c_str(), PATH_MAX - 1);
    opath[PATH_MAX - 1] = '\0';

    PyrString* pyrString = newPyrString(g->gc, opath, 0, true);
    SetObject(a, pyrString);

    return errNone;
}

int prFileMkDir(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    bool result = bfs::create_directories(p);

    SetBool(a, result);
    return errNone;
}

int prFileCopy(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *b = g->sp - 1, *c = g->sp;
    char filename1[PATH_MAX];
    char filename2[PATH_MAX];
    int error;
    error = slotStrVal(b, filename1, PATH_MAX);
    if (error != errNone)
        return error;
    error = slotStrVal(c, filename2, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p1 = SC_Codecvt::utf8_str_to_path(filename1);
    const bfs::path& p2 = SC_Codecvt::utf8_str_to_path(filename2);
    boost::system::error_code error_code;
    bfs::copy(p1, p2, error_code);
    if (error_code) {
        std::ostringstream s;
        s << error_code.message() << ": copy from \"" << filename1 << "\" to \"" << filename2 << "\"";
        throw std::runtime_error(s.str());
    }

    return errNone;
}

int prFileType(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    bfs::file_status s(bfs::symlink_status(p));
    SetInt(a, s.type());
    return errNone;
}

int prFileSize(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a = g->sp - 1, *b = g->sp;
    char filename[PATH_MAX];

    int error = slotStrVal(b, filename, PATH_MAX);
    if (error != errNone)
        return error;

    const bfs::path& p = SC_Codecvt::utf8_str_to_path(filename);
    uintmax_t sz = bfs::file_size(p);
    SetInt(a, sz);
    return errNone;
}


int prFileOpen(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    char filename[PATH_MAX];
    const size_t buf_size = 12;
    char mode[buf_size];
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;
    if (NotObj(c) || !isKindOf(slotRawObject(c), class_string) || NotObj(b)
        || !isKindOf(slotRawObject(b), class_string))
        return errWrongType;
    if (slotRawObject(b)->size > PATH_MAX - 1)
        return errFailed;
    if (slotRawObject(c)->size > buf_size - 1)
        return errFailed;
    pfile = (PyrFile*)slotRawObject(a);

    memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
    filename[slotRawString(b)->size] = 0;
    const bfs::path& path = SC_Codecvt::utf8_str_to_path(filename);

    memcpy(mode, slotRawString(c)->s, slotRawObject(c)->size);
    mode[slotRawString(c)->size] = 0;

#ifdef _WIN32
    if (strcmp(mode, "w") == 0)
        strcpy(mode, "wb");
    if (strcmp(mode, "r") == 0)
        strcpy(mode, "rb");

    // use _wfopen on Windows for full Unicode compatibility
    wchar_t wmode[buf_size];
    size_t wmode_size = 0;
    mbstowcs_s(&wmode_size, wmode, buf_size, mode, buf_size - 1);
    file = _wfopen(path.wstring().c_str(), wmode);
#else
    file = fopen(path.c_str(), mode);
#endif //_WIN32
    if (file) {
        SetPtr(&pfile->fileptr, file);
        SetTrue(a);
    } else {
#ifdef _WIN32
        // check if directory exisits
        // create a temporary file (somewhere) for a handle
        // the file is deleted automatically when closed
        if (bfs::is_directory(path)) {
            int err;
#    ifdef _MSC_VER
            err = tmpfile_s(&file);
            if (!err) {
                SetPtr(&pfile->fileptr, file);
                SetTrue(a);
                return errNone;
            }
#    elif defined(__MINGW32__)
            file = tmpfile();
            if (file) {
                SetPtr(&pfile->fileptr, file);
                SetTrue(a);
                return errNone;
            }
#    else
#        error compiler unsupported
#    endif
        }
#endif
        SetNil(a);
        SetFalse(a);
    }
    return errNone;
}

int prFileClose(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errNone;
    SetPtr(&pfile->fileptr, NULL);
    if (fclose(file))
        return errFailed;
    return errNone;
}

int prFileFlush(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file != NULL)
        fflush(file);
    return errNone;
}

int prFilePos(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;
    long length;

    a = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    if ((length = ftell(file)) == -1)
        return errFailed;

    SetInt(a, (int)length);
    return errNone;
}

int prFileLength(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    PyrFile* pfile = (PyrFile*)slotRawObject(a);
    FILE* file = (FILE*)slotRawPtr(&pfile->fileptr);

    if (file == NULL)
        return errFailed;

    // preserve file position
    fpos_t pos;
    if (fgetpos(file, &pos))
        return errFailed;
    if (fseek(file, 0, SEEK_END))
        return errFailed;
    size_t length;
    length = ftell(file);
    if (fsetpos(file, &pos))
        return errFailed;

    SetInt(a, length);
    return errNone;
}

int prFileSeek(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    PyrFile* pfile;
    FILE* file;
    size_t offset;
    int origin;
    static int originTable[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;
    if (NotInt(b))
        return errWrongType;
    if (NotInt(c))
        return errWrongType;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    offset = slotRawInt(b);
    origin = slotRawInt(c);
    if (origin < 0 || origin > 2)
        return errIndexOutOfRange;
    origin = originTable[origin]; // translate in case ANSI constants ever change..
    if (fseek(file, offset, origin))
        return errFailed;
    return errNone;
}


int prFileWrite(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *ptr;
    PyrFile* pfile;
    FILE* file;
    PyrObject* obj;
    char chr;

    a = g->sp - 1;
    b = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    switch (GetTag(b)) {
    case tagInt: {
        SC_IOStream<FILE*> scio(file);
        scio.writeInt32_be(slotRawInt(b));
        break;
    }
    case tagSym:
        fwrite(slotRawSymbol(b)->name, sizeof(char), slotRawSymbol(b)->length, file);
        break;
    case tagChar:
        chr = slotRawChar(b);
        fwrite(&chr, sizeof(char), 1, file);
        break;
    case tagNil:
    case tagFalse:
    case tagTrue:
    case tagPtr:
        return errWrongType;
    case tagObj: {
        // writes the indexable part of any non obj_slot format object
        obj = slotRawObject(b);
        if (!isKindOf(obj, class_rawarray) || isKindOf(obj, class_symbolarray))
            return errWrongType;
        if (obj->size) {
            ptr = obj->slots;
            int elemSize = gFormatElemSize[obj->obj_format];
            int numElems = obj->size;
#if BYTE_ORDER != BIG_ENDIAN
            switch (elemSize) {
            case 1:
                fwrite(ptr, elemSize, numElems, file);
                break;
            case 2: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 2;
                for (; ptr < ptrend; ptr += 2) {
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            case 4: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 4;
                for (; ptr < ptrend; ptr += 4) {
                    fputc(ptr[3], file);
                    fputc(ptr[2], file);
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            case 8: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 8;
                for (; ptr < ptrend; ptr += 8) {
                    fputc(ptr[7], file);
                    fputc(ptr[6], file);
                    fputc(ptr[5], file);
                    fputc(ptr[4], file);
                    fputc(ptr[3], file);
                    fputc(ptr[2], file);
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            }
#else
            fwrite(ptr, elemSize, numElems, file);
#endif
        }
        break;
    }
    default: // double
    {
        SC_IOStream<FILE*> scio(file);
        scio.writeDouble_be(slotRawFloat(b));
        break;
    }
    }
    return errNone;
}


int prFileWriteLE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *ptr;
    PyrFile* pfile;
    FILE* file;
    PyrObject* obj;
    char chr;

    a = g->sp - 1;
    b = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    switch (GetTag(b)) {
    case tagInt: {
        SC_IOStream<FILE*> scio(file);
        scio.writeInt32_le(slotRawInt(b));
        break;
    }
    case tagSym:
        fwrite(slotRawSymbol(b)->name, sizeof(char), slotRawSymbol(b)->length, file);
        break;
    case tagChar:
        chr = slotRawInt(b);
        fwrite(&chr, sizeof(char), 1, file);
        break;
    case tagNil:
    case tagFalse:
    case tagTrue:
    case tagPtr:
        return errWrongType;
    case tagObj: {
        // writes the indexable part of any non obj_slot format object
        obj = slotRawObject(b);
        if (!isKindOf(obj, class_rawarray) || isKindOf(obj, class_symbolarray))
            return errWrongType;
        if (obj->size) {
            ptr = obj->slots;
            int elemSize = gFormatElemSize[obj->obj_format];
            int numElems = obj->size;
#if BYTE_ORDER == BIG_ENDIAN
            switch (elemSize) {
            case 1:
                fwrite(ptr, elemSize, numElems, file);
                break;
            case 2: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 2;
                for (; ptr < ptrend; ptr += 2) {
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            case 4: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 4;
                for (; ptr < ptrend; ptr += 4) {
                    fputc(ptr[3], file);
                    fputc(ptr[2], file);
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            case 8: {
                char* ptr = slotRawString(b)->s;
                char* ptrend = ptr + numElems * 8;
                for (; ptr < ptrend; ptr += 8) {
                    fputc(ptr[7], file);
                    fputc(ptr[6], file);
                    fputc(ptr[5], file);
                    fputc(ptr[4], file);
                    fputc(ptr[3], file);
                    fputc(ptr[2], file);
                    fputc(ptr[1], file);
                    fputc(ptr[0], file);
                }
                break;
            }
            }
#else
            fwrite(ptr, elemSize, numElems, file);
#endif
        }
        break;
    }
    default: // double
    {
        SC_IOStream<FILE*> scio(file);
        scio.writeDouble_le(slotRawFloat(b));
        break;
    }
    }
    return errNone;
}

int prFileReadLine(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b; // receiver(a File), string
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    char* result = fgets(slotRawString(b)->s, MAXINDEXSIZE(slotRawObject(b)) - 1, file);
    if (!result) {
        SetNil(a);
    } else {
        slotRawString(b)->size = strlen(slotRawString(b)->s);
        if (slotRawString(b)->s[slotRawString(b)->size - 1] == '\n')
            slotRawString(b)->size--;
        slotCopy(a, b);
    }
    return errNone;
}

int prFilePutInt32(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int val;
    int err = slotIntVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeInt32_be(val);

    return errNone;
}

int prFilePutInt16(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int val;
    int err = slotIntVal(b, &val);
    if (err)
        return err;


    SC_IOStream<FILE*> scio(file);
    scio.writeInt16_be(val);

    return errNone;
}


int prFilePutInt32LE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int val;
    int err = slotIntVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeInt32_le(val);

    return errNone;
}

int prFilePutInt16LE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int val;
    int err = slotIntVal(b, &val);
    if (err)
        return err;


    SC_IOStream<FILE*> scio(file);
    scio.writeInt16_le(val);

    return errNone;
}

int prFilePutInt8(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int val;
    int err = slotIntVal(b, &val);
    if (err)
        return err;


    SC_IOStream<FILE*> scio(file);
    scio.writeInt8(val);

    return errNone;
}

int prFilePutChar(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;
    char z;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    if (NotChar(b))
        return errWrongType;

    z = slotRawChar(b);

    SC_IOStream<FILE*> scio(file);
    scio.writeInt8(z);

    return errNone;
}

int prFilePutFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL) {
        dumpObjectSlot(a);
        return errFailed;
    }

    float val;
    int err = slotFloatVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeFloat_be(val);

    return errNone;
}

int prFilePutDouble(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;


    double val;
    int err = slotDoubleVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeDouble_be(val);

    return errNone;
}


int prFilePutFloatLE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL) {
        dumpObjectSlot(a);
        return errFailed;
    }

    float val;
    int err = slotFloatVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeFloat_le(val);

    return errNone;
}

int prFilePutDoubleLE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;

    a = g->sp - 1;
    b = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;


    double val;
    int err = slotDoubleVal(b, &val);
    if (err)
        return err;

    SC_IOStream<FILE*> scio(file);
    scio.writeDouble_le(val);

    return errNone;
}

int prFilePutString(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrFile* pfile;
    FILE* file;
    PyrString* string;

    a = g->sp - 1;
    b = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;
    if (NotObj(b) || slotRawObject(b)->classptr != class_string)
        return errWrongType;
    string = slotRawString(b);
    if (string->size) {
        fwrite(string->s, 1, string->size, file);
    }
    return errNone;
}


int prFileGetDouble(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetFloat(a, scio.readDouble_be());
    }
    return errNone;
}

int prFileGetFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetFloat(a, scio.readFloat_be());
    }
    return errNone;
}


int prFileGetDoubleLE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetFloat(a, scio.readDouble_le());
    }
    return errNone;
}

int prFileGetFloatLE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetFloat(a, scio.readFloat_le());
    }
    return errNone;
}

int prFileGetChar(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;
    char z;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int count = fread(&z, sizeof(char), 1, file);
    if (count == 0)
        SetNil(a);
    else
        SetChar(a, z);
    return errNone;
}

int prFileGetInt8(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;
    int8 z;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int count = fread(&z, sizeof(int8), 1, file);
    if (count == 0)
        SetNil(a);
    else
        SetInt(a, z);
    return errNone;
}

int prFileGetInt16(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetInt(a, scio.readInt16_be());
    }
    return errNone;
}

int prFileGetInt32(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetInt(a, scio.readInt32_be());
    }
    return errNone;
}


int prFileGetInt16LE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetInt(a, scio.readInt16_le());
    }
    return errNone;
}

int prFileGetInt32LE(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrFile* pfile;
    FILE* file;

    a = g->sp;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    if (feof(file))
        SetNil(a);
    else {
        SC_IOStream<FILE*> scio(file);
        SetInt(a, scio.readInt32_le());
    }
    return errNone;
}

int prFileReadRaw(struct VMGlobals* g, int numArgsPushed) {
    PyrFile* pfile;
    FILE* file;

    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (!isKindOfSlot(b, class_rawarray) || isKindOfSlot(b, class_symbolarray))
        return errWrongType;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int elemSize = gFormatElemSize[slotRawObject(b)->obj_format];
    int numElems = slotRawObject(b)->size;
    numElems = fread(slotRawString(b)->s, elemSize, numElems, file);
    slotRawObject(b)->size = numElems;

#if BYTE_ORDER != BIG_ENDIAN
    switch (elemSize) {
    case 1:
        break;
    case 2: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 2;
        for (; ptr < ptrend; ptr += 2) {
            char temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;
        }
        break;
    }
    case 4: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 4;
        for (; ptr < ptrend; ptr += 4) {
            char temp = ptr[0];
            ptr[0] = ptr[3];
            ptr[3] = temp;

            temp = ptr[1];
            ptr[1] = ptr[2];
            ptr[2] = temp;
        }
        break;
    }
    case 8: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 8;
        for (; ptr < ptrend; ptr += 8) {
            char temp = ptr[0];
            ptr[0] = ptr[7];
            ptr[7] = temp;

            temp = ptr[1];
            ptr[1] = ptr[6];
            ptr[6] = temp;

            temp = ptr[2];
            ptr[2] = ptr[5];
            ptr[5] = temp;

            temp = ptr[3];
            ptr[3] = ptr[4];
            ptr[4] = temp;
        }
        break;
    }
    }
#endif

    if (slotRawObject(b)->size == 0)
        SetNil(a);
    else
        slotCopy(a, b);
    return errNone;
}

int prFileReadRawLE(struct VMGlobals* g, int numArgsPushed) {
    PyrFile* pfile;
    FILE* file;

    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (!isKindOfSlot(b, class_rawarray) || isKindOfSlot(b, class_symbolarray))
        return errWrongType;

    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errFailed;

    int elemSize = gFormatElemSize[slotRawObject(b)->obj_format];
    int numElems = slotRawObject(b)->size;
    numElems = fread(slotRawString(b)->s, elemSize, numElems, file);
    slotRawObject(b)->size = numElems;

#if BYTE_ORDER == BIG_ENDIAN
    switch (elemSize) {
    case 1:
        break;
    case 2: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 2;
        for (; ptr < ptrend; ptr += 2) {
            char temp = ptr[0];
            ptr[0] = ptr[1];
            ptr[1] = temp;
        }
        break;
    }
    case 4: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 4;
        for (; ptr < ptrend; ptr += 4) {
            char temp = ptr[0];
            ptr[0] = ptr[3];
            ptr[3] = temp;

            temp = ptr[1];
            ptr[1] = ptr[2];
            ptr[2] = temp;
        }
        break;
    }
    case 8: {
        char* ptr = slotRawString(b)->s;
        char* ptrend = ptr + numElems * 8;
        for (; ptr < ptrend; ptr += 8) {
            char temp = ptr[0];
            ptr[0] = ptr[7];
            ptr[7] = temp;

            temp = ptr[1];
            ptr[1] = ptr[6];
            ptr[6] = temp;

            temp = ptr[2];
            ptr[2] = ptr[5];
            ptr[5] = temp;

            temp = ptr[3];
            ptr[3] = ptr[4];
            ptr[4] = temp;
        }
        break;
    }
    }
#endif

    if (slotRawObject(b)->size == 0)
        SetNil(a);
    else
        slotCopy(a, b);
    return errNone;
}

int prFileGetcwd(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot* a = g->sp - 1; // File
    PyrSlot* string = g->sp;

    if (!isKindOfSlot(string, class_string))
        return errWrongType;

#ifdef _WIN32
    // use wide version, then convert to utf8
    wchar_t buf[PATH_MAX];
    wchar_t* wcwd = _wgetcwd(buf, PATH_MAX);
    if (wcwd == nullptr) {
        error(strerror(errno));
        return errFailed;
    }
    const std::string cwd_str = SC_Codecvt::utf16_wcstr_to_utf8_string(wcwd);
    const char* cwd = cwd_str.c_str();
#else
    char buf[PATH_MAX];
    const char* cwd = getcwd(buf, PATH_MAX);
    if (cwd == NULL) {
        error(strerror(errno));
        return errFailed;
    }
#endif

    strcpy(slotRawString(string)->s, cwd);
    slotRawString(string)->size = strlen(slotRawString(string)->s);

    return errNone;
}

//----------------------------------------------------------------------------//
// Pipe primitives
//----------------------------------------------------------------------------//

int prPipeOpen(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* callerSlot = g->sp - 2;
    PyrSlot* commandLineSlot = g->sp - 1;
    PyrSlot* modeSlot = g->sp;

    // commandLine and mode must be objects
    if (NotObj(modeSlot) || NotObj(commandLineSlot))
        return errWrongType;

    PyrFile* pfile = reinterpret_cast<PyrFile*>(slotRawObject(callerSlot));

    // c++17 structured binding declarations will make this into a single line
    // auto [error, string] = ...
    int error;
    std::string commandLine;
    std::tie(error, commandLine) = slotStrStdStrVal(commandLineSlot);
    if (error != errNone)
        return error;

    std::string mode;
    std::tie(error, mode) = slotStrStdStrVal(modeSlot);
    if (error != errNone)
        return error;

    pid_t pid;
    FILE* file;
    std::tie(pid, file) = sc_popen(std::move(commandLine), mode);
    if (file != nullptr) {
        SetPtr(&pfile->fileptr, file);
        SetInt(callerSlot, pid);
    } else {
        SetNil(callerSlot);
    }
    return errNone;
}

int prPipeOpenArgv(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* callerSlot = g->sp - 2;
    PyrSlot* argsSlot = g->sp - 1;
    PyrSlot* modeSlot = g->sp;

    //????
#ifdef SC_IPHONE
    SetInt(callerSlot, 0);
    return errNone;
#endif

    // argsSlot must be an object
    if (NotObj(argsSlot) || NotObj(modeSlot))
        return errWrongType;

    PyrFile* pfile = reinterpret_cast<PyrFile*>(slotRawObject(callerSlot));

    PyrObject* argsColl = slotRawObject(argsSlot);

    // argsColl must be a collection
    if (!(slotRawInt(&argsColl->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;
    // collection must contain at least one string: the path of executable to run
    if (argsColl->size < 1)
        return errFailed;

    // c++17 structured binding declarations will make this into a single line
    // auto [error, mode] = ...;
    int error;
    std::string mode;
    std::tie(error, mode) = slotStrStdStrVal(modeSlot);
    if (error != errNone)
        return error;

    std::vector<std::string> strings;
    std::tie(error, strings) = PyrCollToVectorStdString(argsColl);
    if (error != errNone)
        return error;

    pid_t pid;
    FILE* file;
    std::tie(pid, file) = sc_popen_argv(strings, mode);

    if (file != nullptr) {
        SetPtr(&pfile->fileptr, file);
        SetInt(callerSlot, pid);
    } else {
        SetNil(callerSlot);
    }
    return errNone;
}

int prPipeClose(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrSlot* b;
    PyrFile* pfile;
    FILE* file;
    pid_t pid;

    a = g->sp - 1;
    b = g->sp;
    pfile = (PyrFile*)slotRawObject(a);
    file = (FILE*)slotRawPtr(&pfile->fileptr);
    if (file == NULL)
        return errNone;
    pid = (pid_t)slotRawInt(b);

    SetPtr(&pfile->fileptr, NULL);
    int perr = sc_pclose(file, pid);
    SetInt(a, perr);
    if (perr == -1)
        return errFailed;
    return errNone;
}

//----------------------------------------------------------------------------//
// SoundFile primitives
//----------------------------------------------------------------------------//

#ifndef NO_LIBSNDFILE

int sampleFormatToString(struct SF_INFO* info, const char** string);
int sampleFormatToString(struct SF_INFO* info, const char** string) {
    unsigned int format = info->format & SF_FORMAT_SUBMASK;
    switch (format) {
    case SF_FORMAT_DPCM_8:
    case SF_FORMAT_PCM_S8:
        *string = "int16";
        break;
    case SF_FORMAT_DPCM_16:
    case SF_FORMAT_PCM_16:
    case SF_FORMAT_DWVW_16:
        *string = "int16";
        break;
    case SF_FORMAT_PCM_24:
    case SF_FORMAT_DWVW_24:
        *string = "int24";
        break;
    case SF_FORMAT_PCM_32:
        *string = "int32";
        break;
    case SF_FORMAT_FLOAT:
        *string = "float";
        break;
    case SF_FORMAT_DOUBLE:
        *string = "double";
        break;
    case SF_FORMAT_ULAW:
        *string = "ulaw";
        break;
    case SF_FORMAT_ALAW:
        *string = "alaw";
        break;
    default:
        *string = "float";
        break;
    }
    return errNone;
}


int headerFormatToString(struct SF_INFO* info, const char** string);
int headerFormatToString(struct SF_INFO* info, const char** string) {
    switch (info->format & SF_FORMAT_TYPEMASK) {
    case SF_FORMAT_WAV:
        *string = "WAV";
        break;
    case SF_FORMAT_AIFF:
        *string = "AIFF";
        break;
    case SF_FORMAT_AU:
        *string = "SUN";
        break;
    case SF_FORMAT_IRCAM:
        *string = "IRCAM";
        break;
    case SF_FORMAT_RAW:
        *string = "raw";
        break;
    case SF_FORMAT_W64:
        *string = "WAV";
        break;
    case SF_FORMAT_FLAC:
        *string = "FLAC";
        break;
// TODO allow other platforms to know vorbis once libsndfile 1.0.18 is established
#    if __APPLE__ || _WIN32 || LIBSNDFILE_1018
    case SF_FORMAT_VORBIS:
        *string = "vorbis";
        break;
#    endif
        /*
                case SF_FORMAT_PAF :

                        break ;

                case SF_FORMAT_SVX :

                        break ;

                case SF_FORMAT_NIST :

                        break ;

                case SF_FORMAT_VOC :

                        break ;

                case SF_FORMAT_MAT4 :

                        break ;

                case SF_FORMAT_MAT5 :

                        break ;

                case SF_FORMAT_PVF :

                        break ;

                case SF_FORMAT_XI :

                        break ;

                case SF_FORMAT_HTK :

                        break ;

                case SF_FORMAT_SDS :

                        break ;
        */
    default:
        *string = " ";
        break;
    }
    return errNone;
}

int sndfileFormatInfoToStrings(struct SF_INFO* info, const char** stringHead, const char** stringSample);
int sndfileFormatInfoToStrings(struct SF_INFO* info, const char** stringHead, const char** stringSample) {
    int error = 0;
    error = headerFormatToString(info, stringHead);
    error = sampleFormatToString(info, stringSample);
    return error;
}

int prSFOpenRead(struct VMGlobals* g, int numArgsPushed);
int prSFOpenRead(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    char filename[PATH_MAX];
    SNDFILE* file;
    SF_INFO info;
    const char* headerstr;
    const char* sampleformatstr;

    a = g->sp - 1;
    b = g->sp;

    PyrObject* obj1 = slotRawObject(a);

    if (!isKindOfSlot(b, class_string))
        return errWrongType;
    if (slotRawObject(b)->size > PATH_MAX - 1)
        return errFailed;

    memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
    filename[slotRawString(b)->size] = 0;

    info.format = 0;
    file = sndfileOpenFromCStr(filename, SFM_READ, &info);

    if (file) {
        SetPtr(obj1->slots + 0, file);
        sndfileFormatInfoToStrings(&info, &headerstr, &sampleformatstr);
        // headerFormatToString(&info, &headerstr);
        PyrString* hpstr = newPyrString(g->gc, headerstr, 0, true);
        SetObject(obj1->slots + 1, hpstr);
        g->gc->GCWriteNew(obj1, (PyrObjectHdr*)hpstr); // we know hpstr is white so we can use GCWriteNew
        PyrString* smpstr = newPyrString(g->gc, sampleformatstr, 0, true);
        SetObject(obj1->slots + 2, smpstr);
        g->gc->GCWriteNew(obj1, (PyrObjectHdr*)smpstr); // we know smpstr is white so we can use GCWriteNew
        SetInt(obj1->slots + 3, info.frames);
        SetInt(obj1->slots + 4, info.channels);
        SetInt(obj1->slots + 5, info.samplerate);

        SetTrue(a);
    } else {
        SetNil(a);
        SetFalse(a);
    }
    return errNone;
}

int prSFOpenWrite(struct VMGlobals* g, int numArgsPushed);
int prSFOpenWrite(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    char filename[PATH_MAX];
    SNDFILE* file;
    SF_INFO info;
    PyrSlot* headerSlot;
    PyrSlot* formatSlot;
    int error;


    a = g->sp - 1;
    b = g->sp;

    headerSlot = (slotRawObject(a)->slots + 1);
    formatSlot = (slotRawObject(a)->slots + 2);


    if (!isKindOfSlot(headerSlot, class_string))
        return errWrongType;
    if (!isKindOfSlot(formatSlot, class_string))
        return errWrongType;

    if (!isKindOfSlot(b, class_string))
        return errWrongType;
    if (slotRawObject(b)->size > PATH_MAX - 1)
        return errFailed;

    memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
    filename[slotRawString(b)->size] = 0;

#    ifdef _WIN32
    char* headerFormat = (char*)malloc(slotRawObject(headerSlot)->size);
#    else
    char headerFormat[slotRawString(headerSlot)->size];
#    endif
    memcpy(headerFormat, slotRawString(headerSlot)->s, slotRawObject(headerSlot)->size);
    headerFormat[slotRawString(headerSlot)->size] = 0;

#    ifdef _WIN32
    char* sampleFormat = (char*)malloc(slotRawString(formatSlot)->size);
#    else
    char sampleFormat[slotRawString(formatSlot)->size];
#    endif
    memcpy(sampleFormat, slotRawString(formatSlot)->s, slotRawObject(formatSlot)->size);
    sampleFormat[slotRawString(formatSlot)->size] = 0;

    error = sndfileFormatInfoFromStrings(&info, headerFormat, sampleFormat);
    if (error) {
#    ifdef _WIN32
        free(sampleFormat);
        free(headerFormat);
#    endif
        return errFailed;
    }

    if (error)
        return errFailed;
    // slotIntVal(slotRawObject(a)->slots + 3, &info.frames);
    slotIntVal(slotRawObject(a)->slots + 4, &info.channels);
    slotIntVal(slotRawObject(a)->slots + 5, &info.samplerate);

    file = sndfileOpenFromCStr(filename, SFM_WRITE, &info);

    sf_command(file, SFC_SET_CLIPPING, NULL, SF_TRUE);

    if (file) {
        SetPtr(slotRawObject(a)->slots + 0, file);
        SetTrue(a);
    } else {
        SetNil(a);
        SetFalse(a);
    }

    return errNone;
}

int prSFClose(struct VMGlobals* g, int numArgsPushed);
int prSFClose(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

    SNDFILE* file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);
    if (file) {
        sf_close(file);
        SetNil(slotRawObject(a)->slots + 0);
    }

    return errNone;
}

int prSFRead(struct VMGlobals* g, int numArgsPushed);
int prSFRead(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;

    a = g->sp - 1;
    b = g->sp;

    SNDFILE* file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

    if (!isKindOfSlot(b, class_rawarray))
        return errWrongType;

    switch (slotRawObject(b)->obj_format) {
    case obj_int16:
        slotRawObject(b)->size = sf_read_short(file, (short*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_int32:
        slotRawObject(b)->size = sf_read_int(file, (int*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_float:
        slotRawObject(b)->size = sf_read_float(file, (float*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_double:
        slotRawObject(b)->size = sf_read_double(file, (double*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    default:
        error("sample format not supported.\n");
        return errFailed;
    }

    return errNone;
}

int prSFWrite(struct VMGlobals* g, int numArgsPushed);
int prSFWrite(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;

    a = g->sp - 1;
    b = g->sp;

    SNDFILE* file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

    if (!isKindOfSlot(b, class_rawarray))
        return errWrongType;

    switch (slotRawObject(b)->obj_format) {
    case obj_int16:
        sf_write_short(file, (short*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_int32:
        sf_write_int(file, (int*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_float:
        sf_write_float(file, (float*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    case obj_double:
        sf_write_double(file, (double*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
        break;
    default:
        error("sample format not supported.\n");
        return errFailed;
    }

    return errNone;
}

int prSFSeek(struct VMGlobals* g, int numArgsPushed);
int prSFSeek(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    SNDFILE* file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

    int origin, offset;
    int err = slotIntVal(b, &offset);
    if (err)
        return err;

    err = slotIntVal(c, &origin);
    if (err)
        return err;

    sf_seek(file, offset, origin);

    return errNone;
}

int prSFHeaderInfoString(struct VMGlobals* g, int numArgsPushed);
int prSFHeaderInfoString(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    a = g->sp;
    SNDFILE* file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);
    if (file) {
        static char strbuffer[(1 << 16)];
        sf_command(file, SFC_GET_LOG_INFO, strbuffer, (1 << 16));
        PyrString* pstring = newPyrString(g->gc, strbuffer, 0, true);
        // post(strbuffer);
        SetObject(a, pstring);
        return errNone;
    }
    return errFailed;
}

#else // !NO_LIBSNDFILE

int prSFOpenRead(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFOpenWrite(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFClose(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFWrite(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFRead(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFSeek(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

int prSFHeaderInfoString(struct VMGlobals* g, int numArgsPushed) { return errFailed; }

#endif // !NO_LIBSNDFILE

/////////////

void initFilePrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_SFOpenRead", prSFOpenRead, 2, 0);
    definePrimitive(base, index++, "_SFOpenWrite", prSFOpenWrite, 2, 0);
    definePrimitive(base, index++, "_SFClose", prSFClose, 1, 0);
    definePrimitive(base, index++, "_SFWrite", prSFWrite, 2, 0);
    definePrimitive(base, index++, "_SFRead", prSFRead, 2, 0);
    definePrimitive(base, index++, "_SFSeek", prSFSeek, 3, 0);
    definePrimitive(base, index++, "_SFHeaderInfoString", prSFHeaderInfoString, 1, 0);

    definePrimitive(base, index++, "_PipeOpen", prPipeOpen, 3, 0);
    definePrimitive(base, index++, "_PipeOpenArgv", prPipeOpenArgv, 3, 0);
    definePrimitive(base, index++, "_PipeClose", prPipeClose, 2, 0);

    definePrimitive(base, index++, "_FileDelete", prFileDelete, 2, 0);
    definePrimitive(base, index++, "_FileDeleteAll", prFileDeleteAll, 2, 0);
    definePrimitive(base, index++, "_FileMTime", prFileMTime, 2, 0);
    definePrimitive(base, index++, "_FileExists", prFileExists, 2, 0);
    definePrimitive(base, index++, "_FileRealPath", prFileRealPath, 2, 0);
    definePrimitive(base, index++, "_FileMkDir", prFileMkDir, 2, 0);
    definePrimitive(base, index++, "_FileCopy", prFileCopy, 3, 0);
    definePrimitive(base, index++, "_FileType", prFileType, 2, 0);
    definePrimitive(base, index++, "_FileSize", prFileSize, 2, 0);

    definePrimitive(base, index++, "_FileOpen", prFileOpen, 3, 0);
    definePrimitive(base, index++, "_FileClose", prFileClose, 1, 0);
    definePrimitive(base, index++, "_FileFlush", prFileFlush, 1, 0);
    definePrimitive(base, index++, "_FileSeek", prFileSeek, 3, 0);
    definePrimitive(base, index++, "_FilePos", prFilePos, 1, 0);
    definePrimitive(base, index++, "_FileLength", prFileLength, 1, 0);
    definePrimitive(base, index++, "_FileWrite", prFileWrite, 2, 0);
    definePrimitive(base, index++, "_FileWriteLE", prFileWriteLE, 2, 0);
    definePrimitive(base, index++, "_FileReadLine", prFileReadLine, 2, 0);
    definePrimitive(base, index++, "_File_getcwd", prFileGetcwd, 2, 0);

    definePrimitive(base, index++, "_FilePutChar", prFilePutChar, 2, 0);
    definePrimitive(base, index++, "_FilePutInt8", prFilePutInt8, 2, 0);
    definePrimitive(base, index++, "_FilePutInt16", prFilePutInt16, 2, 0);
    definePrimitive(base, index++, "_FilePutInt32", prFilePutInt32, 2, 0);
    definePrimitive(base, index++, "_FilePutFloat", prFilePutFloat, 2, 0);
    definePrimitive(base, index++, "_FilePutDouble", prFilePutDouble, 2, 0);
    definePrimitive(base, index++, "_FilePutInt16LE", prFilePutInt16LE, 2, 0);
    definePrimitive(base, index++, "_FilePutInt32LE", prFilePutInt32LE, 2, 0);
    definePrimitive(base, index++, "_FilePutFloatLE", prFilePutFloatLE, 2, 0);
    definePrimitive(base, index++, "_FilePutDoubleLE", prFilePutDoubleLE, 2, 0);

    definePrimitive(base, index++, "_FileGetChar", prFileGetChar, 1, 0);
    definePrimitive(base, index++, "_FileGetInt8", prFileGetInt8, 1, 0);
    definePrimitive(base, index++, "_FileGetInt16", prFileGetInt16, 1, 0);
    definePrimitive(base, index++, "_FileGetInt32", prFileGetInt32, 1, 0);
    definePrimitive(base, index++, "_FileGetFloat", prFileGetFloat, 1, 0);
    definePrimitive(base, index++, "_FileGetDouble", prFileGetDouble, 1, 0);
    definePrimitive(base, index++, "_FileGetInt16LE", prFileGetInt16LE, 1, 0);
    definePrimitive(base, index++, "_FileGetInt32LE", prFileGetInt32LE, 1, 0);
    definePrimitive(base, index++, "_FileGetFloatLE", prFileGetFloatLE, 1, 0);
    definePrimitive(base, index++, "_FileGetDoubleLE", prFileGetDoubleLE, 1, 0);

    definePrimitive(base, index++, "_FilePutString", prFilePutString, 2, 0);

    definePrimitive(base, index++, "_FileReadRaw", prFileReadRaw, 2, 0);
    definePrimitive(base, index++, "_FileReadRawLE", prFileReadRawLE, 2, 0);
}
