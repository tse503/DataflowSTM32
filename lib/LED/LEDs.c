#include "LEDs.h"
#include <stm32f407xx.h>

/* Enable peripheral clock register to on-board LEDs and set mode registers */
void initialiseLEDPorts() {
	// Set bit 3 of RCC_AHB1ENR register to 1 in RCC AHB1 peripheral clock register - enables clock to GPIO D port, corresponding to LED peripherals
	RCC->AHB1ENR = (RCC->AHB1ENR & ~RCC_AHB1ENR_GPIODEN_Msk) | (0x01 << RCC_AHB1ENR_GPIODEN_Pos);
	
	// Set mode register of GPIO port D, pin 12 (green LED) to 01, indicates general purpose output mode
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER12_Msk) | (0x01 << GPIO_MODER_MODER12_Pos);
	
	// Set mode register of GPIO port D, pin 13 (orange LED) to 01, indicates general purpose output mode
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER13_Msk) | (0x01 << GPIO_MODER_MODER13_Pos);
	
	// Set mode register of GPIO port D, pin 14 (red LED) to 01, indicates general purpose output mode
	GPIOD->MODER = (GPIOD->MODER & ~ GPIO_MODER_MODER14_Msk) | (0x01 << GPIO_MODER_MODER14_Pos);
	
	/* Set moder register of GPIO port D, pin 15 (blue LED) to 01, indicates general purpose output mode
	   (GPIOD->MODER & ~GPIO_MODER_MODER15_Msk) -sets bits 30 and 31 to 0, doesn't change other bits
	   (0x01 << GPIO_MODER_MODE15_Pos) - sets bit 31 to 0 and bit 30 to 1, read as 01
	*/
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER15_Msk) | (0x01 << GPIO_MODER_MODE15_Pos);
}

/* Turn on the LED at the given index */
void LEDOn(uint32_t index) {
    if (index == 0) {
        GPIOD->BSRR = 0x1000;       // Set GPIO D bit 12 (corresponding to green LED) to 1
    } else if (index == 1) {
        GPIOD->BSRR = 0x2000;       // Set GPIO D bit 13 (corresponding to orange LED) to 1
    } else if (index == 2) {
        GPIOD->BSRR = 0x4000;       // Set GPIO D bit 14 (corresponding to red LED) to 1
    } else if (index == 3) {
        GPIOD->BSRR = 0x8000;       // Set GPIO D bit 15 (corresponding to blue LED) to 1
    } else {
        return;                     // If index does not correspond to LED, no bit is set 
    }
}

/* Turn on the LED at the given index */
void LEDOff(uint32_t index) {
    if (index == 0) {
        GPIOD->BSRR = 0x10000000;   // Reset GPIO D bit 12 (corresponding to green LED) to 0
    } else if (index == 1) {
        GPIOD->BSRR = 0x20000000;   // Reset GPIO D bit 12 (corresponding to green LED) to 0
    } else if (index == 2) {
        GPIOD->BSRR = 0x40000000;   // Reset GPIO D bit 12 (corresponding to green LED) to 0
    } else if (index == 3) {
        GPIOD->BSRR = 0x80000000;   // Reset GPIO D bit 12 (corresponding to green LED) to 0
    } else {
        return;                     // If index does not correspond to LED, no bit is reset 
    }
}

void LEDOnColour(enum LEDColour colour) {
    if (colour == GREEN_LED) {
        GPIOD->BSRR = 0x1000;               // Set GPIO D bit 12 (corresponding to green LED) to 1
    } else if (colour == ORANGE_LED) {
        GPIOD->BSRR = 0x2000;               // Set GPIO D bit 13 (corresponding to orange LED) to 1
    } else if (colour == RED_LED) {
        GPIOD->BSRR = 0x4000;               // Set GPIO D bit 14 (corresponding to red LED) to 1
    } else if (colour == BLUE_LED) {
        GPIOD->BSRR = 0x8000;               // Set GPIO D bit 15 (corresponding to blue LED) to 1
    } else if (colour == ALL_LED) {
        GPIOD->BSRR = 0xF000;               // Set GPIO D bits 12-15 (corresponding to all LEDs) to 1
    } else {
        return;                             // If colour does not correspond to LED, no bit is reset 
    }
}

void LEDOffColour(enum LEDColour colour) {
    if (colour == GREEN_LED) {
        GPIOD->BSRR = 0x10000000;           // Set GPIO D bit 12 (corresponding to green LED) to 1
    } else if (colour == ORANGE_LED) {
        GPIOD->BSRR = 0x20000000;           // Set GPIO D bit 13 (corresponding to orange LED) to 1
    } else if (colour == RED_LED) {
        GPIOD->BSRR = 0x40000000;           // Set GPIO D bit 14 (corresponding to red LED) to 1
    } else if (colour == BLUE_LED) {
        GPIOD->BSRR = 0x80000000;           // Set GPIO D bit 15 (corresponding to blue LED) to 1
    } else if (colour == ALL_LED) {
        GPIOD->BSRR = 0xF0000000;           // Set GPIO D bits 12-15 (corresponding to all LEDs) to 1
    } else {
        return;                             // If colour does not correspond to LED, no bit is reset 
    }
}