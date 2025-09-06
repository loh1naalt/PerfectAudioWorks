#ifndef MPG123DECODER_H
#define MPG123DECODER_H

#include <mpg123.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MPG123Decoder {
    mpg123_handle *mh;
    long current_frame;
    int channels;
    long samplerate;
    long total_frames;
} MPG123Decoder;

MPG123Decoder* MPG123Decoder_open(const char* filename);
int MPG123Decoder_get_channels(const MPG123Decoder* decoder);
int MPG123Decoder_get_samplerate(const MPG123Decoder* decoder);
long MPG123Decoder_get_total_frames(const MPG123Decoder* decoder);
long MPG123Decoder_read_float(MPG123Decoder* decoder, float* buffer, int frames);
long MPG123Decoder_seek(MPG123Decoder* dec, long frame);
long MPG123Decoder_get_current_frame(const MPG123Decoder* decoder);
void MPG123Decoder_close(MPG123Decoder* decoder);

#ifdef __cplusplus
}
#endif

#endif 
