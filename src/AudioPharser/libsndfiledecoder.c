#include "libsndfiledecoder.h"
#include <stdlib.h>
#include <string.h>

SndFileDecoder* sndfile_open(const char* filename) {

    SndFileDecoder* dec = (SndFileDecoder*)malloc(sizeof(SndFileDecoder));
    memset(dec, 0, sizeof(SndFileDecoder));
    dec->file = sf_open(filename, SFM_READ, &dec->info);
    if (!dec->file) {
        free(dec);
        return NULL;
    }
    dec->current_frame = 0;
    return dec;
}

sf_count_t sndfile_read_float(SndFileDecoder* dec, float* buffer, int frames) {
    if (!dec || !dec->file || !buffer) return 0;
    sf_count_t read = sf_read_float(dec->file, buffer, frames * dec->info.channels);
    dec->current_frame += read / dec->info.channels;
    return read;
}

long sndfile_get_current_frame(SndFileDecoder* dec) {
    if (!dec) return -1;
    return dec->current_frame; 
}

sf_count_t sndfile_seek(SndFileDecoder* dec, sf_count_t frame) {
    if (!dec || !dec->file) return -1;
    sf_count_t result = sf_seek(dec->file, frame, SF_SEEK_SET);
    if (result >= 0) dec->current_frame = result;
    return result;
}

int sndfile_get_channels(SndFileDecoder* dec) { return dec ? dec->info.channels : 0; }
long sndfile_get_total_frames(SndFileDecoder* dec) { return dec ? dec->info.frames : 0; }
int sndfile_get_samplerate(SndFileDecoder* dec) { return dec ? dec->info.samplerate : 0; }

void sndfile_close(SndFileDecoder* dec) {
    if (!dec) return;
    if (dec->file) sf_close(dec->file);
    free(dec);
}
