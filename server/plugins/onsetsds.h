/*
    OnsetsDS - real time musical onset detection library.
    Copyright (c) 2007 Dan Stowell. All rights reserved.

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

/** \file */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
// Macros and consts

// log(0.1)
#define ods_log1 -2.30258509

#define PI 3.1415926535898f
#define MINUSPI -3.1415926535898f
#define TWOPI 6.28318530717952646f
#define INV_TWOPI 0.1591549430919f

#define ods_abs(a) ((a) < 0 ? -(a) : (a))
#define ods_max(a, b) (((a) > (b)) ? (a) : (b))
#define ods_min(a, b) (((a) < (b)) ? (a) : (b))

#define ODS_LOG_LOWER_LIMIT 2e-42
#define ODS_LOGOF_LOG_LOWER_LIMIT -96.0154267
#define ODS_ABSINVOF_LOGOF_LOG_LOWER_LIMIT 0.010414993

////////////////////////////////////////////////////////////////////////////////
// Constants

/**
 * Types of incoming FFT data format. OnsetsDS needs to know where the FFT
 * data comes from in order to interpret it correctly.
 */
enum onsetsds_fft_types {
    ODS_FFT_SC3_COMPLEX, ///< SuperCollider, cartesian co-ords ("SCComplexBuf") - NB it's more efficient to provide
                         ///< polar data from SC
    ODS_FFT_SC3_POLAR, ///< SuperCollider, polar co-ords ("SCPolarBuf")
    ODS_FFT_FFTW3_HC, ///< FFTW <a
                      ///< href="http://www.fftw.org/fftw3_doc/The-Halfcomplex_002dformat-DFT.html">"halfcomplex"</a>
                      ///< format
    ODS_FFT_FFTW3_R2C ///< FFTW regular format, typically produced using <a
                      ///< href="http://www.fftw.org/fftw3_doc/One_002dDimensional-DFTs-of-Real-Data.html#One_002dDimensional-DFTs-of-Real-Data">real-to-complex</a>
                      ///< transform
};

/**
 * Types of onset detection function
 */
enum onsetsds_odf_types {
    ODS_ODF_POWER, ///< Power
    ODS_ODF_MAGSUM, ///< Sum of magnitudes
    ODS_ODF_COMPLEX, ///< Complex-domain deviation
    ODS_ODF_RCOMPLEX, ///< Complex-domain deviation, rectified (only increases counted)
    ODS_ODF_PHASE, ///< Phase deviation
    ODS_ODF_WPHASE, ///< Weighted phase deviation
    ODS_ODF_MKL ///< Modified Kullback-Liebler deviation
};

/**
 * Types of whitening - may not all be implemented yet.
 */
enum onsetsds_wh_types {
    ODS_WH_NONE, ///< No whitening - onsetsds_whiten() becomes a no-op
    ODS_WH_ADAPT_MAX1, ///< Adaptive whitening - tracks recent-peak-magnitude in each bin, normalises that to 1
    ODS_WH_NORMMAX, ///< Simple normalisation - each frame is normalised (independent of others) so largest magnitude
                    ///< becomes 1. Not implemented.
    ODS_WH_NORMMEAN ///< Simple normalisation - each frame is normalised (independent of others) so mean magnitude
                    ///< becomes 1. Not implemented.
};

////////////////////////////////////////////////////////////////////////////////
// Structs

typedef struct OdsPolarBin {
    float mag, phase;
} OdsPolarBin;

typedef struct OdsPolarBuf {
    float dc, nyq;
    OdsPolarBin bin[1];
} OdsPolarBuf;

/// The main data structure for the onset detection routine
typedef struct OnsetsDS {
    /// "data" is a pointer to the memory that must be EXTERNALLY allocated.
    /// Other pointers will point to locations within this memory.
    float *data,
        *psp, ///< Peak Spectral Profile - size is numbins+2, data is stored in order dc through to nyquist
        *odfvals, // odfvals[0] will be the current val, odfvals[1] prev, etc
        *sortbuf, // Used to calculate the median
        *other; // Typically stores data about the previous frame
    OdsPolarBuf* curr; // Current FFT frame, as polar

    float srate, ///< The sampling rate of the input audio. Set by onsetsds_init()
                 // Adaptive whitening params
        relaxtime, ///< Do NOT set this directly. Use onsetsds_setrelax() which will also update relaxcoef.
        relaxcoef, ///< Relaxation coefficient (memory coefficient). See also onsetsds_setrelax()
        floor, ///< floor - the lowest value that a PSP magnitude can take.
        /// A parameter for the ODF. For most this is a magnitude threshold for a single bin to be considered;
        /// but for #ODS_ODF_MKL it is the "epsilon" parameter.
        odfparam,
        /// Value used internally to scale ODF value according to the FFT frame size. Automatically set by
        /// onsetsds_init()
        normfactor,
        // ODF val after median processing
        odfvalpost,
        // Previous val is needed for threshold-crossing detection
        odfvalpostprev,
        /// Threshold (of ODF value, after median processing) for detection.
        /// Values between 0 and 1 are expected, but outside this range may
        /// sometimes be appropriate too.
        thresh;

    int odftype, ///< Choose from #onsetsds_odf_types
        whtype, ///< Choose from #onsetsds_wh_types
        fftformat; ///< Choose from #onsetsds_fft_types
    bool whiten, ///< Whether to apply whitening - onsetsds_init() decides this on your behalf
        detected, ///< Output val - true if onset detected in curr frame
        /**
        NOT YET USED: Whether to convert magnitudes to log domain before processing. This is done as follows:
        Magnitudes below a log-lower-limit threshold (ODS_LOG_LOWER_LIMIT) are pushed up to that threshold (to avoid
        log(0) infinity problems), then the log is taken. The values are re-scaled to a similar range as the
        linear-domain values (assumed to lie between zero and approximately one) by subtracting log(ODS_LOG_LOWER_LIMIT)
        and then dividing by abs(log(ODS_LOG_LOWER_LIMIT)).
        */
        logmags,
        med_odd; ///< Whether median span is odd or not (used internally)

    unsigned int
        /// Number of frames used in median calculation
        medspan,
        /// Size of enforced gap between detections, measured in FFT frames.
        mingap, gapleft;
    size_t fftsize, numbins; // numbins is the count not including DC/nyq
} OnsetsDS;


