#include "Audio_Drivers.h"
#include "user_LEDs.h"
#include "user_button.h"

#include "BFLO_internal.h"
#include "BFLO_tableGenerator.h"
#include "control.h"
#include "controlMultiplier.h"
#include "bufferScaler.h"
#include "bufferAdder.h"
#include "oscillatorLUT.h"

#include <stm32f407xx.h>
#include <stdint.h>
#include <math.h>

#define PBSIZE 4096
#define LUTSIZE 1024

#define FIFTH_INTERVAL 1.49830674321f

int16_t PlayBuff[PBSIZE];

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
}

int main(void) {
    setupAudioAndPeripherals();

    graph_t synthGraph;
    module_t freqControl, freqInterval, freqMultiplier, volControl, volScaler0, volScaler1, fundamentalOsc, fifthOsc;

    BFLO_initGraph(&synthGraph);

    BFLO_initControlModule(&freqControl, &synthGraph, "Frequency Control", CMajScale[0]);
	BFLO_initControlModule(&freqInterval, &synthGraph, "Frequency Interval", FIFTH_INTERVAL);		// Root frequency multiplied by this number gives its 5th interval
	BFLO_initControlModule(&volControl, &synthGraph, "Volume Control", 0.2);
	BFLO_initBufferScalerModule(&volScaler0, &synthGraph, "Volume Scaler 0");						// Volume scaler for fundamental oscillator
	BFLO_initBufferScalerModule(&volScaler1, &synthGraph, "Volume Scaler 1"); 						// Volume scaler for 5th oscillator
    BFLO_initControlMultiplierModule(&freqMultiplier, &synthGraph, "Frequency Multiplier");			// Multiplies the frequency control by the frequency interval
	BFLO_initOcillatorLUTModule(&fundamentalOsc, &synthGraph, "Fundamental Oscillator", 440.0f);
	BFLO_initOcillatorLUTModule(&fifthOsc, &synthGraph, "Fifth Oscillator", 440.0f * FIFTH_INTERVAL);

    BFLO_connectModules(&freqControl, 0, &fundamentalOsc, 0);
	BFLO_connectModules(&freqControl, 0, &freqMultiplier, 0);
	BFLO_connectModules(&volControl, 0, &volScaler0, 1);
	BFLO_connectModules(&volControl, 0, &volScaler1, 1);

	BFLO_connectModules(&freqInterval, 0, &freqMultiplier, 1);
	BFLO_connectModules(&freqMultiplier, 0, &fifthOsc, 0);

	BFLO_connectModules(&fundamentalOsc, 0, &volScaler0, 0);

	// BFLO_orderGraphDFS(&synthGraph);

	// Start the audio driver play routine:
	myAudioStartPlaying(PlayBuff, PBSIZE);

	while(1) {
		if (isUserButtonPressed()) {
			// Check if button has been just been pressed (button status previously = 0)
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
			BFLO_processGraph(&synthGraph);
			// BFLO_processControlMultiplierModule(&freqMultiplier);
            // BFLO_processOscillatorLUTModule(&fundamentalOsc);
			// BFLO_processOscillatorLUTModule(&fifthOsc);

            uint32_t index = 0;
			for (int i = startFill; i < endFill; i += 2) {

                int16_t fundamentalSample = (int16_t)(((float *)(volScaler0.outputs[0].data))[index]);
				int16_t fifthSample = (int16_t)(((float *)(fundamentalOsc.outputs[0].data))[index]);        

				// Play fundamental on the left channel, fifth on the right 
				PlayBuff[i] = fundamentalSample;
				PlayBuff[i + 1] = fifthSample;

                index++;
			}
		}
	} // End of while loop	
}
