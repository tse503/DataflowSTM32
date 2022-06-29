#include "buttonC.h"

/* Read the keyboard button for C (GPIO C, Pin 2)
 * Output 0: BUFFER type: audio output
 */

void initButtonC() {
    // TODO: Abstract the hardware keyboard into one module with multiple outputs
    // Enable clock to GPIO C
    RCC->AHB1ENR = ((RCC->AHB1ENR & ~RCC_AHB1ENR_GPIOCEN_Msk) | (0x01 << RCC_AHB1ENR_GPIOCEN_Pos));

    // Set GPIO C Pin 2 Mode Register to 0b00 (Input)
    GPIOC->MODER = ((GPIOC->MODER & ~GPIO_MODER_MODER2_Msk) | (0x00 << GPIO_MODER_MODER2_Pos));

    // Set GPIO C Pin 2 Pull-Up/Push-Down Register to 0b01 (Pull-Up)
    GPIOC->PUPDR = ((GPIOC->PUPDR & ~GPIO_PUPDR_PUPD2_Msk) | (0x01 << GPIO_PUPDR_PUPD2_Pos));
}

// Return if button C is pressed
float isButtonCPressed() {
    if (GPIOC->IDR & GPIO_IDR_ID2_Msk) {
        return 0.0f;
    } else {
        return 1.0f;
    }
}

void BFLO_processButtonCModule(module_t * module) {
    // Read the state of the button
    float button = isButtonCPressed();

    // Set the module's output as the state of the button
    *(float *)(module->outputs[0].data) = button;
}

uint32_t BFLO_initButtonCModule(module_t * module, graph_t * graph, char * moduleName) {
    initButtonC;

    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 0);

    // Allocate memory for one control value in output 0
    module->outputs[0].data = malloc(sizeof(float));

    // Set initial output value to 0 (button not pressed)
    *(float *)(module->outputs[0].data) = 0;

    // Set module's associated process
    module->process = BFLO_processButtonCModule;

    // Set module's IO types   
    module->outputs[0].type = CONTROL;

    return 1;
}