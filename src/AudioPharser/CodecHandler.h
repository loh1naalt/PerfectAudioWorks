#ifndef CODECHANDLER_H
#define CODECHANDLER_H

typedef enum {
    CODEC_TYPE_NONE = 0,
    CODEC_TYPE_SNDFILE,
    CODEC_TYPE_MPG123,
    CODEC_TYPE_FFMPEG
} CodecType;

typedef struct CodecHandler CodecHandler;

CodecHandler* codec_open(const char* filename);
int codec_get_channels(CodecHandler* ch);
long codec_get_total_frames(CodecHandler* ch);
long codec_get_current_frame(CodecHandler* ch);
int codec_get_samplerate(CodecHandler* ch);
long codec_read_float(CodecHandler* ch, float* buffer, int frames);
long codec_seek(CodecHandler* ch, long frame);
void codec_close(CodecHandler* ch);

#endif
