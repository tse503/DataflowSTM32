#include "oscillatorLUT.h"
#include <stdlib.h>
#include <string.h>

/* Lookup table oscillator
 * Input 0: CONTROL type, frequency of oscillator in Hertz
 * Input 1: TABLE type, lookup table to play back
 * Output 0: BUFFER type, audio output
 */

void BFLO_processOscillatorLUTModule(module_t * module) {
    float volatile frequency, currentPhase, phaseIncrement;

    // Get internal module parameters
    frequency = BFLO_getInputControl(module, 0);
    phaseIncrement = (frequency * 1024) / 44100;    // TODO: Replace magic numbers
    currentPhase = *(float *)module->parameters[1].data;

    /* Fill output buffer with values from look-up table */
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        currentPhase += phaseIncrement;
        
        // If current phase exceeds look-up table size, wrap-around
        if (currentPhase > SINESIZE) {
            currentPhase -= SINESIZE;
        }

        // Set the next sample in the output buffer as the value in the look-up table at the current phase 
        ((float *)(module->outputs[0].data))[i] = SineLUT[(uint16_t)(currentPhase)];
    }

    // Update the module's current phase internal parameter
    *(float *)module->parameters[1].data = currentPhase;
}

uint32_t BFLO_initOcillatorLUTModule(module_t * module, graph_t * graph, char * moduleName, float initFrequency) {
    // // Set module's name
    // strncpy(module->name, moduleName, MAX_NAME_LENGTH);

    // // Initialise all module flags as clear
    // module->status = 0;

    // // Set number of IO and parameters
    // module->numInputs = 2;
    // module-> numOutputs = 1;
    // module->numParameters = 3;

    // // Allocate memory for two input_t structs
    // module->inputs = calloc(2, sizeof(input_t));

    // // Allocate memory for one output_t struct
    // module->outputs = malloc(sizeof(output_t));

    // // Allocate memory for 3 parameter structs
    // module->parameters = calloc(module->numParameters, sizeof(parameter_t));

    // Perform common module initialisation tasks - allocating memory for 2 inputs, 1 output, 3 parameters
    BFLO_initModule(module, graph, moduleName, 2, 1, 3); 

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

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
    *(float *)(module->parameters[2].data) = (initFrequency * 1024) / 44100;    // TODO: Replace magic numbers // QUESTION: Does this value actually need to be stored? Recomputed in procees loop anyway 

    // Set module's associated process
    module->process = BFLO_processOscillatorLUTModule;

    // Set module's IO types
    module->inputs[0].type = CONTROL;
    module->inputs[1].type = TABLE;
    
    module->outputs[0].type = BUFFER;

    // TODO: Remove this when LUT received externally
    // Set up the sine look-up table:
	for (int i = 0; i < SINESIZE; i++) {
		float q = 32760 * sin(i * 2.0 * PI / SINESIZE);
		SineLUT[i] = q;
	}

    return 1;
}
