/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com
    Copyright (c) 2017 Brian Heim (boost::filesystem additions)

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

#include "AllocPools.h"
#include "ClassLibraryInfo.hpp"
#include "ClassLibraryLoader.hpp"
#include "FatalInterpreterError.h"
#include "GC.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"
#include "PyrKernelProto.h"
#include "PyrObject.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrSched.h"
#include "PyrSymbolTable.h"
#include "PredefinedSymbols.h"
#include "SCBase.h"
#include "SC_LanguageConfig.hpp"
#include "SpecialSelectorsOperatorsAndClasses.h"
#include "VMGlobals.h"

#include <cstring>
#include <exception>
#include <mutex>
#include <new>
#include <stdexcept>

void schedRun();


static void runShutdown(bool wasCompiledPreviously) {
    // printf("->aboutToCompileLibrary\n");
    gLangMutex.lock();
    if (wasCompiledPreviously) {
        VMGlobals* g = gMainVMGlobals;

        setCanCallOS(g, LANG_THREAD_IS_MAIN_THREAD);

        ++g->sp;
        SetObject(g->sp, g->process);
        runInterpreter(g, s_shutdown, 1);

        g->canCallOS = false;
    }
    gLangMutex.unlock();
    // printf("<-aboutToCompileLibrary\n");
}

void closeAllGUIScreens();
void TempoClock_stopAll();
void closeAllCustomPorts();

void shutdownLibrary(bool wasCompiledPreviously) {
    gClassLibraryInfo.markShuttingDown();
    closeAllGUIScreens();

    schedStop();

    runShutdown(wasCompiledPreviously);

    TempoClock_stopAll();

    {
        auto lock = std::lock_guard<std::timed_mutex> { gLangMutex };
        closeAllCustomPorts();

        if (wasCompiledPreviously) {
            VMGlobals* g = gMainVMGlobals;
            setCanCallOS(g, LANG_THREAD_IS_MAIN_THREAD);
            g->gc->RunAllFinalizers();
            g->canCallOS = false;
        }

        pyr_pool_runtime->FreeAll();
        unloadClassLibrary();
    }

    gClassLibraryInfo.reset();
    deinitPrimitives();
}

