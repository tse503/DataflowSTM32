#include "BFLO_internal.h"
#include <stdio.h>
#include <stdlib.h>

/*~~~~~ Utility Functions  ~~~~~*/

/* Connect the specified source module's output to the specified sink modules' input */
void BFLO_connectModules(module_t * sourceModule, uint32_t outputIndex, module_t * sinkModule, uint32_t inputIndex) {
    // Check source and sink modules are in the same graph
    if (sourceModule->graph == sinkModule->graph) {
        // Check source output and sink input have the same data type 
        if (sourceModule->outputs[outputIndex].type == sinkModule->inputs[inputIndex].type) {
            // Connect modules and update sink modules pointer to source module
            sinkModule->inputs[inputIndex].source = sourceModule;
            sinkModule->inputs[inputIndex].index = outputIndex;
        }
    }   
}

/* Returns the incoming control value at the specified module's input, performing safety checks at the cost of increased overhead */
float BFLO_getInputControl(module_t * module, uint32_t inputIndex) {
    if (module->numInputs == 0) {
        printf(module->name);
        printf(" has no inputs!\n");
        // TODO: Deal with this
        return -1.0;
    }

    // QUESTION: Nested if or && preferable?
    // Check specified input index is within bounds of module's inputs
    if (inputIndex < module->numInputs) {
        // Check requested type matches input's type
        if (module->inputs[inputIndex].type == CONTROL) {
            module_t * sourceModule = module->inputs[inputIndex].source;
            uint32_t outIndex = module->inputs[inputIndex].index;

            // Cast void pointer to pointer-to-integer then dereference to get value
            float value = *(float *)(sourceModule->outputs[outIndex].data); // QUESTION: Could just return this line...
            return value;
        } else {
            printf("Module input %ld is not of type CONTROL! Returning 0\n", inputIndex);
            // QUESTION: What should I return if function is not valid?
            return -1.0;
        }
    } else {
        printf(module->name);
        printf("Specified input index %ld exceeds module's input count: %ld\n", inputIndex, module->numInputs);
        return -1.0;
    }

    // TODO: check module is connected and/or input isn't null pointer
    // QUESTION: are these TODO checks necessary? would it add increased overhead when used in audio processing functions?   
    
}

/* Get the control output at the specified output index of the specified module
 * Useful for checking if the output of a module is what is expected
 */
float BFLO_getOutputControl(module_t * module, uint32_t outputIndex) {
    if (outputIndex < module->numOutputs) {
        if (module->outputs[outputIndex].type == CONTROL) {
            return *(float *)(module->outputs[outputIndex].data);
        }
    }
    // TODO: Return/error if specified outputIndex is out of bounds
}

/* Set the value of the specified module's control output */
void BFLO_setOutputControl(module_t * module, uint32_t outputIndex, float value) {
    *(float *)(module->outputs[outputIndex].data) = value;
}

/* Gets a pointer to the incoming buffer of module at inputIndex */ 
float * BFLO_getInputBuffer(module_t * module, uint32_t inputIndex) {
    // Check specified input index is within bounds of module's inputs
    if (inputIndex < module->numInputs) {
        // Check requested type matches input's type
        if (module->inputs[inputIndex].type == BUFFER) {
            module_t * sourceModule = module->inputs[inputIndex].source;
            uint32_t outIndex = module->inputs[inputIndex].index;

            // Cast void pointer to pointer-to-float
            float * buffer = (float *)(sourceModule->outputs[outIndex].data); // QUESTION: Could just return this line...
            return buffer;
        }
    }
}

float * BFLO_getOutputBuffer(module_t * module, uint32_t outputIndex) {
    return (float *)(module->outputs[outputIndex].data);
}

/* Function that does nothing. Used in process member for modules that don't perform a process. */
void BFLO_doNothing(module_t * module) {
    return;
}

/* Copy contents of source buffer to destination buffer */
void copyBufferContents(float * dest, float * source) {
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        dest[i] = source[i];
    }
}

