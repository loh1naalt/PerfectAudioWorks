#include "CodecHandler.h"
#include "libsndfiledecoder.h"
#include "mpg123decoder.h"
#include "ffmpegdecoder.h"
#include "../miscellaneous/file.h"
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
    const char* filetype = get_file_data(filename);

    if (strcmp(filetype, "WAV") == 0 ||
        strcmp(filetype, "FLAC") == 0 ||
        strcmp(filetype, "OGG") == 0 ||
        strcmp(filetype, "AIFF") == 0 ||
        strcmp(filetype, "OPUS") == 0 ||
        strcmp(filetype, "MP3") == 0) {
        SndFileDecoder* sf = sndfile_open(filename);
        if (sf) {
            ch->type = CODEC_TYPE_SNDFILE;
            ch->decoder = sf;
            return ch;
        }
    } 
    // mpg123 is disabled for a while...
    // else if (strcmp(filetype, "MP3") == 0) {
    //     MPG123Decoder* mp3 = MPG123Decoder_open(filename);
    //     if (mp3) {
    //         ch->type = CODEC_TYPE_MPG123;
    //         ch->decoder = mp3;
    //         return ch;
    //     }
    // }
    // FFmpeg supports a wide range of formats
    else {
        FFmpegDecoder* ffdec = ffmpeg_open(filename);
        if (ffdec) {
            ch->type = CODEC_TYPE_FFMPEG;
            ch->decoder = ffdec;
            return ch;
        }
    }

    free(ch);
    return NULL;
}

int codec_get_channels(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_get_channels((SndFileDecoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_get_channels((MPG123Decoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_get_channels((FFmpegDecoder*)ch->decoder);
    }
    return 0;
}

long codec_get_total_frames(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_get_total_frames((SndFileDecoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_get_total_frames((MPG123Decoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_get_total_frames((FFmpegDecoder*)ch->decoder);
    }
    return 0;
}

int codec_get_samplerate(CodecHandler* ch) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_get_samplerate((SndFileDecoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_get_samplerate((MPG123Decoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_get_samplerate((FFmpegDecoder*)ch->decoder);
    }
    return 0;
}

long codec_read_float(CodecHandler* ch, float* buffer, int frames) {
    if (!ch) return 0;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_read_float((SndFileDecoder*)ch->decoder, buffer, frames);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_read_float((MPG123Decoder*)ch->decoder, buffer, frames);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_read_float((FFmpegDecoder*)ch->decoder, buffer, frames);
    }
    return 0;
}

long codec_seek(CodecHandler* ch, long frame) {
    if (!ch) return -1;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_seek((SndFileDecoder*)ch->decoder, frame);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_seek((MPG123Decoder*)ch->decoder, frame);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_seek((FFmpegDecoder*)ch->decoder, frame);
    }
    return -1;
}

long codec_get_current_frame(CodecHandler* ch) {
    if (!ch) return -1;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        return sndfile_get_current_frame((SndFileDecoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_MPG123) {
        return MPG123Decoder_get_current_frame((MPG123Decoder*)ch->decoder);
    }
    if (ch->type == CODEC_TYPE_FFMPEG) {
        return ffmpeg_get_current_frame((FFmpegDecoder*)ch->decoder);
    }
    return -1;
}

void codec_close(CodecHandler* ch) {
    if (!ch) return;
    if (ch->type == CODEC_TYPE_SNDFILE) {
        sndfile_close((SndFileDecoder*)ch->decoder);
    } else if (ch->type == CODEC_TYPE_MPG123) {
        MPG123Decoder_close((MPG123Decoder*)ch->decoder);
    } else if (ch->type == CODEC_TYPE_FFMPEG) {
        ffmpeg_close((FFmpegDecoder*)ch->decoder);
    }
    free(ch);
}
