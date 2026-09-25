
#pragma once

/* special gc colors */
enum {
    obj_permanent = 1, // sent to gc->New as a flag
    obj_gcmarker = 2 // gc treadmill marker
};

/* obj flag fields */
enum { obj_inaccessible = 4, obj_immutable = 16, obj_marked = 128 };

/* format types : */
enum {
    obj_notindexed,
    obj_slot,
    obj_double,
    obj_float,
    obj_int32,
    obj_int16,
    obj_int8,
    obj_char,
    obj_symbol,

    NUMOBJFORMATS
};


/*
    PyrObject represents the structure of all SC Objects.
    PyrObjectHdr : object header fields
    prev, next : pointers in the GC treadmill
    classptr : pointer to the object's class
    size : number of slots or indexable elements.

    obj_format : what kind of data this object holds
    obj_sizeclass : power of two size class of the object
    obj_flags :
        immutable : set if object may not be updated.
        finalize : set if object requires finalization.
        marked : used by garbage collector debug sanity check. may be used by primitives but must be cleared before
   exiting primitive.
   gc_color : GC color : black, grey, white, free, permanent
   scratch1 : undefined value. may be used within primitives as a temporary scratch value.
*/

struct PyrObjectHdr {
    struct PyrObjectHdr *prev, *next;
    struct PyrClass* classptr;
    int size;

    unsigned char obj_format;
    unsigned char obj_sizeclass;
    unsigned char obj_flags;
    unsigned char gc_color;

    int scratch1;

    int SizeClass() { return obj_sizeclass; }

    void SetMark() { obj_flags |= obj_marked; }
    void ClearMark() { obj_flags &= ~obj_marked; }
    bool IsMarked() const { return obj_flags & obj_marked; }
    bool IsPermanent() const { return gc_color == obj_permanent; }
    bool IsImmutable() const { return obj_flags & obj_immutable; }
    bool IsMutable() const { return !IsImmutable(); }
};
