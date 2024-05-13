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

#include "onsetsds.h"


#define ODS_DEBUG_POST_CSV 0

#ifdef _MSC_VER
// msvc doesn't support c99
#    define hypotf _hypotf
#    define inline /* inline */
#endif

static inline float onsetsds_phase_rewrap(float phase) {
    return (phase > MINUSPI && phase < PI) ? phase : phase + TWOPI * (1.f + floorf((MINUSPI - phase) * INV_TWOPI));
}


size_t onsetsds_memneeded(int odftype, size_t fftsize, unsigned int medspan) {
    /*
    Need memory for:
    - median calculation (2 * medspan floats)
    - storing old values (whether as OdsPolarBuf or as weirder float lists)
    - storing the OdsPolarBuf (size is NOT sizeof(OdsPolarBuf) but is fftsize)
    - storing the PSP (numbins + 2 values)
    All these are floats.
    */

    int numbins = (fftsize >> 1) - 1; // No of bins, not counting DC/nyq

    switch (odftype) {
    case ODS_ODF_POWER:
    case ODS_ODF_MAGSUM:

        // No old FFT frames needed, easy:
        return (medspan + medspan + fftsize + numbins + 2) * sizeof(float);

    case ODS_ODF_COMPLEX:
    case ODS_ODF_RCOMPLEX:

        return (medspan + medspan + fftsize + numbins
                + 2
                // For each bin (NOT dc/nyq) we store mag, phase and d_phase
                + numbins + numbins + numbins)
            * sizeof(float);

    case ODS_ODF_PHASE:
    case ODS_ODF_WPHASE:

        return (medspan + medspan + fftsize + numbins
                + 2
                // For each bin (NOT dc/nyq) we store phase and d_phase
                + numbins + numbins)
            * sizeof(float);

    case ODS_ODF_MKL:

        return (medspan + medspan + fftsize + numbins
                + 2
                // For each bin (NOT dc/nyq) we store mag
                + numbins)
            * sizeof(float);


        break;
    }
    return -1; // bleh
}


void onsetsds_init(OnsetsDS* ods, float* odsdata, int fftformat, int odftype, size_t fftsize, unsigned int medspan,
                   float srate) {
    int numbins, realnumbins;

    // The main pointer to the processing area - other pointers will indicate areas within this
    ods->data = odsdata;
    // Set all vals in processing area to zero
    memset(odsdata, 0, onsetsds_memneeded(odftype, fftsize, medspan));

    ods->srate = srate;

    numbins = (fftsize >> 1) - 1; // No of bins, not counting DC/nyq
    realnumbins = numbins + 2;

    // Also point the other pointers to the right places
    ods->curr = (OdsPolarBuf*)odsdata;
    ods->psp = odsdata + fftsize;
    ods->odfvals = odsdata + fftsize + realnumbins;
    ods->sortbuf = odsdata + fftsize + realnumbins + medspan;
    ods->other = odsdata + fftsize + realnumbins + medspan + medspan;

    // Default settings for Adaptive Whitening, user can set own values after init
    onsetsds_setrelax(ods, 1.f, fftsize >> 1);
    ods->floor = 0.1;

    switch (odftype) {
    case ODS_ODF_POWER:
        ods->odfparam = 0.01; // "powthresh" in SC code
        ods->normfactor = 2560.f / (realnumbins * fftsize);
        break;
    case ODS_ODF_MAGSUM:
        ods->odfparam = 0.01; // "powthresh" in SC code
        ods->normfactor = 113.137085f / (realnumbins * sqrt(fftsize));
        break;
    case ODS_ODF_COMPLEX:
        ods->odfparam = 0.01; // "powthresh" in SC code
        ods->normfactor = 231.70475f / pow(fftsize, 1.5); // / fftsize;
        break;
    case ODS_ODF_RCOMPLEX:
        ods->odfparam = 0.01; // "powthresh" in SC code
        ods->normfactor = 231.70475f / pow(fftsize, 1.5); // / fftsize;
        break;
    case ODS_ODF_PHASE:
        ods->odfparam = 0.01; // "powthresh" in SC code
        ods->normfactor = 5.12f / fftsize; // / fftsize;
        break;
    case ODS_ODF_WPHASE:
        ods->odfparam = 0.0001; // "powthresh" in SC code. For WPHASE it's kind of superfluous.
        ods->normfactor = 115.852375f / pow(fftsize, 1.5); // / fftsize;
        break;
    case ODS_ODF_MKL:
        ods->odfparam = 0.01; // EPSILON parameter. Brossier recommends 1e-6 but I (ICMC 2007) found larger vals (e.g
                              // 0.01) to work better
        ods->normfactor = 7.68f * 0.25f / fftsize;
        break;
    default:
        printf("onsetsds_init ERROR: \"odftype\" is not a recognised value\n");
    }

    ods->odfvalpost = 0.f;
    ods->odfvalpostprev = 0.f;
    ods->thresh = 0.5f;
    ods->logmags = false;

    ods->odftype = odftype;
    ods->whtype = ODS_WH_ADAPT_MAX1;
    ods->fftformat = fftformat;

    ods->whiten = (odftype != ODS_ODF_MKL); // Deactivate whitening for MKL by default
    ods->detected = false;
    ods->med_odd = (medspan & 1) != 0;

    ods->medspan = medspan;

    ods->mingap = 0;
    ods->gapleft = 0;

    ods->fftsize = fftsize;
    ods->numbins = numbins;

    // printf("End of _init: normfactor is %g\n", ods->normfactor);
}

