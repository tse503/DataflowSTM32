#include "Audio_Drivers.h"
#include "user_LEDs.h"
#include "user_button.h"

#include "BFLO_internal.h"
#include "BFLO_tableGenerator.h"
#include "control.h"
#include "hardware/buttonDiscovery.h"
#include "hardware/keyboard/buttonC.h"
#include "synthesis/oscillatorLUT.h"
#include "synthesis/lookupTable.h"
#include "envelopeAR.h"
#include "sampling/samplePlayer.h"

#include "samples.h"

#include <stm32f407xx.h>
#include <stdint.h>
#include <math.h>

#define PBSIZE 4096

#define FIFTH_INTERVAL 1.49830674321f

table_t SineTable;
table_t SquareTable;
int16_t PlayBuff[PBSIZE];

enum eBufferStatus {empty, finished, firstHalfReq, firstHalfDone, secondHalfReq, secondHalfDone} bufferStatus = empty;

uint32_t userButtonStatus = 0;
int32_t scaleIndex = 0;
int32_t scaleDirection = 1; // 1 = go up scale, -1 = go down scale

float const CMajScale[8] = {261.00f, 293.66f, 329.63f, 349.23f, 392.00f, 440.00f, 493.88f, 523.25f};

// Audio callbacks	
void myAudioHalfTransferCallback(void) {
	bufferStatus = firstHalfReq;
}

void myAudioTransferCompleteCallback(void) {
	myAudioChangeBuffer(PlayBuff, PBSIZE);
	bufferStatus = secondHalfReq;
}

void setupAudioAndPeripherals(void) {
    initUserLEDs();
	// initUserButton();	

	// Speed up the clock, then configure the audio timer (required for checking timeouts):
	myAudioSpeedUpTheSystemClock();
	initAudioTimer();
	
	// Initialise the audio driver:
	myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);
    
	// Initialise the audio buffer with silence:
	for(int i=0; i < PBSIZE; i++) {
		PlayBuff[i] = 0;
	}
}

void setupLUTs(void) {
	// Fill the lookup table with samples
	BFLO_generateLUT(SineTable.samples, SINE);
	BFLO_generateLUT(SquareTable.samples, SQUARE);

	// Set lookup table sizes
	SineTable.size = 1024;
	SquareTable.size = 1024;

	// Give the tables a name
	strncpy(SineTable.name, "Sine Lookup", MAX_NAME_LENGTH);
	strncpy(SquareTable.name, "Square Lookup", MAX_NAME_LENGTH);
}

// int main(void) {
//     setupAudioAndPeripherals();
// 	setupLUTs();

//     graph_t synthGraph;
//     module_t buttonDiscovery, buttonC;

//     BFLO_initGraph(&synthGraph);

// 	BFLO_initButtonDiscoveryModule(&buttonDiscovery, &synthGraph, "Button Discovery");
// 	BFLO_initButtonCModule(&buttonC, &synthGraph, "Button C");

// 	while(1) {
// 		BFLO_processGraph(&synthGraph);

// 		if (BFLO_getOutputControl(&buttonC, 0) == 1) {
// 			LEDOn(LED_ORANGE);
// 		} else {
// 			LEDOff(LED_ORANGE);
// 		}

// 		if (BFLO_getOutputControl(&buttonDiscovery, 0) == 1) {
// 			LEDOn(LED_GREEN);
// 		} else {
// 			LEDOff(LED_GREEN);
// 		}
// 	}
// }

int main(void) {
    setupAudioAndPeripherals();
	setupLUTs();

    graph_t synthGraph;
    module_t sampler, freqControl, envReset, table, osc, env, buttonDiscovery, buttonC;	

    BFLO_initGraph(&synthGraph);

	// BFLO_initSamplePlayerModule(&sampler, &synthGraph, "Sampler", KickSamples);
    BFLO_initControlModule(&freqControl, &synthGraph, "Frequency Control", 440.0f);
	BFLO_initLookupTableModule(&table, &synthGraph, "Table", &SineTable);
	BFLO_initOcillatorLUTModule(&osc, &synthGraph, "Oscillator", 440.0f);
	BFLO_initEnvelopeARModule(&env, &synthGraph, "Envelope", 22050.0f, 2000.0f);
	BFLO_initButtonDiscoveryModule(&buttonDiscovery, &synthGraph, "Discovery User Button");

	BFLO_initButtonCModule(&buttonC, &synthGraph, "Button C");

	BFLO_connectModules(&buttonC, 0, &env, 1);
	BFLO_connectModules(&freqControl, 0, &osc, 0);
	BFLO_connectModules(&table, 0, &osc, 1);
	BFLO_connectModules(&osc, 0, &env, 0);

	// BFLO_orderGraphDFS(&synthGraph);

	// Start the audio driver play routine:
	myAudioStartPlaying(PlayBuff, PBSIZE);


	while(1) {
		// if (isUserButtonPressed()) {
		// 	// Check if button has been just been pressed (button status previously = 0)
		// 	if (userButtonStatus == 0) {
		// 		//On button press, reset AR envelope
		// 		BFLO_setOutputControl(&envReset, 0, 1.0f);
        //         // On button press, increment the note in the scale
		// 		BFLO_setOutputControl(&freqControl, 0, CMajScale[scaleIndex]);
		// 		scaleIndex += scaleDirection;
        //         // When reached either end of scale, change direction
		// 		if (scaleIndex == 7) {
		// 			scaleDirection = -1;
		// 		} else if (scaleIndex == 0) {
		// 			scaleDirection = 1;
		// 		}
		// 		LEDOn(LED_GREEN);
		// 		userButtonStatus = 1;
		// 	}			
		// } else {
		// 	LEDOff(LED_GREEN);
		// 	BFLO_setOutputControl(&envReset, 0, 0.0f);
		// 	userButtonStatus = 0;
		// }

		// printf("Adding a tad of delay in the loop...\n");	// QUESTION: Why does adding some delay make the sound work
		// LEDOff(LED_ALL);	// QUESTION: Why does sound only play when an LED set on/off at start of while loop?

		// If there's been a request to fill half of the buffer, then set the start and end points to fill
		uint32_t startFill = 0, endFill = 0;
		
		if (bufferStatus == firstHalfReq) {
			startFill = 0;
			endFill = PBSIZE / 2;
			bufferStatus = firstHalfDone;	
		} 

		else if (bufferStatus == secondHalfReq) {
			startFill = PBSIZE / 2;
			endFill = PBSIZE;
			bufferStatus = secondHalfDone;			
		}
		
		if (startFill != endFill) {
			BFLO_processGraph(&synthGraph);

			if(GPIOE->IDR & GPIO_IDR_ID7_Msk) {
				LEDOn(LED_RED);
			} else {
				LEDOff(LED_RED);
			}

            uint32_t index = 0;
			for (int i = startFill; i < endFill; i += 2) {

                int16_t sample = (int16_t)(((float *)(env.outputs[0].data))[index]);     

				// Play fundamental on the left channel, fifth on the right 
				PlayBuff[i] = sample;
				PlayBuff[i + 1] = sample;

                index++;
			}
		}

		LEDOn(LED_GREEN);
	} // End of while loop	
}
