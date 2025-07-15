#include <stdlib.h>
#include <stdio.h>

#include <portaudio.h>

static void checkerror(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}
void getDefaultIODevice(){
	PaDeviceIndex inputDevice, outputDevice;
	inputDevice = Pa_GetDefaultInputDevice();
  	outputDevice = Pa_GetDefaultOutputDevice();

    const PaDeviceInfo *infoI = Pa_GetDeviceInfo(inputDevice);
	const PaDeviceInfo *infoO = Pa_GetDeviceInfo(outputDevice);

    printf("Default Input Device: %s\n", infoI->name);
    printf("Default Output Device: %s\n", infoO->name);
  

}

void getAllDevices(int quantityDeivces){
	const PaDeviceInfo* deviceInfo;
	for(int i = 0; i < quantityDeivces; i++){
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("Device %d:\n", i);
		printf("	name: %s\n", deviceInfo->name);
		printf("	maxInputChannels: %d\n", deviceInfo->maxInputChannels);
		printf("	maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
		printf("	defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
	}
}

void Portaudiohandler(int calltype) {
	PaError err;
	err = Pa_Initialize();
	checkerror(err);

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
		1 - Get Default device
	*/

	if (calltype == 0){
		getAllDevices(quantityDevices);
	}
	else if (calltype == 1){
		getDefaultIODevice();
	}
	else{
		return;
	}

	err = Pa_Terminate();
	checkerror(err);

}
