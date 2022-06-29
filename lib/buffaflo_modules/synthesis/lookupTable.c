#include "lookupTable.h"

uint32_t BFLO_initLookupTableModule(module_t * module, graph_t * graph, char * moduleName, table_t * initTable) {
    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 0); 

    // Allocate memory for one pointer to a lookup table value in output 0
    module->outputs[0].data = malloc(sizeof(table_t *));

    // Set pointer to initial lookup table
    module->outputs[0].data = initTable;

    // Set module's associated process
    module->process = BFLO_doNothing;

    // Set module's IO types   
    module->outputs[0].type = TABLE;

    return 1;
}