bool onsetsds_process(OnsetsDS* ods, float* fftbuf) {
    onsetsds_loadframe(ods, fftbuf);

    onsetsds_whiten(ods);
    onsetsds_odf(ods);
    onsetsds_detect(ods);

    return ods->detected;
}


void onsetsds_setrelax(OnsetsDS* ods, float time, size_t hopsize) {
    ods->relaxtime = time;
    ods->relaxcoef = (time == 0.0f) ? 0.0f : exp((ods_log1 * hopsize) / (time * ods->srate));
}


void onsetsds_loadframe(OnsetsDS* ods, float* fftbuf) {
    float *pos, *pos2, imag, real;
    int i;

    switch (ods->fftformat) {
    case ODS_FFT_SC3_POLAR:
        // The format is the same! dc, nyq, mag[1], phase[1], ...
        memcpy(ods->curr, fftbuf, ods->fftsize * sizeof(float));
        break;

    case ODS_FFT_SC3_COMPLEX:

        ods->curr->dc = fftbuf[0];
        ods->curr->nyq = fftbuf[1];

        // Then convert cartesian to polar:
        pos = fftbuf + 2;
        for (i = 0; i < (ods->numbins << 1); i += 2) {
            real = pos[i];
            imag = pos[i + 1]; // Plus 1 rather than increment; seems to avoid LSU reject on my PPC
            ods->curr->bin[i].mag = hypotf(imag, real);
            ods->curr->bin[i].phase = atan2f(imag, real);
        }
        break;

    case ODS_FFT_FFTW3_HC:

        ods->curr->dc = fftbuf[0];
        ods->curr->nyq = fftbuf[ods->fftsize >> 1];

        // Then convert cartesian to polar:
        // (Starting positions: real and imag for bin 1)
        pos = fftbuf + 1;
        pos2 = fftbuf + ods->fftsize - 1;
        for (i = 0; i < ods->numbins; i++) {
            real = *(pos++);
            imag = *(pos2--);
            ods->curr->bin[i].mag = hypotf(imag, real);
            ods->curr->bin[i].phase = atan2f(imag, real);
        }
        break;

    case ODS_FFT_FFTW3_R2C:

        ods->curr->dc = fftbuf[0];
        ods->curr->nyq = fftbuf[ods->fftsize];

        // Then convert cartesian to polar:
        pos = fftbuf + 2;
        for (i = 0; i < ods->numbins; i++) {
            real = *(pos++);
            imag = *(pos++);
            ods->curr->bin[i].mag = hypotf(imag, real);
            ods->curr->bin[i].phase = atan2f(imag, real);
        }
        break;
    }

    // Conversion to log-domain magnitudes, including re-scaling to aim back at the zero-to-one range.
    // Not well tested yet.
    if (ods->logmags) {
        for (i = 0; i < ods->numbins; i++) {
            ods->curr->bin[i].mag =
                (log(ods_max(ods->curr->bin[i].mag, ODS_LOG_LOWER_LIMIT)) - ODS_LOGOF_LOG_LOWER_LIMIT)
                * ODS_ABSINVOF_LOGOF_LOG_LOWER_LIMIT;
        }
        ods->curr->dc = (log(ods_max(ods_abs(ods->curr->dc), ODS_LOG_LOWER_LIMIT)) - ODS_LOGOF_LOG_LOWER_LIMIT)
            * ODS_ABSINVOF_LOGOF_LOG_LOWER_LIMIT;
        ods->curr->nyq = (log(ods_max(ods_abs(ods->curr->nyq), ODS_LOG_LOWER_LIMIT)) - ODS_LOGOF_LOG_LOWER_LIMIT)
            * ODS_ABSINVOF_LOGOF_LOG_LOWER_LIMIT;
    }
}

