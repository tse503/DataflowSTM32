#include "BFLO_internal.h"
#include "oscillatorLUT.h"
#include "LEDs.h"
#include <stm32f407xx.h>
#include <stdio.h>

void setup(void) {
    SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 4);
    initialiseLEDPorts();
}

int main(void) {
    setup();

    module_t osc0;
    graph_t synthGraph;

    BFLO_initGraph(&synthGraph);

    if(BFLO_initOcillatorLUTModule(&osc0, "Sine", 440.0f)) {
        printf("Oscillator module successfully initialsied.\n");
    }

    BFLO_insertModule(&synthGraph, &osc0);

    LEDOnColour(ALL_LED);

    return 0;
}