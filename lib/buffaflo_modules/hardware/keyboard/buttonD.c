#include "buttonC.h"

/* Read the keyboard button for C (GPIO C, Pin 2)
 * Output 0: BUFFER type: audio output
 */

void initButtonD(void) {
    // TODO: Abstract the hardware keyboard into one module with multiple outputs
    // Enable clock to GPIO E
    RCC->AHB1ENR = ((RCC->AHB1ENR & ~RCC_AHB1ENR_GPIOEEN_Msk) | (0x01 << RCC_AHB1ENR_GPIOEEN_Pos));

    // Set GPIO E Pin 9 Mode Register to 0b00 (Input)
    GPIOE->MODER = ((GPIOE->MODER & ~GPIO_MODER_MODER9_Msk) | (0x00 << GPIO_MODER_MODER9_Pos));

    // Set GPIO E Pin 9 Pull-Up/Push-Down Register to 0b01 (Pull-Up)
    GPIOE->PUPDR = ((GPIOE->PUPDR & ~GPIO_PUPDR_PUPD9_Msk) | (0x01 << GPIO_PUPDR_PUPD9_Pos));
}

// Return if button D is pressed
float isButtonDPressed(void) {
    if (GPIOE->IDR & GPIO_IDR_ID9_Msk) {
        return 0.0f;
    } else {
        return 1.0f;
    }
}

void BFLO_processButtonDModule(module_t * module) {
    // Read the state of the button
    float button = isButtonDPressed();

    // Set the module's output as the state of the button
    *(float *)(module->outputs[0].data) = button;
}

uint32_t BFLO_initButtonDModule(module_t * module, graph_t * graph, char * moduleName) {
    initButtonD();

    // Perform common module initialisation tasks - allocating memory for 0 inputs, 1 output, 0 parameters
    BFLO_initModule(module, graph, moduleName, 0, 1, 0);

    // Allocate memory for one control value in output 0
    module->outputs[0].data = malloc(sizeof(float));

    // Set initial output value to 0 (button not pressed)
    *(float *)(module->outputs[0].data) = 0;

    // Set module's associated process
    module->process = BFLO_processButtonDModule;

    // Set module's IO types   
    module->outputs[0].type = CONTROL;

    return 1;
}