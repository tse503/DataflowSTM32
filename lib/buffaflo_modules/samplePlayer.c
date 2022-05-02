#include "samplePlayer.h"

void BFLO_processSamplePlayerModule(module_t * module) {
    uint32_t volatile currentSample = *(uint32_t *)(module->parameters[0].data);
    float * samples = (module->parameters[1].data);

    /* Fill output buffer with values from look-up table */
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        // Once sample has been played, output silence 
        if (currentSample > MAX_SAMPLE_SIZE) {
            ((float *)(module->outputs[0].data))[i] = 0;
        } 
        // Otherwise, play fill audio buffer with samples
        else {
            ((float *)(module->outputs[0].data))[i] = samples[currentSample];
            currentSample++;
        }

    }

    // Update the module's current phase internal parameter
    *(float *)module->parameters[0].data = currentSample;
}

uint32_t BFLO_initSamplePlayerModule(module_t * module, graph_t * graph, char * moduleName, float samples[MAX_SAMPLE_SIZE]) {
    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 2 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 2); 

    // Allocate memory for one buffer in output 0
    module->outputs[0].data = calloc(BUFFER_SIZE, sizeof(float));

    // Allocate memory for parameter's data members
    module->parameters[0].data = malloc(sizeof(uint32_t));
    module->parameters[1].data = malloc(sizeof(float *));

    // Set parameter names
    strncpy(module->parameters[0].name, "Current Sample", MAX_NAME_LENGTH);
    strncpy(module->parameters[1].name, "Samples Pointer", MAX_NAME_LENGTH);

    // Set initial parameter values
    *(uint32_t *)(module->parameters[0].data) = 0;
    (module->parameters[1].data) = samples;

    // Set module's associated process
    module->process = BFLO_processSamplePlayerModule;

    // Set module's IO types
    module->outputs[0].type = BUFFER;

    return 1;
}
