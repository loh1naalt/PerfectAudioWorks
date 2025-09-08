#include "portaudio_backend.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int audio_init() {
    return Pa_Initialize();
}

int audio_terminate() {
    return Pa_Terminate();
}


int audio_play(AudioPlayer* player, const char* filename, int device) {
    if (!player || !filename) return -1;


    player->codec = codec_open(filename);
    if (!player->codec) {
        fprintf(stderr, "Failed to open codec for file: %s\n", filename);
        return -1;
    }

    player->channels = codec_get_channels(player->codec);
    player->samplerate = codec_get_samplerate(player->codec);
    player->totalFrames = codec_get_total_frames(player->codec);
    player->currentFrame = 0;
    player->paused = 0;

    if (device == -1) {
        device = Pa_GetDefaultOutputDevice();
        if (device == paNoDevice) {
            fprintf(stderr, "No default output device found\n");
            codec_close(player->codec);
            player->codec = NULL;
            return -1;
        }
    }

    const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(device);
    if (!devInfo) {
        fprintf(stderr, "Invalid output device index: %d\n", device);
        codec_close(player->codec);
        player->codec = NULL;
        return -1;
    }

    int maxChans = devInfo->maxOutputChannels;
    if (player->channels > maxChans) {
        fprintf(stderr, "Codec channels (%d) exceed device max (%d), reducing to max\n",
                player->channels, maxChans);
        player->channels = maxChans;
    }


    PaStreamParameters output;
    memset(&output, 0, sizeof(PaStreamParameters));
    output.device = device;
    output.channelCount = player->channels;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = devInfo->defaultLowOutputLatency;
    output.hostApiSpecificStreamInfo = NULL;
    fprintf(stderr, "Failed to open codec for file: %d\n", player->channels);
    PaError err = Pa_OpenStream(&player->stream,
                                NULL,  
                                &output,
                                player->samplerate,
                                512,   
                                paNoFlag,
                                audio_callback_c,
                                player);
    if (err != paNoError) {
        fprintf(stderr, "Failed to open PortAudio stream: %s\n", Pa_GetErrorText(err));
        codec_close(player->codec);
        player->codec = NULL;
        return -1;
    }

    err = Pa_StartStream(player->stream);
    if (err != paNoError) {
        fprintf(stderr, "Failed to start PortAudio stream: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(player->stream);
        player->stream = NULL;
        codec_close(player->codec);
        player->codec = NULL;
        return -1;
    }

    return 0; 
}


int audio_stop(AudioPlayer* player) {
    if (player->stream) {
        Pa_StopStream(player->stream);
        Pa_CloseStream(player->stream);
        player->stream = NULL;
    }
    if (player->codec) {
        codec_close(player->codec);
        player->codec = NULL;
    }
    return 0;
}

int audio_pause(AudioPlayer* player, int pause) {
    player->paused = pause;
    return 0;
}

int audio_seek(AudioPlayer* player, long frame) {
    if (!player->codec) return -1;
    player->currentFrame = codec_seek(player->codec, frame);
    return player->currentFrame;
}

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
        memset(out, 0, sizeof(float) * frameCount * channels);
        return paContinue;
    }

    long readFrames = codec_read_float(player->codec, out, frameCount);
    player->currentFrame += readFrames;

    if (readFrames < (long)frameCount) {
        memset(out + readFrames * channels, 0, (frameCount - readFrames) * channels * sizeof(float));
        if (player->currentFrame >= player->totalFrames) return paComplete;
    }

    return paContinue;
}
