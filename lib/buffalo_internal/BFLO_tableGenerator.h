#include "BFLO_internal.h"
#include <math.h>

enum BFLO_WAVEFORM {
    SINE        = 0x00u,
    SAWTOOTH    = 0x01U,
    SQUARE      = 0x02U,
    TRIANGLE    = 0x03U
};

void BFLO_generateLUT(float * table, enum BFLO_WAVEFORM waveform);