#include "envelopeAR.h"

/* Attack-Release Envelope
 * Input 0: BUFFER type: audio input
 * Input 1: CONTROL type: reset - when TRUE (not equal to 0), restart envelope
 * Output 0: BUFFER type: audio output
 */

void BFLO_processEnvelopeARModule(module_t * module) {
    float volatile reset, attackSamples, releaseSamples, currentSample;

    // Pointer to the incoming audio buffer
    float * buffer0 = BFLO_getInputBuffer(module, 0);

    // Get reset input 
    reset = BFLO_getInputControl(module, 1);

    // Get internal module parameters
    attackSamples = *(float *)module->parameters[0].data;
    releaseSamples = *(float *)module->parameters[1].data;
    currentSample = *(float *)module->parameters[2].data;

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        // If in attack stage, ramp up to maximum
        if (currentSample < attackSamples) {

        }
    }
}

uint32_t BFLO_initEnvelopeARModule(module_t * module, graph_t * graph, char * moduleName, float initAttackSamples, float initReleaseSamples) {
    // Perform common module initialisation tasks - allocating memory for 2 inputs, 1 output, 3 parameters
    BFLO_initModule(module, graph, moduleName, 2, 1, 3);

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

    // Allocate memory for 3 floats in each parameter's data member
    module->parameters[0].data = malloc(sizeof(float));
    module->parameters[1].data = malloc(sizeof(float));
    module->parameters[2].data = malloc(sizeof(float));

    // Set parameter names
    strncpy(module->parameters[0].name, "Attack Time (Samples)", MAX_NAME_LENGTH);
    strncpy(module->parameters[1].name, "Release Time (Samples)", MAX_NAME_LENGTH);
    strncpy(module->parameters[2].name, "Envelope Position", MAX_NAME_LENGTH);

    // Set initial parameter values
    *(float *)(module->parameters[0].data) = initAttackSamples;
    *(float *)(module->parameters[1].data) = initReleaseSamples;
    *(float *)(module->parameters[2].data) = 0.0f;

    // Set module's associated process
    module->process = BFLO_processEnvelopeARModule;

    // Set module's IO types
    module->inputs[0].type = BUFFER;
    module->inputs[1].type = CONTROL;
    
    module->outputs[0].type = BUFFER;

    return 1;
}