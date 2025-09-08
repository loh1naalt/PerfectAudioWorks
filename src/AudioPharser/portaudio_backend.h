#ifdef __cplusplus
extern "C" {
#endif
#pragma once
#include "CodecHandler.h"
#include <portaudio.h>

typedef struct {
    CodecHandler* codec;
    PaStream* stream;
    int DeviceIndex;
    char DeviceName[50];
    int channels;
    int samplerate;
    long totalFrames;
    long currentFrame;
    int paused;
} AudioPlayer;

// Initialize/terminate PortAudio
int audio_init();
int audio_terminate();

// Playback control
int audio_play(AudioPlayer* player, const char* filename, int device);
int audio_stop(AudioPlayer* player);
int audio_pause(AudioPlayer* player, int pause);
int audio_seek(AudioPlayer* player, long frame);

// Callback
int audio_callback_c(const void* input, void* output, unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void* userData);
#ifdef __cplusplus
}
#endif