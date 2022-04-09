#include "LEDs.h"
#include <stm32f407xx.h>

/* Enable clock to user LEDs and setup GPIO registers */
void initLEDs(void) {
    // Enable AHB1 Clock to GPIO Port D (all LED pins on this port)
    RCC->AHB1ENR = ((RCC->AHB1ENR & ~RCC_AHB1ENR_GPIODEN_Msk) | (0x01 << RCC_AHB1ENR_GPIODEN_Pos)); 

    /* Set GPIO Port D Mode Register to 0b01 for LED pins - indicates general purpose output mode */
    GPIOD->MODER = ((GPIOD->MODER & ~GPIO_MODER_MODE12_Msk) | (0x01 << GPIO_MODER_MODE12_Pos)); // Pin 12 - Green LED
    GPIOD->MODER = ((GPIOD->MODER & ~GPIO_MODER_MODE13_Msk) | (0x01 << GPIO_MODER_MODE13_Pos)); // Pin 13 - Orange LED
    GPIOD->MODER = ((GPIOD->MODER & ~GPIO_MODER_MODE14_Msk) | (0x01 << GPIO_MODER_MODE14_Pos)); // Pin 14 - Red LED
    GPIOD->MODER = ((GPIOD->MODER & ~GPIO_MODER_MODE15_Msk) | (0x01 << GPIO_MODER_MODE15_Pos)); // Pin 15 - Blue LED

    /* Set GPIO Port D Output Type Register to 0b0 for LED pins - indicates output push-pull type */
    GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT12_Msk); // Pin 12 - Green LED
    GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT13_Msk); // Pin 13 - Orange LED
    GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT14_Msk); // Pin 14 - Red LED
    GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT15_Msk); // Pin 15 - Blue LED

    /* Set GPIO Port D Pull-up/pull-down Register to 0b00 for LED pins - indicates output push-pull type */
    GPIOD->PUPDR = (GPIOD->PUPDR & ~GPIO_PUPDR_PUPD12_Msk); // Pin 12 - Green LED
    GPIOD->PUPDR = (GPIOD->PUPDR & ~GPIO_PUPDR_PUPD13_Msk); // Pin 13 - Orange LED
    GPIOD->PUPDR = (GPIOD->PUPDR & ~GPIO_PUPDR_PUPD14_Msk); // Pin 14 - Red LED
    GPIOD->PUPDR = (GPIOD->PUPDR & ~GPIO_PUPDR_PUPD15_Msk); // Pin 15 - Blue LED
}

/* Turn on the LED at the specified index */
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

/* Turn off the LED at the specified index */
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

/* Turn on the LED using the specified colour */
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

/* Turn off the LED using the specified colour */
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