/* Print each element in a buffer */
void displayBuffer(float * buffer) {
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        printf("Element %ld = %f\n", i, buffer[i]);
    }
}

/* Read the value of the specified flag 0-31*/
uint32_t readFlag(module_t * module, uint32_t flag) {
    
}

/* Set the specified flag 0-31 to true (1)*/
void setFlag(module_t * module, uint32_t flag) {
    
}

/*~~~~~ Graph Utility Functions  ~~~~~*/

/* Initialise the graph's variables */
void BFLO_initGraph(graph_t * graph) {
    graph->size = 0;
}

/* Insert the module into the graph and increment its size by 1 */
void BFLO_insertModule(graph_t * graph, module_t * module) {
    // TODO: Check module isn't already in graph...
    // Check graph is not full
    if (graph->size < MAX_GRAPH_SIZE) {
        // Set the module's graph struct to store what graph the module is stored in
        (module->graph) = graph;

        // Put the module in the graph's modules array and increment the graph size by 1
        graph->modules[(graph->size)++] = module;
    } else {
        printf("Graph is full!\n");
    }
}

/* Recursive function used to check if a module is ready to be ordered in the topological search */
void BFLO_depthFirstSearch(module_t * module, module_t ** ordering, uint32_t * orderingIndex) {
    printf("Searching module: ");
    printf(module->name);
    printf("\n");
    // Using a bitwise AND, check module isn't already flagged as ordered
    if ((module->status & STATUS_FLAG_ORDERED) == 0) {
        // Check if module is a root in the graph
        if (module->numInputs == 0) {
            printf("No inputs! Adding to ordering...\n");
            // Place the module in the ordering at the next available index
            ordering[*orderingIndex] = module;
            // Increment the next available index
            (*orderingIndex)++;
            // Set ordered flag in module's status 
            module->status |= STATUS_FLAG_ORDERED;
        } else {
            printf("Module has dependencies!\n");
            // Recursively search through and order each of the module's dependencies
            for (uint32_t i = 0; i < module->numInputs; i++) {
                BFLO_depthFirstSearch(module->inputs[i].source, ordering, orderingIndex);
            }

            printf("Parents ordered! Adding to ordering...\n");

            // Once parents ordered, add module to ordering and increment ordering index QUESTION: Elimiate code reuse here? No need to check if numInputs == 0
            ordering[(*orderingIndex)++] = module;

            // Set ordered flag in module's status 
            module->status |= STATUS_FLAG_ORDERED;
        }
    } else {
        printf(module->name);
        printf(" already ordered!\n");
    }    
}

/* Depth-first search topological sort algorithm directed acyclic graph's modules into correct precedence order. */ 
void BFLO_orderGraphDFS(graph_t * graph) {
    // Array of pointer-to-modules to hold ordering 
    module_t * order[MAX_GRAPH_SIZE];

    // Holds next free index in the order
    uint32_t orderIndex = 0;

    // Perform depth-first search and sorting on modules in graph
    for (uint32_t i = 0; i < graph->size; i++) {
        BFLO_depthFirstSearch(graph->modules[i], order, &orderIndex);
    }

    // Print ordering
    for (uint32_t i = 0; i < orderIndex; i++) {
        printf("Ordered at %ld: ", i);
        printf(order[i]->name);
        printf("\n");
    }

    // Update the ordering of the modules in the graph
    for (uint32_t i = 0; i < graph->size; i++) {
        graph->modules[i] = order[i];
    }
    // QUESTION: Reorder graph's modules member or have a new member for holding ordering (would require twice as many pointer-to-modules...)     
}

