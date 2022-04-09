#include "user_button.h"

void initUserButton(void) {
    // enable clock to GPIOA
    RCC->AHB1ENR = ((RCC->AHB1ENR & ~RCC_AHB1ENR_GPIOAEN_Msk) | (0x01 << RCC_AHB1ENR_GPIOAEN_Pos));

    GPIOA->MODER = ((GPIOA->MODER & ~GPIO_MODER_MODER0_Msk) | (0x00 << GPIO_MODER_MODER0_Pos));

    GPIOA->PUPDR = ((GPIOA->PUPDR & ~GPIO_PUPDR_PUPD0_Msk) | (0x10 << GPIO_PUPDR_PUPD0_Pos));
}

uint32_t isUserButtonPressed() {
    return !(GPIOA->ODR & 0x01);
}