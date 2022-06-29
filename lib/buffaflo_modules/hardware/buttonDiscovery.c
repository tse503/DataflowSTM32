#include "buttonDiscovery.h"

/* Read the Discovery's onboard user button
 * Output 0: BUFFER type: audio output
 */

void BFLO_processButtonDiscoveryModule(module_t * module) {
    // Read the state of the button
    float button = (float)isUserButtonPressed();

    // Set the module's output as the state of the button
    *(float *)(module->outputs[0].data) = button;
}

uint32_t BFLO_initButtonDiscoveryModule(module_t * module, graph_t * graph, char * moduleName) {
    initUserButton();

    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 0);

    // Allocate memory for one control value in output 0
    module->outputs[0].data = malloc(sizeof(float));

    // Set initial output value to 0
    *(float *)(module->outputs[0].data) = 0;

    // Set module's associated process
    module->process = BFLO_processButtonDiscoveryModule;

    // Set module's IO types   
    module->outputs[0].type = CONTROL;

    return 1;
}