/* Kahn's topological sort algorithm directed acyclic graph's modules into correct precedence order. */ 
void BFLO_orderGraphKahn(graph_t * graph) {
    // TODO: Remove commented out printf statements

    // Array of pointer-to-modules to hold ordering 
    module_t * order[MAX_GRAPH_SIZE];

    // Holds modules to order during algorithm iterations, initialise the first value to NULL until a root module is found 
    module_t * queue[MAX_GRAPH_SIZE];
    queue[0] = NULL;

    // Stores the amount of unordered dependencies (the degree) for each module - index of degree corresponds to index of module in graph 
    uint32_t degrees[MAX_GRAPH_SIZE];

    // Holds next free index in the order
    uint32_t orderIndex = 0;

    // Holds the index for modules to be enqueud or dequeued in the queue
    uint32_t enqueueIndex = 1;
    uint32_t dequeueIndex = 0;

    // Identify root modules and enqueue them
    for (uint32_t i = 0; i < graph->size; i++) {
        // If module has no dependencies add it to the queue and set its enqueued status flag
        if (graph->modules[i]->numInputs == 0) {
            // Set the module as the starting root module if one hasn't been identified
            if (queue[0] == NULL) {
                queue[0] = graph->modules[i];
            } else {
                // If module has no dependencies and a root module is determined
                queue[enqueueIndex++] = graph->modules[i];
            }

            // Set the enqueud flag in each module added
            graph->modules[i]->status |= STATUS_FLAG_ENQUEUED;
        }
        
        printf(graph->modules[i]->name);
        printf(" identify root loop status = %ld\n", graph->modules[i]->status);

        // Set the starting degree for each module
        degrees[i] = graph->modules[i]->numInputs;
        // printf("Degree of module %d = %d\n", i, graph->modules[i]->numInputs);
    }

    // Iterate through modules in the queue
    while (orderIndex < graph->size) {
        // Set ordered flag and clear enqueued flag of dequeued module
        printf("Dequeueing "); printf(queue[dequeueIndex]->name); printf("\n");
        queue[dequeueIndex]->status |= STATUS_FLAG_ORDERED;
        queue[dequeueIndex]->status &= ~(STATUS_FLAG_ENQUEUED);

        // Place the module at the front of the queue into the ordering and increment both the ordering and dequeue indices
        order[orderIndex++] = queue[dequeueIndex++];

        // Iterate through each module to update how many dependenices it now has
        for (uint32_t i = 0; i < graph->size; i++) {
            // Reset degrees, 
            degrees[i] = 0;
            // Iterate through the module's dependencies
            for (uint32_t j = 0; j < graph->modules[i]->numInputs; j++) {
                // For every dependency not in the ordering, add 1 to the module's degree
                // TODO: Double check this, would an if check be better if STATUS_FLAG_ORDERED no longer = 1?
                degrees[i] += !(graph->modules[i]->inputs[j].source->status & STATUS_FLAG_ORDERED);
            }
            
            // If a module that is not already ordered or enqueued now has a degree of 0, add it to the queue
            if ((degrees[i] == 0) && ((graph->modules[i]->status & STATUS_FLAG_ORDERED) == 0) && ((graph->modules[i]->status & STATUS_FLAG_ENQUEUED) == 0)) {
                queue[enqueueIndex++] = graph->modules[i];
                graph->modules[i]->status |= STATUS_FLAG_ENQUEUED;
                // printf(graph->modules[i]->name); printf(" added to the queue\n");
            }
        }
    }

    // Print ordering
    for (uint32_t i = 0; i < graph->size; i++) {
        printf("Ordered at %ld: ", i);
        printf(order[i]->name);
        printf("\n");
    }

    // Update the ordering of the modules in the graph
    for (uint32_t i = 0; i < graph->size; i++) {
        graph->modules[i] = order[i];
    }
    // QUESTION: Reorder graph's modules member or have a new member for holding ordering (would require twice as many pointer-to-modules...)     
}

/* Recursive function used to */
void BFLO_processGraph(graph_t * graph) {
    for (uint32_t i = 0; i < graph->size; i++) {
        graph->modules[i]->process(graph->modules[i]);
    }   
}

/* Return the current number of modules in the graph */
uint32_t BFLO_getGraphSize(graph_t * graph) {
    return graph->size;
}



