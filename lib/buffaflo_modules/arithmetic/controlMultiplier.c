#include "controlMultiplier.h"

void BFLO_processControlMultiplierModule(module_t * module) {
    float in0 = BFLO_getInputControl(module, 0);
    float in1 = BFLO_getInputControl(module, 1);

    *((float *)(module->outputs[0].data)) = in0 * in1;
}

uint32_t BFLO_initControlMultiplierModule(module_t * module, graph_t * graph, char * moduleName) {
    // Perform common module initialisation tasks - allocating memory for 2 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 2, 1, 0); 

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

    // Set module's associated process
    module->process = BFLO_processControlMultiplierModule;

    // Set module's IO types
    module->inputs[0].type = CONTROL;
    module->inputs[1].type = CONTROL;
    
    module->outputs[0].type = CONTROL;

    return 1;    
}
