#include "controlMultiplier.h"

void BFLO_processControlMultiplierModule(module_t * module) {
    float in0 = BFLO_getInputControl(module, 0);
    float in1 = BFLO_getInputControl(module, 1);

    *((float *)(module->outputs[0].data)) = in0 + in1;

}

uint32_t BFLO_initControlMultiplierModule(module_t * module, char * moduleName) {
    // Set module's name
    strncpy(module->name, moduleName, MAX_NAME_LENGTH);

    // Initialise all module flags as clear
    module->status = 0;

    // Set number of IO and parameters
    module->numInputs = 2;
    module->numOutputs = 1;
    module->numParameters = 0;

    // Allocate memory for two input_t structs
    module->inputs = calloc(2, sizeof(input_t));

    // Allocate memory for one output_t struct
    module->outputs = malloc(sizeof(output_t));

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
