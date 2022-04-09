#include "oscillatorLUT.h"
#include <stdlib.h>
#include <string.h>

/* Lookup table oscillator
 * Input 0: CONTROL type, frequency of oscillator in Hertz
 * Input 1: TABLE type, lookup table to play back
 * Output 0: BUFFER type, audio output
 */

void BFLO_processOscillatorLUTModule(module_t * module) {

}

uint32_t BFLO_initOcillatorLUTModule(module_t * module, char * moduleName, float initFrequency) {
    // Set module's name
    strncpy(module->name, moduleName, MAX_NAME_LENGTH);

    // Initialise all module flags as clear
    module->status = 0;

    // Set number of IO and parameters
    module->numInputs = 2;
    module-> numOutputs = 1;
    module->numParameters = 3;

    // Allocate memory for two input_t structs
    module->inputs = calloc(2, sizeof(input_t));

    // Allocate memory for one output_t struct
    module->outputs = malloc(sizeof(output_t));

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

    // Allocate memory for 3 parameter structs
    module->parameters = calloc(module->numParameters, sizeof(parameter_t));

    // Allocate memory for 3 floats in each parameter's data member
    module->parameters[0].data = malloc(sizeof(float));
    module->parameters[1].data = malloc(sizeof(float));
    module->parameters[2].data = malloc(sizeof(float));

    // Set parameter names
    strncpy(module->parameters[0].name, "Frequency", MAX_NAME_LENGTH);
    strncpy(module->parameters[1].name, "Current Phase", MAX_NAME_LENGTH);
    strncpy(module->parameters[2].name, "Phase increment", MAX_NAME_LENGTH);

    // Set initial parameter values
    *(float *)(module->parameters[0].data) = initFrequency;
    *(float *)(module->parameters[1].data) = 0.0f;
    *(float *)(module->parameters[2].data) = 0.0f;   // TODO: Set actual pahse increment for initial frequency value     
         

    // Set module's associated process
    module->process = BFLO_processOscillatorLUTModule;

    // Set module's IO types
    module->inputs[0].type = CONTROL;
    module->inputs[1].type = TABLE;
    
    module->outputs[0].type = BUFFER;

    return 1;
}

