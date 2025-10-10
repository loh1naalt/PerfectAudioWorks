#include "portaudio_backend.h"
#include "CodecHandler.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct AudioPlayer {
    CodecHandler* codec;
    const char* CodecName;
    int channels;
    int samplerate;
    long totalFrames;
    long currentFrame;
    int paused;
    PaStream* stream;
    pthread_mutex_t lock;
};

int audio_callback_c(const void* input, void* output, unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void* userData)
{
    AudioPlayer* player = (AudioPlayer*)userData;
    float* out = (float*)output;
    if (!player || !player->codec) return paAbort;
    int channels = player->channels;
    if (player->paused) {
        memset(out, 0, frameCount * channels * sizeof(float));
        return paContinue;
    }
    long readFrames = 0;
    pthread_mutex_lock(&player->lock);
    readFrames = codec_read_float(player->codec, out, frameCount);
    pthread_mutex_unlock(&player->lock);
    player->currentFrame += readFrames;
    if (readFrames < (long)frameCount) {
        memset(out + readFrames * channels, 0,
               (frameCount - readFrames) * channels * sizeof(float));
        if (player->currentFrame >= player->totalFrames)
            return paComplete;
    }
    return paContinue;
}

int audio_init() {
    return Pa_Initialize();
}

int audio_terminate() {
    return Pa_Terminate();
}

int audio_play(AudioPlayer* player, const char* filename, int device) {
    if (!player || !filename) return -1;
    pthread_mutex_init(&player->lock, NULL);
    player->codec = codec_open(filename);
    if (!player->codec) return -1;
    player->CodecName = codec_return_codec(player->codec);
    player->channels = codec_get_channels(player->codec);
    player->samplerate = codec_get_samplerate(player->codec);
    player->totalFrames = codec_get_total_frames(player->codec);
    player->currentFrame = 0;
    player->paused = 0;
    if (device == -1) device = Pa_GetDefaultOutputDevice();
    const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(device);
    if (!devInfo) return -1;
    if (player->channels > devInfo->maxOutputChannels)
        player->channels = devInfo->maxOutputChannels;
    PaStreamParameters output;
    memset(&output, 0, sizeof(PaStreamParameters));
    output.device = device;
    output.channelCount = player->channels;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = devInfo->defaultLowOutputLatency;
    output.hostApiSpecificStreamInfo = NULL;
    PaError err = Pa_OpenStream(&player->stream, NULL, &output, player->samplerate,
                                512, paNoFlag, audio_callback_c, player);
    if (err != paNoError) return -1;
    err = Pa_StartStream(player->stream);
    if (err != paNoError) {
        Pa_CloseStream(player->stream);
        player->stream = NULL;
        return -1;
    }
    return 0;
}

int audio_stop(AudioPlayer* player) {
    if (!player) return -1;
    if (player->stream) {
        Pa_StopStream(player->stream);
        Pa_CloseStream(player->stream);
        player->stream = NULL;
    }
    if (player->codec) {
        pthread_mutex_destroy(&player->lock);
        codec_close(player->codec);
        player->codec = NULL;
    }
    return 0;
}

int audio_pause(AudioPlayer* player, int pause) {
    if (!player) return -1;
    player->paused = pause;
    return 0;
}

int audio_seek(AudioPlayer* player, int64_t frame) {
    if (!player || !player->codec) return -1;
    audio_pause(player, 1);
    pthread_mutex_lock(&player->lock);
    long pos = codec_seek(player->codec, frame);
    if (pos < 0) pos = 0;
    if (pos > player->totalFrames) pos = player->totalFrames;
    player->currentFrame = pos;
    float tmp[512 * player->channels];
    long r = codec_read_float(player->codec, tmp, 512);
    player->currentFrame += r;
    pthread_mutex_unlock(&player->lock);
    audio_pause(player, 0);
    return player->currentFrame;
}