void onsetsds_whiten(OnsetsDS* ods) {
    float val, oldval, relaxcoef, floor;
    int numbins, i;
    OdsPolarBuf* curr;
    float* psp;
    float* pspp1; // Offset by 1, avoids quite a lot of "+1"s in the following code

    if (ods->whtype == ODS_WH_NONE) {
        // printf("onsetsds_whiten(): ODS_WH_NONE, skipping\n");
        return;
    }

    // NB: Apart from the above, ods->whtype is currently IGNORED and only one mode is used.

    relaxcoef = ods->relaxcoef;
    numbins = ods->numbins;
    curr = ods->curr;
    psp = ods->psp;
    pspp1 = psp + 1;
    floor = ods->floor;

    // printf("onsetsds_whiten: relaxcoef=%g, relaxtime=%g, floor=%g\n", relaxcoef, ods->relaxtime, floor);

    ////////////////////// For each bin, update the record of the peak value /////////////////////

    val = fabs(curr->dc); // Grab current magnitude
    oldval = psp[0];
    // If it beats the amplitude stored then that's our new amplitude;
    // otherwise our new amplitude is a decayed version of the old one
    if (val < oldval) {
        val = val + (oldval - val) * relaxcoef;
    }
    psp[0] = val; // Store the "amplitude trace" back

    val = fabs(curr->nyq);
    oldval = pspp1[numbins];
    if (val < oldval) {
        val = val + (oldval - val) * relaxcoef;
    }
    pspp1[numbins] = val;

    for (i = 0; i < numbins; ++i) {
        val = fabs(curr->bin[i].mag);
        oldval = pspp1[i];
        if (val < oldval) {
            val = val + (oldval - val) * relaxcoef;
        }
        pspp1[i] = val;
    }

    //////////////////////////// Now for each bin, rescale the current magnitude ////////////////////////////
    curr->dc /= ods_max(floor, psp[0]);
    curr->nyq /= ods_max(floor, pspp1[numbins]);
    for (i = 0; i < numbins; ++i) {
        curr->bin[i].mag /= ods_max(floor, pspp1[i]);
    }
}

void onsetsds_odf(OnsetsDS* ods) {
    int numbins = ods->numbins;
    OdsPolarBuf* curr = ods->curr;
    float* val = ods->odfvals;
    int i, tbpointer;
    float deviation, diff, curmag;
    double totdev;
    float predmag, predphase, yesterphase, yesterphasediff;
    float yestermag;


    bool rectify = true;

    // Here we shunt the "old" ODF values down one place
    memcpy(val + 1, val, (ods->medspan - 1) * sizeof(float));

    // Now calculate a new value and store in ods->odfvals[0]
    switch (ods->odftype) {
    case ODS_ODF_POWER:

        *val = (curr->nyq * curr->nyq) + (curr->dc * curr->dc);
        for (i = 0; i < numbins; i++) {
            *val += curr->bin[i].mag * curr->bin[i].mag;
        }
        break;

    case ODS_ODF_MAGSUM:

        *val = ods_abs(curr->nyq) + ods_abs(curr->dc);

        for (i = 0; i < numbins; i++) {
            *val += ods_abs(curr->bin[i].mag);
        }
        break;

    case ODS_ODF_COMPLEX:
        rectify = false;
        // ...and then drop through to:
    case ODS_ODF_RCOMPLEX:

        // Note: "other" buf is stored in this format: mag[0],phase[0],d_phase[0],mag[1],phase[1],d_phase[1], ...

        // Iterate through, calculating the deviation from expected value.
        totdev = 0.0;
        tbpointer = 0;
        for (i = 0; i < numbins; ++i) {
            curmag = ods_abs(curr->bin[i].mag);

            // Predict mag as yestermag
            predmag = ods->other[tbpointer++];
            yesterphase = ods->other[tbpointer++];
            yesterphasediff = ods->other[tbpointer++];

            // Thresholding as Brossier did - discard (ignore) bin's deviation if bin's power is minimal
            if (curmag > ods->odfparam) {
                // If rectifying, ignore decreasing bins
                if ((!rectify) || !(curmag < predmag)) {
                    // Predict phase as yesterval + yesterfirstdiff
                    predphase = yesterphase + yesterphasediff;

                    // Here temporarily using the "deviation" var to store the phase difference
                    //  so that the rewrap macro can use it more efficiently
                    deviation = predphase - curr->bin[i].phase;

                    // Deviation is Euclidean distance between predicted and actual.
                    // In polar coords: sqrt(r1^2 +  r2^2 - r1r2 cos (theta1 - theta2))
                    deviation = sqrtf(predmag * predmag + curmag * curmag
                                      - predmag * curmag * cosf(onsetsds_phase_rewrap(deviation)));

                    totdev += deviation;
                }
            }
        }

        // totdev will be the output, but first we need to fill tempbuf with today's values, ready for tomorrow.
        tbpointer = 0;
        for (i = 0; i < numbins; ++i) {
            ods->other[tbpointer++] = ods_abs(curr->bin[i].mag); // Storing mag
            diff = curr->bin[i].phase - ods->other[tbpointer]; // Retrieving yesterphase from buf
            ods->other[tbpointer++] = curr->bin[i].phase; // Storing phase
            // Wrap onto +-PI range
            diff = onsetsds_phase_rewrap(diff);

            ods->other[tbpointer++] = diff; // Storing first diff to buf
        }
        *val = (float)totdev;

        break;


    case ODS_ODF_PHASE:
        rectify = false; // So, actually, "rectify" means "useweighting" in this context
        // ...and then drop through to:
    case ODS_ODF_WPHASE:

        // Note: "other" buf is stored in this format: phase[0],d_phase[0],phase[1],d_phase[1], ...

        // Iterate through, calculating the deviation from expected value.
        totdev = 0.0;
        tbpointer = 0;
        for (i = 0; i < numbins; ++i) {
            // Thresholding as Brossier did - discard (ignore) bin's phase deviation if bin's power is low
            if (ods_abs(curr->bin[i].mag) > ods->odfparam) {
                // Deviation is the *second difference* of the phase, which is calc'ed as curval - yesterval -
                // yesterfirstdiff
                deviation = curr->bin[i].phase - ods->other[tbpointer] - ods->other[tbpointer + 1];
                tbpointer += 2;
                // Wrap onto +-PI range
                deviation = onsetsds_phase_rewrap(deviation);

                if (rectify) { // "rectify" meaning "useweighting"...
                    totdev += fabs(deviation * ods_abs(curr->bin[i].mag));
                } else {
                    totdev += fabs(deviation);
                }
            }
        }

        // totdev will be the output, but first we need to fill tempbuf with today's values, ready for tomorrow.
        tbpointer = 0;
        for (i = 0; i < numbins; ++i) {
            diff = curr->bin[i].phase - ods->other[tbpointer]; // Retrieving yesterphase from buf
            ods->other[tbpointer++] = curr->bin[i].phase; // Storing phase
            // Wrap onto +-PI range
            diff = onsetsds_phase_rewrap(diff);

            ods->other[tbpointer++] = diff; // Storing first diff to buf
        }
        *val = (float)totdev;
        break;


    case ODS_ODF_MKL:

        // Iterate through, calculating the Modified Kullback-Liebler distance
        totdev = 0.0;
        tbpointer = 0;
        for (i = 0; i < numbins; ++i) {
            curmag = ods_abs(curr->bin[i].mag);
            yestermag = ods->other[tbpointer];

            // Here's the main implementation of Brossier's MKL eq'n (eqn 2.9 from his thesis):
            deviation = ods_abs(curmag) / (ods_abs(yestermag) + ods->odfparam);
            totdev += log(1.f + deviation);

            // Store the mag as yestermag
            ods->other[tbpointer++] = curmag;
        }
        *val = (float)totdev;
        break;
    }

#if ODS_DEBUG_POST_CSV
    printf("%g,", *val);
    printf("%g,", ods->odfvals[0] * ods->normfactor);
#endif

    ods->odfvals[0] *= ods->normfactor;
}
// End of ODF function

