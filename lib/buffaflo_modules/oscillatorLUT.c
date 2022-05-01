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

    table_t * inTable = BFLO_getInputTable(module, 1);

    // Get internal module parameters to use in the processing stage
    frequency = BFLO_getInputControl(module, 0);
    phaseIncrement = (frequency * 1024) / 44100;    // TODO: Replace magic numbers
    currentPhase = *(float *)module->parameters[1].data;

    /* Fill output buffer with values from look-up table */
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        currentPhase += phaseIncrement;
        
        // If current phase exceeds look-up table size, wrap-around
        if (currentPhase >= LUTSIZE) {
            currentPhase -= LUTSIZE;
        }

        // Set the next sample in the output buffer as the value in the look-up table at the current phase 
        ((float *)(module->outputs[0].data))[i] = inTable->samples[(uint16_t)(currentPhase)];
    }

    // Update the module's current phase internal parameter
    *(float *)module->parameters[1].data = currentPhase;
}

uint32_t BFLO_initOcillatorLUTModule(module_t * module, graph_t * graph, char * moduleName, float initFrequency) {
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
    *(float *)(module->parameters[2].data) = (initFrequency * MAX_LUT_SIZE) / 44100;    // TODO: Replace magic numbers // QUESTION: Does this value actually need to be stored? Recomputed in procees loop anyway 

    // Set module's associated process
    module->process = BFLO_processOscillatorLUTModule;

    // Set module's IO types
    module->inputs[0].type = CONTROL;
    module->inputs[1].type = TABLE;
    
    module->outputs[0].type = BUFFER;

    return 1;
}
