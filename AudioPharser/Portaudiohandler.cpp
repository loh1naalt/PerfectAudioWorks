#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

#include <sndfile.h>
#include <portaudio.h>
#include "Portaudiohandler.h"
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
typedef struct
{
    SNDFILE     *file;
    SF_INFO      Fileinfo;
	float	 	 frametime;
	sf_count_t	 currentframe;
	int			 rewindtoframe;
	
} callback_data_s;

/* function for cheking if Portaudio works correctly*/
static void CheckPaError(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}


/* function for checking if file is not corrupted or something*/
static int checkFileOnErrors(SNDFILE *file){
	return sf_error(file);
}

int audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                    void *userData )
					{
						float *out;
						callback_data_s *p_data = (callback_data_s*)userData;
						sf_count_t num_read;

						out = (float*)outputBuffer;
						p_data = (callback_data_s*)userData;

						/* clear output buffer */
						memset(out, 0, sizeof(float) * framesPerBuffer * p_data->Fileinfo.channels);

						/* read directly into output buffer */
						num_read = sf_read_float(p_data->file, out, framesPerBuffer * p_data->Fileinfo.channels);

						
						p_data->currentframe = sf_seek(p_data->file, 0, SEEK_CUR);
						//printf("%lld\n", p_data->currentframe); 
					
						if (num_read < framesPerBuffer || p_data->currentframe > p_data->Fileinfo.frames)
						{
								return paComplete;
						}
						return paContinue;
					}

int PaHandler(char* filename) {
	SNDFILE *file;
	PaStream *stream;
	PaError err;
	callback_data_s filedata;


	err = Pa_Initialize();
	CheckPaError(err);


	

	filedata.file = sf_open(filename, SFM_READ, &filedata.Fileinfo);
	checkFileOnErrors(filedata.file);

	int OutputId, InputId;

	
	OutputId = Portaudiohandler(2);
	
	const int OutputDevice = OutputId;

	PaStreamParameters outputParameters;
	memset(&outputParameters, 0, sizeof(outputParameters));
	outputParameters.channelCount = filedata.Fileinfo.channels;
	outputParameters.device = OutputDevice;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(OutputDevice)->defaultLowOutputLatency;


	err = Pa_OpenStream(
		&stream,
		0,
		&outputParameters,
		filedata.Fileinfo.samplerate,
		BitsPerSample,
		paNoFlag,
		audio_callback,
		&filedata
	);
	CheckPaError(err);

	read_wav_file(filename);
	Pa_StartStream(stream);
	while (Pa_IsStreamActive(stream)){
		
		Pa_Sleep(100);
	}
	

	CheckPaError(err);

	err = Pa_CloseStream(stream);
	CheckPaError(err);

	err = Pa_Terminate();
	CheckPaError(err);
	return 0;
}