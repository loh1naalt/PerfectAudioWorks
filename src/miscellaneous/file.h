#pragma once
#include <stddef.h>

typedef struct FileInfo {
    char title[128];
    char artist[128];
    char album[128];
    char genre[64];
    char format[16];
    int samplerate;
    int channels;
    long frames;
    unsigned char* cover_image;  
    size_t cover_size;
} FileInfo;

#ifdef __cplusplus
extern "C" {
#endif

int get_metadata(const char *filename, FileInfo *info);
const char* get_file_format(const char* filename);
#ifdef __cplusplus
}
#endif
