#include "bufferAdder.h"

void BFLO_processBufferAdderModule(module_t * module) {
    float * buffer0 = BFLO_getInputBuffer(module, 0);
    float * buffer1 = BFLO_getInputBuffer(module, 1);

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        ((float *)(module->outputs[0].data))[i] = (buffer0[i]) + (buffer1[i]);
    }
}

uint32_t BFLO_initBufferAdderModule(module_t * module, graph_t * graph, char * moduleName) {
    // Perform common module initialisation tasks - allocating memory for 2 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 2, 1, 0); 

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

    // Set module's associated process
    module->process = BFLO_processBufferAdderModule;

    // Set module's IO types
    module->inputs[0].type = BUFFER;
    module->inputs[1].type = BUFFER;
    
    module->outputs[0].type = BUFFER;

    return 1;    
}
