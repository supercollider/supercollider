/*
    Regression test: basicRemoveAt() must not hide the first unscanned child of a
    partially scanned object from the incremental garbage collector.

    The collector may stop in the middle of scanning a large array. While the parent
    is the partial-scan object, slots below GC::GetPartialScanIndex() have been
    scanned and the slots from the cursor on have not. basicRemoveAt() memmoves every
    slot after the removed index down by one. If the removed index is before the
    cursor, the child that was sitting exactly at the cursor is shifted to
    cursor - 1, i.e. behind the scan cursor. The collector will never look at that
    slot again although the parent still references the child, so the child is swept
    even though it is reachable.

    This test drives a real VMGlobals/PyrGC created by compiling the real class
    library, puts a real array into the partial-scan state, calls the real
    basicRemoveAt() primitive and verifies that the child that crossed the cursor is
    still considered live by the collector afterwards.
*/

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <cstdio>
#include <filesystem>

#include "SC_LanguageClient.h"
#include "SC_LanguageConfig.hpp"

#include "GC.h"
#include "PyrObject.h"
#include "PyrSlot.h"
#include "VMGlobals.h"

#ifndef SC_CLASS_LIB_DIR
#    error "SC_CLASS_LIB_DIR must be defined to the in-tree SCClassLibrary"
#endif

// Defined in lang/LangPrimSource/PyrArrayPrimitives.cpp and registered as the
// _BasicRemoveAt primitive. It has no header of its own.
int basicRemoveAt(VMGlobals* g, int numArgsPushed);

namespace {

constexpr int kParentSlots = 512;
constexpr int kChildSlots = 8;
constexpr int kBudget = 8;
// Collect(kBudget) adds the 12.5% reserve to the child's own allocation credit.
constexpr int kExpectedCursor = kBudget + (kBudget >> 3);

class StderrLanguageClient final : public SC_LanguageClient {
public:
    StderrLanguageClient() : SC_LanguageClient("gc_array_test") {}

    void postText(const char* str, size_t len) override { fwrite(str, 1, len, stderr); }

    void postFlush(const char* str, size_t len) override {
        fwrite(str, 1, len, stderr);
        fflush(stderr);
    }

    void postError(const char* str, size_t len) override {
        fputs("ERROR: ", stderr);
        fwrite(str, 1, len, stderr);
    }

    void flush() override { fflush(stderr); }
};

class LanguageLock {
public:
    LanguageLock() : mClient(SC_LanguageClient::instance()) {
        BOOST_REQUIRE(mClient != nullptr);
        mClient->lock();
    }

    ~LanguageLock() {
        if (mClient)
            mClient->unlock();
    }

    LanguageLock(const LanguageLock&) = delete;
    LanguageLock& operator=(const LanguageLock&) = delete;

private:
    SC_LanguageClient* mClient;
};

class StackRestore {
public:
    explicit StackRestore(VMGlobals* g) : mGlobals(g), mSaved(g->sp) {}
    ~StackRestore() { mGlobals->sp = mSaved; }

    StackRestore(const StackRestore&) = delete;
    StackRestore& operator=(const StackRestore&) = delete;

private:
    VMGlobals* mGlobals;
    PyrSlot* mSaved;
};

struct GCRuntime {
    GCRuntime() {
        SC_LanguageClient::Options options;
        options.mPort = 0;
        mClient.initRuntime(options);

        SC_LanguageConfig::freeLibraryConfig();
        gLanguageConfig = new SC_LanguageConfig();
        gLanguageConfig->setExcludeDefaultPaths(true);

        const std::filesystem::path classLibraryDir(SC_CLASS_LIB_DIR);
        gLanguageConfig->addIncludedDirectory(classLibraryDir);
        // Matches build_sclang.cfg: never execute a user's startup.scd from a test.
        gLanguageConfig->addIncludedDirectory(classLibraryDir.parent_path() / "platform" / "disable_startup_files");

#ifdef SC_CLASS_LIB_EXCLUDE_GUI
        // The Qt-less build installs the class library without any directory named
        // GUI (see SCCLASSLIB_EXCLUDE_REGEX in the top-level CMakeLists.txt).
        const std::filesystem::path libraryRoot = classLibraryDir;
        for (auto it = std::filesystem::recursive_directory_iterator(libraryRoot);
             it != std::filesystem::recursive_directory_iterator(); ++it) {
            if (it->is_directory() && it->path().filename() == "GUI")
                gLanguageConfig->addExcludedDirectory(it->path());
        }
#endif
#ifdef SC_CLASS_LIB_EXCLUDE_ABLETON
        gLanguageConfig->addExcludedDirectory(classLibraryDir / "External" / "Ableton");
#endif

        mCompiled = mClient.compileLibrary(true);
    }

    ~GCRuntime() {
        mClient.shutdownLibrary();
        mClient.shutdownRuntime();
        SC_LanguageConfig::freeLibraryConfig();
    }

    StderrLanguageClient mClient;
    bool mCompiled = false;
};

} // namespace

