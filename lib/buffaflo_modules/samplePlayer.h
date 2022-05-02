#include "BFLO_internal.h"

#define MAX_SAMPLE_SIZE 22050

void BFLO_processSamplePlayerModule(module_t * module);

uint32_t BFLO_initSamplePlayerModule(module_t * module, graph_t * graph, char * moduleName, float samples[MAX_SAMPLE_SIZE]);