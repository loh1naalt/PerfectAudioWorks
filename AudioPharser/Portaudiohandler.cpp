#include <stdlib.h>
#include <stdio.h>

#include <portaudio.h>

static void checkerror(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}
int main() {
	PaError err;
	err = Pa_Initialize();
	checkerror(err);

	int quantityDevices = Pa_GetDeviceCount();
	printf("Devices detected: %d\n", quantityDevices);

	err = Pa_Terminate();
	checkerror(err);

	return 0;
}
