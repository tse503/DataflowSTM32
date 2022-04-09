#ifndef DATAFLOW_H
#define DATAFLOW_H

#include <stdint.h>

#define BUFFER_SIZE 8
#define MAX_GRAPH_SIZE 8
#define MAX_NAME_LENGTH 32

enum IO_type {BUFFER, CONTROL, TABLE};
// TODO: enum for parameter type?

// TODO: Use defintions for status flag bit shift positions

/*~~~~~ Status flags ~~~~~*/
#define STATUS_FLAG_ORDERED (1UL << 0)   // When true, module has been ordered correctly during topological ordering of graph
#define STATUS_FLAG_ENQUEUED (1UL << 1)  // When true, module has been enqueued during Kahn's topological ordering algorithm 

/*~~~~~ Structure Definitions ~~~~~*/

typedef struct { // QUESTION: Separate array for parameter and operation modules? 
    uint32_t size;                          // Current count of modules connected in the graph                    
    struct module * modules[MAX_GRAPH_SIZE];     // Array to hold pointer-to-modules
} graph_t;

typedef struct {
    enum IO_type type;          // Description of input data type
    struct module * source;     // Pointer to source module that data is received from
    uint32_t index;             // Index of source module's output to receive data from
} input_t;

typedef struct {
    enum IO_type type;  // Description of input data type // TODO: Make this const?
    void * data;        // Pointer to memory where output data is stored  // QUESTION: Use union instead?
} output_t;

/*  */
typedef struct {
    char name[MAX_NAME_LENGTH]; // Name of the parameter // TODO: Make this const?
    void * data;                // Pointer to memory where parameter data is stored
} parameter_t;

typedef struct module { // QUESTION: Is this kind of forward-declaration sutiable for resolving circular dependencies between module_t and input_t?
    char name[MAX_NAME_LENGTH];         // String module name // TODO: Make this const?
    uint32_t status;                    // Holds boolean flags about the module's status
    uint32_t numInputs;                 
    uint32_t numOutputs;                
    uint32_t numParameters;             
    input_t * inputs;                   // Module's inputs
    output_t * outputs;                 // Module's outputs
    parameter_t * parameters;           // Module's internal parameters
    void (*process)(struct module *);   // Function pointer to module's process function QUESTION: Should this be a const?
    graph_t * graph;                    // Pointer to graph that contains the module
} module_t;

/*~~~~~ Utility Functions  ~~~~~*/

void BFLO_connectModules(module_t * sourceModule, uint32_t outputIndex, module_t * sinkModule, uint32_t inputIndex);

float BFLO_getInputControl(module_t * module, uint32_t inputIndex);
float BFLO_getOutputControl(module_t * module, uint32_t outputIndex);
void BFLO_setOutputControl(module_t * module, uint32_t outputIndex, float value);

float * BFLO_getInputBuffer(module_t * module, uint32_t inputIndex);
float * BFLO_getOutputBuffer(module_t * module, uint32_t outputIndex);

void BFLO_doNothing(module_t * module);

void copyBufferContents(float * dest, float * source);
void displayBuffer(float * buffer);

uint32_t readFlag(module_t * module, uint32_t flag);
void setFlag(module_t * module, uint32_t flag);

/*~~~~~ Graph Utility Functions  ~~~~~*/

void BFLO_initGraph(graph_t * graph);

void BFLO_insertModule(graph_t * graph, module_t * module);

void BFLO_depthFirstSearch(module_t * module, module_t ** ordering, uint32_t * orderingIndex);
void BFLO_orderGraphDFS(graph_t * graph);
void BFLO_orderGraphKahn(graph_t * graph);

void BFLO_processGraph(graph_t * graph);

uint32_t BFLO_getGraphSize(graph_t * graph);


#endif /* DATAFLOW_H */