BOOST_FIXTURE_TEST_CASE(basicRemoveAt_keeps_first_unscanned_child_alive, GCRuntime) {
    BOOST_REQUIRE(mCompiled);

    VMGlobals* g = gMainVMGlobals;
    BOOST_REQUIRE(g != nullptr);
    PyrGC* gc = g->gc;
    BOOST_REQUIRE(gc != nullptr);
    BOOST_REQUIRE(class_array != nullptr);

    LanguageLock languageLock;
    StackRestore stackRestore(g);

    // Start from an empty scan budget and no pending partial scan.
    gc->FullCollection();

    // Large parent whose scanned prefix is nil, so scanning it creates no grey
    // credit that could mask the bug.
    PyrObject* parent = newPyrArray(gc, kParentSlots, 0, false);
    BOOST_REQUIRE(parent != nullptr);
    nilSlots(parent->slots, kParentSlots);
    parent->size = kParentSlots;
    BOOST_REQUIRE(gc->IsWhite(parent));

    // Root the parent on the VM stack. It must survive every collection below.
    ++g->sp;
    SetObject(g->sp, parent);
    PyrSlot* rootSlot = g->sp;

    // Spend the parent's allocation credit with a full scan and a flip, so the next
    // collection can stop in the middle of the parent.
    gc->FullCollection();
    BOOST_REQUIRE_EQUAL(slotRawObject(rootSlot), parent);
    BOOST_REQUIRE(!gc->IsFree(parent));
    BOOST_REQUIRE(!gc->IsPartialScanObject(parent));

    // One small white child. The parent is otherwise nil, so after a partial scan of
    // the nil prefix this child is the first unscanned child.
    PyrObject* child = newPyrArray(gc, kChildSlots, 0, false);
    BOOST_REQUIRE(child != nullptr);
    child->size = kChildSlots;
    for (int i = 0; i < kChildSlots; ++i)
        SetInt(&child->slots[i], 0x5100 + i);
    BOOST_REQUIRE(gc->IsWhite(child));
    BOOST_REQUIRE(kExpectedCursor < parent->size);
    BOOST_REQUIRE(parent->slots[kExpectedCursor].isNil());
    SetObject(&parent->slots[kExpectedCursor], child);

    // ToGrey2() (unlike ToGrey()) does not add scan credit, so the fresh child's
    // credit is the whole budget. Put the parent at the front of the grey queue.
    BOOST_REQUIRE(!gc->IsGrey(parent));
    gc->ToGrey2(parent);
    BOOST_REQUIRE(gc->IsGrey(parent));

    gc->Collect(static_cast<uint64>(kBudget));

    BOOST_REQUIRE(gc->IsPartialScanObject(parent));
    BOOST_REQUIRE_EQUAL(gc->GetPartialScanIndex(), kExpectedCursor);
    BOOST_REQUIRE(gc->IsBlack(parent));
    // The first unscanned child is still white and still at the cursor.
    BOOST_REQUIRE(gc->IsWhite(child));
    BOOST_REQUIRE_EQUAL(slotRawObject(&parent->slots[kExpectedCursor]), child);
    for (int i = 0; i < kExpectedCursor; ++i)
        BOOST_REQUIRE(parent->slots[i].isNil());

    // Remove slot 0. This memmoves every later slot down by one, so the first
    // unscanned child moves from the cursor to cursor - 1, behind the cursor.
    ++g->sp;
    SetObject(g->sp, parent); // receiver
    ++g->sp;
    SetInt(g->sp, 0); // index
    BOOST_REQUIRE(basicRemoveAt(g, 2) == errNone);
    BOOST_REQUIRE((g->sp - 1)->isNil()); // removed element was the nil in slot 0
    g->sp -= 2;

    BOOST_REQUIRE_EQUAL(parent->size, kParentSlots - 1);
    BOOST_REQUIRE_EQUAL(gc->GetPartialScanIndex(), kExpectedCursor);
    BOOST_REQUIRE_EQUAL(slotRawObject(&parent->slots[kExpectedCursor - 1]), child);
    BOOST_REQUIRE(parent->slots[kExpectedCursor].isNil());

    // Full collection: the parent is the partial-scan object and the child is only
    // reachable through the slot that was shifted behind the scan cursor.
    gc->FullCollection();

    BOOST_REQUIRE(!gc->IsPartialScanObject(parent));
    BOOST_REQUIRE(!gc->IsFree(parent));
    // The failing invariant: the collector must not treat a child that the live
    // parent still references as dead just because basicRemoveAt() moved it behind
    // the partial-scan cursor.
    BOOST_CHECK(!gc->IsFree(child));

    BOOST_REQUIRE_EQUAL(slotRawObject(&parent->slots[kExpectedCursor - 1]), child);
    BOOST_CHECK_EQUAL(child->classptr, class_array);
    BOOST_CHECK_EQUAL(child->size, kChildSlots);
    for (int i = 0; i < kChildSlots; ++i)
        BOOST_CHECK_EQUAL(slotRawInt(&child->slots[i]), 0x5100 + i);

    // A live child must not be handed out again by the allocator. If the collector
    // treated it as garbage, this same-size-class allocation can recycle it.
    PyrObject* recycled = newPyrArray(gc, kChildSlots, 0, false);
    BOOST_REQUIRE(recycled != nullptr);
    BOOST_CHECK(recycled != child);
}
