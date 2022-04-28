#include "BFLO_tableGenerator.h"

/* Static function for generating a sine lookup table */
static void generateSine(float * table) {
    for (uint32_t i = 0; i < MAX_LUT_SIZE; i++) {
        // Calculate sine value 
		float sample = 32760 * sin(i * 2.0 * PI / MAX_LUT_SIZE);
        // Place generated sample in the LUT
		table[i] = sample;
	}
}

/* Static function for generating a sawtooth lookup table */
static void generateSawtooth(float * table) {
    for (uint32_t i = 0; i < MAX_LUT_SIZE; i++) {
        // Ramp up linearly from minimum to maximum amplitude on each iteration
		float sample = -32760 + (i * 65520 / (MAX_LUT_SIZE - 1));
        // Place generated sample in the LUT
		table[i] = sample;
	}
}

/* Static function for generating a square lookup table */
static void generateSquare(float * table) {
    // Samples are at maximum amplitude for the first half of the LUT 
    for (uint32_t i = 0; i < (MAX_LUT_SIZE / 2); i++) {
		table[i] = 32760;
	}
    // Samples are at minimum amplitude for the second half of the LUT 
    for (uint32_t i = (MAX_LUT_SIZE / 2); i < MAX_LUT_SIZE; i++) {
		table[i] = -32760;
	}
}

/* Fill the table with generated samples based on the specified waveform */
void BFLO_generateLUT(float * table, enum BFLO_WAVEFORM waveform) {
    if (waveform == SINE) {
        generateSine(table);
    } else if (waveform == SAWTOOTH) {
        generateSawtooth(table);
    } else if (waveform == SQUARE) {
        generateSquare(table);
    }
}
