#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdio.h>

typedef struct FileInfo {
    char* title;
    char* artist;
    char* album;
    char* genre;
    char* format;
    int samplerate;
    int channels;
    long frames;
    unsigned char* cover_image;  
    size_t cover_size;
} FileInfo;

const char* get_file_data(const char* filename);

#ifdef __cplusplus
}
#endif

#endif