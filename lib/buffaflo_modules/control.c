#include "control.h"
#include <stdlib.h>
#include <string.h>

uint32_t BFLO_initControlModule(module_t * module, graph_t * graph, char * moduleName, float initValue) {
    // // Set module's name
    // strncpy(module->name, moduleName, MAX_NAME_LENGTH);

    // // Initialise all module flags as clear
    // module->status = 0;

    // // Set number of IO and parameters
    // module->numInputs = 0;
    // module-> numOutputs = 1;
    // module->numParameters = 0;
    
    // // Allocate memory for one output_t struct
    // module->outputs = malloc(sizeof(output_t));

    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 0); 

    // Allocate memory for one control value in output 0
    module->outputs[0].data = malloc(sizeof(float));

    // Set initial output value
    *(float *)(module->outputs[0].data) = initValue;

    // Set module's associated process
    module->process = BFLO_doNothing;

    // Set module's IO types   
    module->outputs[0].type = CONTROL;

    return 1;
}