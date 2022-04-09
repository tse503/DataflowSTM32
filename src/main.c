#include "BFLO_internal.h"
#include "oscillatorLUT.h"
#include "LEDs.h"
#include "user_button.h"
#include <stm32f407xx.h>
#include <stdio.h>

void SysTick_Handler(void) {
    
}

void setupClockAndPeripherals(void) {
    SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock);
    initLEDs();
    initUserButton();
}

int main(void) {
    setupClockAndPeripherals();

    graph_t synthGraph;
    module_t osc0;

    BFLO_initGraph(&synthGraph);
    BFLO_initOcillatorLUTModule(&osc0, "Sine", 440.0f);

    while (1) {
        if (GPIOA->IDR & GPIO_IDR_ID0_Msk) {
            LEDOnColour(ALL_LED);        
        } else {
            LEDOffColour(ALL_LED); 
        }    
    }
}
