#include "bufferAdder.h"

void BFLO_processBufferAdderModule(module_t * module) {
    float * buffer0 = BFLO_getInputBuffer(module, 0);
    float * buffer1 = BFLO_getInputBuffer(module, 1);

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        ((float *)(module->outputs[0].data))[i] = (buffer0[i]) + (buffer1[i]);
    }
}

uint32_t BFLO_initBufferAdderModule(module_t * module, char * moduleName) {
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
    module->process = BFLO_processBufferAdderModule;

    // Set module's IO types
    module->inputs[0].type = BUFFER;
    module->inputs[1].type = BUFFER;
    
    module->outputs[0].type = BUFFER;

    return 1;    
}
