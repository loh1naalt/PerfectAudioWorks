#include "ffmpegdecoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>

FFmpegDecoder* ffmpeg_open(const char* filename) {
    FFmpegDecoder* dec = (FFmpegDecoder*)malloc(sizeof(FFmpegDecoder));
    if (!dec) return NULL;
    memset(dec, 0, sizeof(FFmpegDecoder));
    dec->audio_stream_index = -1;

    // Open input
    if (avformat_open_input(&dec->fmt_ctx, filename, NULL, NULL) < 0) goto fail;
    if (avformat_find_stream_info(dec->fmt_ctx, NULL) < 0) goto fail;

    dec->audio_stream_index = av_find_best_stream(dec->fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (dec->audio_stream_index < 0) goto fail;

    AVStream* audio_stream = dec->fmt_ctx->streams[dec->audio_stream_index];
    const AVCodec* codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    if (!codec) goto fail;

    dec->audio_codec_ctx = avcodec_alloc_context3(codec);
    if (!dec->audio_codec_ctx) goto fail;
    if (avcodec_parameters_to_context(dec->audio_codec_ctx, audio_stream->codecpar) < 0) goto fail;
    if (avcodec_open2(dec->audio_codec_ctx, codec, NULL) < 0) goto fail;

    dec->frame = av_frame_alloc();
    if (!dec->frame) goto fail;

    dec->swr_ctx = swr_alloc();
    if (!dec->swr_ctx) goto fail;

    // Input channel layout
    AVChannelLayout in_ch_layout;
    if (audio_stream->codecpar->ch_layout.nb_channels > 0) {
        av_channel_layout_copy(&in_ch_layout, &audio_stream->codecpar->ch_layout);
    } else {
        av_channel_layout_default(&in_ch_layout, audio_stream->codecpar->ch_layout.nb_channels > 0 ? audio_stream->codecpar->ch_layout.nb_channels : 2);
    }

    av_opt_set_chlayout(dec->swr_ctx, "in_channel_layout", &in_ch_layout, 0);
    av_opt_set_int(dec->swr_ctx, "in_sample_rate", dec->audio_codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(dec->swr_ctx, "in_sample_fmt", dec->audio_codec_ctx->sample_fmt, 0);

    // Output channel layout (stereo)
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 2);
    av_opt_set_chlayout(dec->swr_ctx, "out_channel_layout", &out_ch_layout, 0);
    av_opt_set_int(dec->swr_ctx, "out_sample_rate", dec->audio_codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(dec->swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

    if (swr_init(dec->swr_ctx) < 0) goto fail;

    dec->channels = 2; // stereo output
    dec->samplerate = dec->audio_codec_ctx->sample_rate;
    dec->total_frames = audio_stream->duration > 0 ? audio_stream->duration * dec->samplerate / AV_TIME_BASE : 0;
    dec->current_frame = 0;

    return dec;

fail:
    if (dec->swr_ctx) swr_free(&dec->swr_ctx);
    if (dec->frame) av_frame_free(&dec->frame);
    if (dec->audio_codec_ctx) avcodec_free_context(&dec->audio_codec_ctx);
    if (dec->fmt_ctx) avformat_close_input(&dec->fmt_ctx);
    free(dec);
    return NULL;
}




long ffmpeg_read_float(FFmpegDecoder* dec, float* buffer, int frames) {
    if (!dec || !dec->audio_codec_ctx || !dec->swr_ctx) return 0;
    
    long frames_filled = 0;
    
    while (frames_filled < frames) {
        int ret = avcodec_receive_frame(dec->audio_codec_ctx, dec->frame);
        
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            if (ret == AVERROR_EOF) return frames_filled;
            
            // Need a new packet
            if (av_read_frame(dec->fmt_ctx, &dec->packet) < 0) {
                // End of file
                return frames_filled;
            }

            if (dec->packet.stream_index == dec->audio_stream_index) {
                if (avcodec_send_packet(dec->audio_codec_ctx, &dec->packet) < 0) {
                    av_packet_unref(&dec->packet);
                    continue;
                }
            }
            av_packet_unref(&dec->packet);
            continue;
        } else if (ret < 0) {
            fprintf(stderr, "Error decoding frame\n");
            return frames_filled;
        }

        int converted_frames = swr_convert(dec->swr_ctx,
                                         (uint8_t**)&buffer,
                                         frames - frames_filled,
                                         (const uint8_t**)dec->frame->extended_data,
                                         dec->frame->nb_samples);
        
        if (converted_frames < 0) {
            fprintf(stderr, "Error converting samples\n");
            av_frame_unref(dec->frame);
            return frames_filled;
        }
        
        frames_filled += converted_frames;
        dec->current_frame += converted_frames;
        av_frame_unref(dec->frame);
    }

    return frames_filled;
}

long ffmpeg_seek(FFmpegDecoder* dec, long long frame) {
    if (!dec || !dec->fmt_ctx) return -1;
    
    long long timestamp = frame * AV_TIME_BASE / dec->samplerate;
    
    int ret = av_seek_frame(dec->fmt_ctx, dec->audio_stream_index, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0) {
        fprintf(stderr, "Error seeking\n");
        return -1;
    }
    
    avcodec_flush_buffers(dec->audio_codec_ctx);
    dec->current_frame = frame;
    return frame;
}

long long ffmpeg_get_current_frame(FFmpegDecoder* dec) {
    if (!dec) return -1;
    return dec->current_frame;
}

long long ffmpeg_get_total_frames(FFmpegDecoder* dec) {
    if (!dec) return -1;
    return dec->total_frames;
}

int ffmpeg_get_channels(FFmpegDecoder* dec) {
    if (!dec) return 0;
    return dec->channels;
}

int ffmpeg_get_samplerate(FFmpegDecoder* dec) {
    if (!dec) return 0;
    return dec->samplerate;
}

void ffmpeg_close(FFmpegDecoder* dec) {
    if (!dec) return;
    if (dec->swr_ctx) swr_free(&dec->swr_ctx);
    if (dec->frame) av_frame_free(&dec->frame);
    if (dec->audio_codec_ctx) avcodec_free_context(&dec->audio_codec_ctx);
    if (dec->fmt_ctx) avformat_close_input(&dec->fmt_ctx);
    av_packet_unref(&dec->packet);
    free(dec);
}
