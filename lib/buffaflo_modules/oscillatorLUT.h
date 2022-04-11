#include "BFLO_internal.h"
#include <math.h>

// TODO: Remove these when table received externally
#define SINESIZE 1024
#define PI 3.141592653589793 
float SineLUT[SINESIZE];

void BFLO_processOscillatorLUTModule(module_t * module);

uint32_t BFLO_initOcillatorLUTModule(module_t * module, char * moduleName, float initFrequency);
