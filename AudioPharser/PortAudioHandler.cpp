#include "PortAudioHandler.h"
#define BitsPerSample 512

int PortaudioThread::audio_callback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags,
                    void *userData )
					{
						float *out;
						sf_count_t num_read;
						callback_data_s *p_data;

						out = (float*)outputBuffer;
						p_data = (callback_data_s*)userData;

						/* clear output buffer */
                        memset(out, 0, sizeof(float) * framesPerBuffer * p_data->Fileinfo.channels);

						/* read directly into output buffer */
						num_read = sf_read_float(p_data->file, out, framesPerBuffer * p_data->Fileinfo.channels);

						
						p_data->currentframe = sf_seek(p_data->file, 0, SEEK_CUR);
						

					
						if (num_read < framesPerBuffer || p_data->currentframe > p_data->Fileinfo.frames)
						{
								return paComplete;
						}
						return paContinue;
					}


static int checkFileOnErrors(SNDFILE *file){
	return sf_error(file);
}

static void CheckPaError(PaError err) {
	if(err != paNoError){
		printf("Portaudio Error!: %s \n", Pa_GetErrorText(err));
		exit(1);
	}
}


int PortaudioThread::Portaudiohandler(int calltype) {
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

PortaudioThread::PortaudioThread(QObject *parent)
	:QThread(parent), filename(filename), IsRunning(1){}

float PortaudioThread::CalculatePercentage(int currentFrame, int MaxFrames){
    float framesinpercent;
	printf("%lld\n", currentFrame); 
	printf("%lld\n", MaxFrames); 
	printf("%f\n", (currentFrame * 1.0f) / MaxFrames  * 100.0f);
	return framesinpercent;
}

void PortaudioThread::PaInit() {
	PaError err;
	Pa_info pa_handler_info;


	err = Pa_Initialize();
	CheckPaError(err);	

}

void PortaudioThread::StartPlayback(){
    SNDFILE *file;
	PaStream *stream;
	PaError err;
	callback_data_s filedata;
    Pa_info pa_handler_info;

	stream = pa_handler_info.Stream;
	IsRunning = 1;

    filedata.file = sf_open(filename, SFM_READ, &filedata.Fileinfo);
	checkFileOnErrors(filedata.file);
    printf("hi");


	const int OutputDevice = Portaudiohandler(2);
    printf("%d\n", OutputDevice);
	printf("%s\n", filename);
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
	while (Pa_IsStreamActive (stream) == 1)
	{
		float floatinpercent = CalculatePercentage(filedata.currentframe, filedata.Fileinfo.frames);
		QThread::msleep(100);
	}
	

}

PortaudioThread::~PortaudioThread() {
	stop();
}

void PortaudioThread::stop() {
	Pa_info pa_handler_info;
	IsRunning = 0;
    wait(); 

    if (pa_handler_info.Stream) {
        Pa_StopStream(pa_handler_info.Stream);
        Pa_CloseStream(pa_handler_info.Stream);
        Pa_Terminate();
    }
}
void PortaudioThread::setFile(char *filenameset) {
    filename = filenameset;
}
void PortaudioThread::run() {
	StartPlayback();
}


bool PortaudioThread::returnIsRunning(){
	return IsRunning;
}