SCLANG_DLLEXPORT_C bool compileLibrary(bool wasCompiledPreviously, bool standalone) try {
    // ensure buffer is flushed
    struct FlushOnExit {
        ~FlushOnExit() { flushPostBuf(); }
    } flush;

    shutdownLibrary(wasCompiledPreviously);

    auto lock = std::lock_guard<std::timed_mutex> { gLangMutex };

    // TODO: this should just use chrono directly.
    const auto startTime = elapsedTime();

    if (!gLanguageConfig)
        SC_LanguageConfig::readLibraryConfig(standalone);


    pyr_pool_runtime->FreeAllInternal();

    void* ptr = pyr_pool_runtime->Alloc(sizeof(SymbolTable));
    if (!ptr)
        throw FatalInterpreterError { "Out of memory" };

    gMainVMGlobals->symbolTable = new (ptr) SymbolTable(pyr_pool_runtime, 65536);

#ifdef NDEBUG
    postfl("Compiling class library...\n");
#else
    postfl("Compiling class library (debug build)...\n");
#endif

    gClassLibraryInfo.markCompilationInProgress();

    initSymbols();
    initSpecialSelectors();
    initSpecialClasses();
    initClasses();
    initPrimitives();

    std::size_t thingsPosted { 0 };

    if (!loadClassLibrary(thingsPosted))
        throw std::runtime_error { "Class Library has failed to compile." };

    // build sub class arrays
    {
        // TODO: this the ordering that the subclass arrays have.
        // It ultimately is used to set the order the initClass methods are called.
        // This means if we change the hash of the symbol table people's code will break.
        // DONT change the hash until we can do some decent static analysis to give them a decent warning.


        // Yes, this will do many linear passes of the class list...
        const auto forEachClass = [&](const auto& f) {
            PyrClass* classobj = gClassList;
            while (classobj) {
                f(classobj);
                classobj = classobj->nextclass.getPyrObjType<PyrClass>();
            }
        };

        // Assign the number of children to each parent in the 'subclasses' slot (this will be an array in the next
        // step!).
        forEachClass([](PyrClass* c) {
            if (PyrClass * superClassObj { c->superclass.getSymbol()->u.classobj }) {
                // This is currently required to be set to an int when initialized.
                superClassObj->subclasses = PyrSlot::make(superClassObj->subclasses.getInt() + 1);
            }
        });

        // Create an array for the subclasses using the size as assigned in the previous step.
        forEachClass([](PyrClass* c) {
            if (const auto numSubclasses = c->subclasses.getInt()) {
                auto array = newPyrArray(nullptr, numSubclasses, obj_permanent | obj_immutable, false);
                array->size = 0;
                c->subclasses = PyrSlot::make(array);
            } else {
                // Set it to nil if empty.
                c->subclasses = PyrSlot {};
            }
        });

        forEachClass([](PyrClass* c) {
            if (PyrClass * super { c->superclass.getSymbol()->u.classobj }) {
                objAddIndexedObject(super->subclasses.getPyrObjType<PyrObject>(), c);
            }
        });
    }


    // This puts all the classes into alphabetical order.
    gClassList = sortClasses(gClassList);

    gNumClasses = indexClassTree(class_abstract_object, 0);
    const auto numSelectors = setSelectorFlags();

    buildBigMethodMatrix(numSelectors);
    checkFinalisedClassLibrary();

    post("\tNumber of Symbols %d\n", gMainVMGlobals->symbolTable->NumItems());
    const auto elapsed = elapsedTime() - startTime;
    post("\tcompiled in %.2f seconds\n", elapsed);
    const auto numOverwrites = gClassLibraryInfo.methodOverrideCount();
    if (numOverwrites == 1) {
        post("\nInfo: One method is currently overwritten by an extension. To see which, "
             "execute:\nMethodOverride.printAll\n\n");
    } else if (numOverwrites > 1) {
        post("\nInfo: %i methods are currently overwritten by extensions. To see which, "
             "execute:\nMethodOverride.printAll\n\n",
             numOverwrites);
    }

    pyr_pool_compile->FreeAll();

    PyrClass* class_main = s_main->u.classobj;
    if (!class_main)
        throw std::runtime_error { "Class 'Main' was not defined" };


    if (!isSubclassOf(class_main, class_process)) {
        throw std::runtime_error { "Class 'Main' is not a subclass of 'Process'" };
    }

    if (thingsPosted > 20) {
        post("\nInfo: many errors or warnings were posted.\nIf quarks are installed, try updating them with "
             "`Quarks.all.do{ |quark| quark.update() };\n`");
    }

    post("Compile done.\n");
    gClassLibraryInfo.markCompilationOkay();

    post("Initialising runtime.\n");

    initRuntime(gMainVMGlobals, 128 * 1024, pyr_pool_runtime);

    post("Executing 'Process.startup'...\n");
    VMGlobals* g = gMainVMGlobals;
    setCanCallOS(g, LANG_THREAD_IS_MAIN_THREAD);
    ++g->sp;
    SetObject(g->sp, g->process);

    // This will post the main greating.
    runInterpreter(g, s_startup, 1);
    g->canCallOS = false;
    schedRun();

    gClassLibraryInfo.markLibraryInitalized();

    return true;
} catch (const FatalInterpreterError& e) {
    gClassLibraryInfo.markCompilationFailed();
    throw;
} catch (const std::exception& e) {
    error("Exception thrown while booting: %s\n", e.what());
    flushPostBuf();
    gClassLibraryInfo.markCompilationFailed();
    return false;
} catch (...) {
    error("Unexpected throw object");
    gClassLibraryInfo.markCompilationFailed();
    return false;
}

void dumpByteCodes(PyrBlock* theBlock);

SCLANG_DLLEXPORT_C void runLibrary(PyrSymbol* selector, const bool runsInMainThread) {
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = runsInMainThread;
    try {
        if (gClassLibraryInfo.acceptsInput()) {
            ++g->sp;
            SetObject(g->sp, g->process);
            runInterpreter(g, selector, 1);
        } else {
            postfl("Library has not been compiled successfully.\n");
        }
    } catch (const FatalInterpreterError& er) {
        error("A fatal interpreter error has occured. Reason: %s\n", er.what());
        throw;
    } catch (const std::exception& ex) {
        PyrMethod* meth = g->method;
        if (meth) {
            int ip = slotRawInt8Array(&meth->code) ? g->ip - slotRawInt8Array(&meth->code)->b : -1;
            post("caught exception in runLibrary %s:%s %3d\n",
                 slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name, ip);
            dumpByteCodes(meth);
        } else {
            post("caught exception in runLibrary\n");
        }
        error(ex.what());
    } catch (...) { postfl("DANGER: OUT of MEMORY. Operation failed.\n"); }
    g->canCallOS = false;
}

SCLANG_DLLEXPORT_C void setCommandLine(const char* txt, size_t txtSize, const char* filePath, int lineNumber,
                                       int column) {
    VMGlobals* g = gMainVMGlobals;

    auto interpreter = g->process->interpreter.getPyrObjType<PyrInterpreter>();

    PyrString* strobj = newPyrStringN(g->gc, txtSize, 0, true);
    memcpy(strobj->s, txt, txtSize);
    interpreter->cmdLine = PyrSlot::make(strobj);

    interpreter->filePath = filePath ? PyrSlot::make(getsym(filePath)) : PyrSlot {};
    interpreter->lineNumber = PyrSlot::make(lineNumber);
    interpreter->column = PyrSlot::make(column);

    // we know strobj is white so we can use GCWriteNew
    g->gc->GCWriteNew(interpreter, strobj);
}
