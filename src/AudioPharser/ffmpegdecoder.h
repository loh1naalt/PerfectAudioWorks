#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h> 
#include <libavutil/opt.h>


typedef struct {
    AVFormatContext* fmt_ctx;
    AVCodecContext* audio_codec_ctx;
    SwrContext* swr_ctx;
    AVFrame* frame;
    AVPacket packet;
    int audio_stream_index;
    long long current_frame;
    long long total_frames;
    int channels;
    int samplerate;
    int samples_per_frame;
} FFmpegDecoder;

#ifdef __cplusplus
extern "C" {
#endif

// Opens an audio file and prepares the decoder
FFmpegDecoder* ffmpeg_open(const char* filename);

// Reads decoded audio frames into the buffer
long ffmpeg_read_float(FFmpegDecoder* dec, float* buffer, int frames);

// Seeks to a specific frame
long ffmpeg_seek(FFmpegDecoder* dec, long long frame);

// Gets the total number of frames in the file
long long ffmpeg_get_total_frames(FFmpegDecoder* dec);

// Gets the current playback frame
long long ffmpeg_get_current_frame(FFmpegDecoder* dec);

// Gets the number of channels
int ffmpeg_get_channels(FFmpegDecoder* dec);

// Gets the sample rate
int ffmpeg_get_samplerate(FFmpegDecoder* dec);

// Closes and cleans up the decoder
void ffmpeg_close(FFmpegDecoder* dec);

#ifdef __cplusplus
}
#endif

#endif // FFMPEGDECODER_H