void SelectionSort(float* array, int length);
void SelectionSort(float* array, int length) {
    // Algo is simply based on http://en.wikibooks.org/wiki/Algorithm_implementation/Sorting/Selection_sort
    int max, i;
    float temp;
    while (length > 0) {
        max = 0;
        for (i = 1; i < length; i++)
            if (array[i] > array[max])
                max = i;
        temp = array[length - 1];
        array[length - 1] = array[max];
        array[max] = temp;
        length--;
    }
}


void onsetsds_detect(OnsetsDS* ods) {
    float* sortbuf = ods->sortbuf;
    int medspan = ods->medspan;

    // Shift the yesterval to its rightful place
    ods->odfvalpostprev = ods->odfvalpost;

    ///////// MEDIAN REMOVAL ////////////

    // Copy odfvals to sortbuf
    memcpy(sortbuf, ods->odfvals, medspan * sizeof(float));

    // Sort sortbuf
    SelectionSort(sortbuf, medspan);

    // Subtract the middlest value === the median
    if (ods->med_odd) {
        ods->odfvalpost = ods->odfvals[0] - sortbuf[(medspan - 1) >> 1];
    } else {
        ods->odfvalpost = ods->odfvals[0] - ((sortbuf[medspan >> 1] + sortbuf[(medspan >> 1) - 1]) * 0.5f);
    }

    // Detection not allowed if we're too close to a previous detection.
    if (ods->gapleft != 0) {
        ods->gapleft--;
        ods->detected = false;
    } else {
        // Now do the detection.
        ods->detected = (ods->odfvalpost > ods->thresh) && (ods->odfvalpostprev <= ods->thresh);
        if (ods->detected) {
            ods->gapleft = ods->mingap;
        }
    }
#if ODS_DEBUG_POST_CSV
    printf("%g\n", ods->odfvalpost);
#endif
}