////////////////////////////////////////////////////////////////////////////////
// Function prototypes


/**
 * \defgroup MainUserFuncs Main user functions
 */
//@{

/**
 * Determine how many bytes of memory must be allocated (e.g. using malloc) to
 * accompany the OnsetsDS struct, operating using the specified settings (used to
 * store part-processed FFT data etc). The user must
 * call this, and then allocate the memory, BEFORE calling onsetsds_init().
 * @param odftype Which onset detection function (ODF) you'll be using, chosen from #onsetsds_odf_types
 * @param fftsize Size of FFT: 512 is recommended.
 * @param medspan The number of past frames that will be used for median calculation during triggering
 */
size_t onsetsds_memneeded(int odftype, size_t fftsize, unsigned int medspan);

/**
 * Initialise the OnsetsDS struct and its associated memory, ready to detect
 * onsets using the specified settings. Must be called before any call to
 * onsetsds_process().
 *
 * Note: you can change the onset detection function type in mid-operation
 * by calling onsetsds_init() again, but because memory will be reset this
 * will behave as if starting from scratch (rather than being aware of the past
 * few frames of sound). Do not attempt to change the
 * onset detection function in a more hacky way (e.g. fiddling with the struct)
 * because memory is set up differently for each of the different ODFs.
 * @param ods An instance of the OnsetsDS struct
 * @param odsdata A pointer to the memory allocated, size given by onsetsds_memneeded().
 * @param fftformat Which format of FFT data is to be expected, chosen from #onsetsds_fft_types
 * @param odftype Which onset detection function (ODF) you'll be using, chosen from #onsetsds_odf_types
 * @param fftsize Size of FFT: 512 or 1024 is recommended.
 * @param medspan The number of past frames that will be used for median calculation during triggering
 * @param srate The sampling rate of the input audio
 */
void onsetsds_init(OnsetsDS* ods, float* odsdata, int fftformat, int odftype, size_t fftsize, unsigned int medspan,
                   float srate);

/**
 * Process a single FFT data frame in the audio signal. Note that processing
 * assumes that each call to onsetsds_process() is on a subsequent frame in
 * the same audio stream - to handle multiple streams you must use separate
 * OnsetsDS structs and memory!
 *
 * This function's main purpose is to call some of the library's other functions,
 * in the expected sequence.
 */
bool onsetsds_process(OnsetsDS* ods, float* fftbuf);

//@}


////////////////////////////////////////////////////////////////////////////////
// Function prototypes less commonly called by users

/**
 * \defgroup LessCommonFuncs Other useful functions
 */
//@{
/**
 * Set the "memory coefficient" indirectly via the time for the
 * memory to decay by 60 dB.
 * @param ods The OnsetsDS
 * @param time The time in seconds
 * @param hopsize The FFT frame hopsize (typically this will be half the FFT frame size)
 */
void onsetsds_setrelax(OnsetsDS* ods, float time, size_t hopsize);

//@}

////////////////////////////////////////////////////////////////////////////////
// Function prototypes not typically called by users

/**
 * \defgroup OtherFuncs Other functions, not typically called by users
 */
//@{
/**
 * Load the current frame of FFT data into the OnsetsDS struct.
 *
 * Not typically called directly by users since onsetsds_process() calls this.
 */
void onsetsds_loadframe(OnsetsDS* ods, float* fftbuf);

/**
 * Apply adaptive whitening to the FFT data in the OnsetsDS struct.
 *
 * Not typically called directly by users since onsetsds_process() calls this.
 */
void onsetsds_whiten(OnsetsDS* ods);

/**
 * Calculate the Onset Detection Function (includes scaling ODF outputs to
 * similar range)
 *
 * Not typically called directly by users since onsetsds_process() calls this.
 */
void onsetsds_odf(OnsetsDS* ods);

/**
 * Detects salient peaks in Onset Detection Function by removing the median,
 * then thresholding. Afterwards, the member ods.detected will indicate whether
 * or not an onset was detected.
 *
 * Not typically called directly by users since onsetsds_process() calls this.
 */
void onsetsds_detect(OnsetsDS* ods);

//@}

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
