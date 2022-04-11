#include <stm32f407xx.h>

#define LED_GREEN   0UL
#define LED_ORANGE  1UL
#define LED_RED     2UL
#define LED_BLUE    3UL
#define LED_ALL     4UL

void initUserLEDs();

void LEDOn(uint32_t index);
void LEDOff(uint32_t index);

