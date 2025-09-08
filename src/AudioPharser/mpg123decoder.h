#ifndef MPG123_DECODER_H
#define MPG123_DECODER_H

#include <mpg123.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    mpg123_handle *mh;
    int channels;
    long samplerate;
    long current_frame;
    long total_frames;
} MPG123Decoder;

#ifdef __cplusplus
extern "C" {
#endif

MPG123Decoder* MPG123Decoder_open(const char* filename);
int MPG123Decoder_get_channels(const MPG123Decoder* dec);
int MPG123Decoder_get_samplerate(const MPG123Decoder* dec);
long MPG123Decoder_get_total_frames(const MPG123Decoder* dec);
long MPG123Decoder_get_current_frame(const MPG123Decoder* dec);
long MPG123Decoder_read_int16(MPG123Decoder* dec, int16_t* buffer, int frames);
long MPG123Decoder_seek(MPG123Decoder* dec, long frame);
void MPG123Decoder_close(MPG123Decoder* dec);

#ifdef __cplusplus
}
#endif

#endif // MPG123_DECODER_H