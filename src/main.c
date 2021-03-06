#include "Audio_Drivers.h"
#include "user_LEDs.h"
#include "user_button.h"

#include "BFLO_internal.h"
#include "control.h"
#include "oscillatorLUT.h"

#include <stm32f407xx.h>
#include <stdint.h>
#include <math.h>

#define PBSIZE 4096
#define SINESIZE 1024
#define PI 3.141592653589793 

int16_t PlayBuff[PBSIZE];
int16_t SineBuff[SINESIZE];

enum eNoteStatus {ready, going, finish} noteStatus = ready;
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
	initUserButton();
	// Speed up the clock, then configure the audio timer (required for checking timeouts):
	myAudioSpeedUpTheSystemClock();
	initAudioTimer();
	
	// Initialise the audio driver:
	myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);
    
	// Initialise the audio buffer with silence:
	for(int i=0; i <= PBSIZE; i++) {
		PlayBuff[i] = 0;
	}
	
	// Set up the sine look-up table:
	for (int i = 0; i < SINESIZE; i++) {
		float q = 32760 * sin(i * 2.0 * PI / SINESIZE);
		SineBuff[i] = (int16_t)q;
	}
}

int main(void) {
    setupAudioAndPeripherals();

	// Set-up the phase and phase increment:
	float currentPhase = 0.0;
	float desiredFreq = 880.0f;
	float phaseIncrement = (desiredFreq * SINESIZE) / AUDIO_FREQUENCY_44K; 
	
    graph_t synthGraph;
    module_t freqControl, osc;

    BFLO_initGraph(&synthGraph);
    BFLO_initControlModule(&freqControl, "Frequency Control", CMajScale[0]);
    BFLO_initOcillatorLUTModule(&osc, "Sine Oscillator", 440.0f);

    BFLO_insertModule(&synthGraph, &freqControl);
    BFLO_insertModule(&synthGraph, &osc);

    BFLO_connectModules(&freqControl, 0, &osc, 0);

	// Start the audio driver play routine:
	myAudioStartPlaying(PlayBuff, PBSIZE);

	while(1) {
		if (isUserButtonPressed()) {
			// Check if button has been just been pressed (button status previosuly = 0)
			if (userButtonStatus == 0) {
                // On button press, increment the note in the scale
				BFLO_setOutputControl(&freqControl, 0, CMajScale[scaleIndex]);
				scaleIndex += scaleDirection;
                // When reached either end of scale, change direction
				if (scaleIndex == 7) {
					scaleDirection = -1;
				} else if (scaleIndex == 0) {
					scaleDirection = 1;
				}
				LEDOn(LED_GREEN);
				userButtonStatus = 1;
			}			
		} else {
			LEDOff(LED_GREEN);
			userButtonStatus = 0;
		}

		phaseIncrement = (desiredFreq * SINESIZE) / AUDIO_FREQUENCY_44K; 
		// printf("Adding a tad of delay in the loop...\n");	// QUESTION: Why does adding some delay make the sound work
		// LEDOffColour(ORANGE_LED);	// QUESTION: Why does sound only play when an LED set on/off at start of while loop?

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
            BFLO_processOscillatorLUTModule(&osc);
            uint32_t index = 0;
			for (int i = startFill; i < endFill; i += 2) {
				currentPhase += phaseIncrement;
				
				if (currentPhase > SINESIZE) {
					currentPhase -= SINESIZE;
				}

                int16_t modSample = (int16_t)(((float *)(osc.outputs[0].data))[index]);      

				PlayBuff[i] = modSample;
				PlayBuff[i + 1] = modSample;

                index++;
			}
		}
	} // End of while loop	
}
