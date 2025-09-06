#include "CodecHandler.h"
#include "libsndfiledecoder.h"
#include "mpg123decoder.h"
#include <stdlib.h>
#include <string.h>

struct CodecHandler {
    CodecType type;
    void* decoder;
};

CodecHandler* codec_open(const char* filename) {
    CodecHandler* ch = (CodecHandler*)malloc(sizeof(CodecHandler));
    if (!ch) return NULL;
    memset(ch, 0, sizeof(CodecHandler));

    SndFileDecoder* sf = sndfile_open(filename); 
    if (sf) {
        ch->type = CODEC_TYPE_SNDFILE;
        ch->decoder = sf;
        return ch;
    }

    MPG123Decoder* mp3 = MPG123Decoder_open(filename); 
    if (mp3) {
        ch->type = CODEC_TYPE_MPG123;
        ch->decoder = mp3;
        return ch;
    }

    free(ch);
    return NULL;
}

int codec_get_channels(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_get_channels((SndFileDecoder*)ch->decoder);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_get_channels((MPG123Decoder*)ch->decoder);
    return 0;
}

long codec_get_total_frames(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_get_total_frames((SndFileDecoder*)ch->decoder);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_get_total_frames((MPG123Decoder*)ch->decoder);
    return 0;
}

int codec_get_samplerate(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_get_samplerate((SndFileDecoder*)ch->decoder);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_get_samplerate((MPG123Decoder*)ch->decoder);
    return 0;
}

long codec_read_float(CodecHandler* ch, float* buffer, int frames) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_read_float((SndFileDecoder*)ch->decoder, buffer, frames);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_read_float((MPG123Decoder*)ch->decoder, buffer, frames);
    return 0;
}

long codec_seek(CodecHandler* ch, long frame) {
    if (!ch) return -1;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_seek((SndFileDecoder*)ch->decoder, frame);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_seek((MPG123Decoder*)ch->decoder, frame);
    return -1;
}

long codec_get_current_frame(CodecHandler* ch) {
    if (!ch) return -1;
    if (ch->type == CODEC_TYPE_SNDFILE) return sndfile_get_current_frame((SndFileDecoder*)ch->decoder);
    if (ch->type == CODEC_TYPE_MPG123) return MPG123Decoder_get_current_frame((MPG123Decoder*)ch->decoder);
    return -1;
}


void codec_close(CodecHandler* ch) {
    if (!ch) return;
    if (ch->type == CODEC_TYPE_SNDFILE) sndfile_close((SndFileDecoder*)ch->decoder);
    if (ch->type == CODEC_TYPE_MPG123) MPG123Decoder_close((MPG123Decoder*)ch->decoder);
    free(ch);
}
