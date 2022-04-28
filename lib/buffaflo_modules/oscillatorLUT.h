#include "BFLO_internal.h"
#include <math.h>

// TODO: Remove these when table received externally
#define LUTSIZE 1024
#define PI 3.141592653589793 
float SineLUT[LUTSIZE];
float SawUpLUT[LUTSIZE];
float SawDownLUT[LUTSIZE];

void BFLO_processOscillatorLUTModule(module_t * module);

uint32_t BFLO_initOcillatorLUTModule(module_t * module, graph_t * graph, char * moduleName, float initFrequency);
