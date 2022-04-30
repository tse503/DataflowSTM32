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

/* Static function for generating a triangle lookup table */
static void generateTriangle(float * table) {
    // Indices of quarter- and half-way values of the lookup table 
    uint32_t quarter = (MAX_LUT_SIZE * 0.25);
    uint32_t half = (MAX_LUT_SIZE * 0.5);

    // Ramp up from 0 to maximum amplitude
    for (uint32_t i = 0; i < quarter; i++) {
        table[i] = 32760 * i / quarter; 
    }

    // Ramp down from maximum amplitude to minimum amplitude
    for (uint32_t i = 0; i < half; i++) {
        table[i + quarter] = 32760 - (2 * 32760 * i / half);
    }

    // Ramp up from minimum amplitude to 0
    for (uint32_t i = 0; i < quarter; i++) {
        table[i + (3 * quarter)] = -32760 + (32760 * i / quarter); 
    }

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
    // Generate waveform based on specified waveform
    if (waveform == SINE) {
        generateSine(table);
    } else if (waveform == SAWTOOTH) {
        generateSawtooth(table);
    } else if (waveform == SQUARE) {
        generateSquare(table);
    }
}
