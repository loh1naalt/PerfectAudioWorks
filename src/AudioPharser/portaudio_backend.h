#ifdef __cplusplus
extern "C" {
#endif
#pragma once
#include "CodecHandler.h"
#include <portaudio.h>
#include <stdint.h>
#include <pthread.h>

typedef struct {
    CodecHandler* codec;
    PaStream* stream;
    const char* CodecName;
    int channels;
    int samplerate;
    long totalFrames;
    long currentFrame;
    int paused;
    pthread_mutex_t lock; 
} AudioPlayer;

int audio_init();
int audio_terminate();

int audio_play(AudioPlayer* player, const char* filename, int device);
int audio_stop(AudioPlayer* player);
int audio_pause(AudioPlayer* player, int pause);
int audio_seek(AudioPlayer* player, int64_t frame);

int audio_callback_c(const void* input, void* output, unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void* userData);

#ifdef __cplusplus
}
#endif
