#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#include <sndfile.h>
#include <portaudio.h>
#include "wavpharser.h"

#define BitsPerSample 512

int Portaudiohandler(int calltype) {
	int quantityDevices = Pa_GetDeviceCount();
	printf("Devices detected: %d\n", quantityDevices);
	if (quantityDevices < 0){
		printf("Error getting devices amount\n");
		exit(1);
	}
	else if (quantityDevices == 0) {
		printf("no devices detected on this machine\n");
		exit(0);
	}

	/* 
		0 - Get All devices
		1 - Get Default Input device
		2 - Get Default Output device
	*/

	switch (calltype){
		case 0:
		{
			const PaDeviceInfo* deviceInfo;

			for(int i = 0; i < quantityDevices; i++){
				deviceInfo = Pa_GetDeviceInfo(i);
				printf("Device %d:\n", i);
				printf("	name: %s\n", deviceInfo->name);
				printf("	maxInputChannels: %d\n", deviceInfo->maxInputChannels);
				printf("	maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
				printf("	defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
			}
			/*
				for now we are returning 0 
				(otherwise we'll get core dumped error)
				but for later i think to return array of devices id's
			*/ 
			return 0;
			break;
		}
		case 1:
		{
			PaDeviceIndex inputDevice;
			inputDevice = Pa_GetDefaultInputDevice();

			const PaDeviceInfo *infoI = Pa_GetDeviceInfo(inputDevice);

			printf("Default Input Device: %s\n", infoI->name);
			return inputDevice;
			break;
		}
		case 2:
		{
			PaDeviceIndex outputDevice;

			outputDevice = Pa_GetDefaultOutputDevice();

			const PaDeviceInfo *infoO = Pa_GetDeviceInfo(outputDevice);
			printf("Default output Device: %s\n", infoO->name);
			return outputDevice;
			break;
		}
		default:
			return 0;
			break;
	}
}
/* function for cheking if Portaudio works correctly*/
static void CheckPaError(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}

/* function for checking if file is not corrupter or something*/
static int checkFileOnErrors(SNDFILE *file){
	return sf_error(file);
}

int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                    void *userData )
					{
						return 0;
					}

int PaHandler(char* filename) {
	PaError err;
	err = Pa_Initialize();
	CheckPaError(err);

	SNDFILE *file;
	SF_INFO Fileinfo;
	int samplerate;

	file = sf_open(filename, SFM_READ, &Fileinfo);
	checkFileOnErrors(file);

	int OutputId, InputId;

	InputId = Portaudiohandler(1);
	OutputId = Portaudiohandler(2);
	
	const int InputDevice = InputId;
	const int OutputDevice = OutputId;
	const int SampleRate = Fileinfo.samplerate;

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
		0,
		&outputParameters,
		SampleRate,
		BitsPerSample,
		paNoFlag,
		audio_callback,
		NULL
	);
	
	CheckPaError(err);

	printf("%d\n",SampleRate);
	printf("%d\n",BitsPerSample);

	err = Pa_CloseStream(stream);
	CheckPaError(err);

	err = Pa_Terminate();
	CheckPaError(err);
	return 0;
}