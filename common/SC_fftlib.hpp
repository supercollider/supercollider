#pragma once

#include "SC_fftlib.h"

// undefine macros from SC_InterfaceTable.h
#undef scfft_create
#undef scfft_dofft
#undef scfft_doifft
#undef scfft_destroy

// To initialise a specific FFT, ensure your input and output buffers exist. Internal data structures
// will be allocated using the alloc object,
// Both "fullsize" and "winsize" should be powers of two (this is not checked internally).
scfft* scfft_create(size_t fullsize, size_t winsize, int32 wintype, float* indata, float* outdata, int32 direction,
                    SCFFT_Allocator* alloc);

// These two will take data from indata, use trbuf to process it, and put their results in outdata.
void scfft_dofft(scfft* f);
void scfft_doifft(scfft* f);

// destroy any resources held internally.
void scfft_destroy(scfft* f, SCFFT_Allocator* alloc);

// initialize thread local buffers
void scfft_thread_init();
