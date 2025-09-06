#include "mpg123decoder.h"

MPG123Decoder* MPG123Decoder_open(const char* filename) {
    if (mpg123_init() != MPG123_OK) return NULL;

    MPG123Decoder* dec = (MPG123Decoder*)malloc(sizeof(MPG123Decoder));
    if (!dec) return NULL;

    memset(dec, 0, sizeof(MPG123Decoder));

    dec->mh = mpg123_new(NULL, NULL);
    if (!dec->mh) {
        free(dec);
        return NULL;
    }

    if (mpg123_open(dec->mh, filename) != MPG123_OK) {
        mpg123_delete(dec->mh);
        free(dec);
        return NULL;
    }

    long rate;
    int channels, encoding;
    if (mpg123_getformat(dec->mh, &rate, &channels, &encoding) != MPG123_OK) {
        mpg123_close(dec->mh);
        mpg123_delete(dec->mh);
        free(dec);
        return NULL;
    }

    mpg123_format_none(dec->mh);
    mpg123_format(dec->mh, rate, channels, encoding);

    dec->channels = channels;
    dec->samplerate = rate;
    dec->current_frame = 0;
    dec->total_frames = mpg123_length(dec->mh);

    return dec;
}

int MPG123Decoder_get_channels(const MPG123Decoder* dec) {
    if (!dec) return 0;
    return dec->channels;
}

int MPG123Decoder_get_samplerate(const MPG123Decoder* dec) {
    if (!dec) return 0;
    return dec->samplerate;
}

long MPG123Decoder_get_total_frames(const MPG123Decoder* dec) {
    if (!dec) return 0;
    return dec->total_frames;
}

long MPG123Decoder_get_current_frame(const MPG123Decoder* dec) {
    if (!dec) return -1;
    return dec->current_frame;
}

long MPG123Decoder_read_float(MPG123Decoder* dec, float* buffer, int frames) {
    if (!dec || !buffer) return 0;

    size_t samples_to_read = frames * dec->channels;
    size_t samples_read = 0;
    int err = mpg123_read(dec->mh, (unsigned char*)buffer, samples_to_read * sizeof(float), &samples_read);

    if (err != MPG123_OK && err != MPG123_DONE) return 0;

    dec->current_frame += samples_read / dec->channels;
    return samples_read / dec->channels;
}


long MPG123Decoder_seek(MPG123Decoder* dec, long frame) {
    if (!dec || !dec->mh) return -1;

    off_t result = mpg123_seek(dec->mh, frame, SEEK_SET); 
    if (result < 0) return -1;

    dec->current_frame = result;
    return result;
}


void MPG123Decoder_close(MPG123Decoder* dec) {
    if (!dec) return;
    if (dec->mh) {
        mpg123_close(dec->mh);
        mpg123_delete(dec->mh);
    }
    free(dec);
}
