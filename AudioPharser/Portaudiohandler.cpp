#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#include <portaudio.h>
#include "Portaudiohandler.h"
#include "wavpharser.h"

static void checkerror(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}

int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                    void *userData ){
						return 0;
					}

int main(int argc, char* argv[]) {
	PaError err;
	err = Pa_Initialize();
	checkerror(err);

	int OutputId, InputId;

	InputId = Portaudiohandler(1);
	OutputId = Portaudiohandler(2);
	
	const int InputDevice = InputId;
	const int OutputDevice = OutputId;
	const int SampleRate = getSampleRate(argv[1]);
	const int BitsPerSample = getBitsPerSample(argv[1]);

	PaStreamParameters inputParameters;
	memset(&inputParameters, 0, sizeof(inputParameters));
	inputParameters.channelCount = 2;
	inputParameters.device = InputDevice;
	inputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(InputDevice)->defaultLowOutputLatency;

	PaStreamParameters outputParameters;
	memset(&outputParameters, 0, sizeof(outputParameters));
	outputParameters.channelCount = 2;
	outputParameters.device = OutputDevice;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultLowOutputLatency;

	PaStream* stream;
	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		&outputParameters,
		SampleRate,
		BitsPerSample,
		paNoFlag,
		audio_callback,
		NULL
	);
	checkerror(err);

	printf("%d\n",SampleRate);
	printf("%d\n",BitsPerSample);

	err = Pa_CloseStream(stream);
	checkerror(err);

	err = Pa_Terminate();
	checkerror(err);
	return 0;
}
