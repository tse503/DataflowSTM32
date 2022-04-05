#include <stm32f407xx.h>

enum LEDColour {GREEN_LED, ORANGE_LED, RED_LED, BLUE_LED, ALL_LED};

void initialiseLEDPorts();

void LEDOn(uint32_t index);
void LEDOff(uint32_t index);

void LEDOnColour(enum LEDColour colour);
void LEDOffColour(enum LEDColour colour);
