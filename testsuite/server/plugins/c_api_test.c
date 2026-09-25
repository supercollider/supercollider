#include "SC_PlugIn.h"
#include "FFT_UGens.h"

InterfaceTable* ft;

typedef struct CPlugin {
    Unit unit;
    int m_fbufnum;
    float* buffer;
} CPlugin;

void CPlugin_next(CPlugin* x, int numSamples) {
    Unit* unit = &x->unit;

    // just add the two inputs
    const float* in1 = IN(0);
    const float* in2 = IN(1);
    float* out = OUT(0);
    for (int i = 0; i < numSamples; ++i) {
        out[i] = in1[i] + in2[i];
    }
}

void CPlugin_Ctor(CPlugin* x) {
    Unit* unit = &x->unit;

    x->m_fbufnum = -1;

    x->buffer = RTAlloc(unit->mWorld, sizeof(float) * FULLBUFLENGTH);
    ClearUnitIfMemFailed(x->buffer);
    memset(x->buffer, 0, sizeof(float) * FULLBUFLENGTH);

    SETCALC(CPlugin_next);
}

void CPlugin_Dtor(CPlugin* x) { RTFree(x->unit.mWorld, x->buffer); }

PluginLoad(ML_UGens) {
    ft = inTable;

    DefineDtorUnit(CPlugin);
}
