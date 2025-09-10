#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h> // ntohl

// --- Helpers ---
const char* get_file_format(const char* filename) {
    FILE *file = fopen(filename, "rb");   
    if (!file) return "Unknown";

    unsigned char buffer[64];  
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    if (bytesRead >= 12) {
        if (memcmp(buffer, "RIFF", 4) == 0) return "WAV";
        if (memcmp(buffer, "fLaC", 4) == 0) return "FLAC";
        if (memcmp(buffer, "OggS", 4) == 0) return "OGG";
        if (memcmp(buffer, "FORM", 4) == 0) return "AIFF";
        if (memcmp(buffer, "ID3", 3) == 0) return "MP3";
        if ((buffer[0] == 0xFF) && ((buffer[1] & 0xE0) == 0xE0)) return "MP3";
        if (memcmp(buffer, "MThd", 4) == 0) return "MIDI";
        if (memcmp(buffer, "OpusHead", 8) == 0) return "OPUS";
        if (memcmp(buffer + 28, "OpusHead", 8) == 0) return "OPUS";  // Ogg Opus
        if (memcmp(buffer, "\x30\x26\xB2\x75", 4) == 0) return "WMA";
        if (memcmp(buffer+4, "ftypM4A", 7) == 0 || memcmp(buffer+4, "ftypisom", 8) == 0) return "M4A";
    }

    return "Unknown";
}

// --- MP3 ID3v1 ---
static void parse_id3v1(const char *filename, FileInfo *info) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;

    fseek(f, -128, SEEK_END);
    unsigned char tag[128];
    if (fread(tag, 1, 128, f) != 128) { fclose(f); return; }

    if (memcmp(tag, "TAG", 3) == 0) {
        strncpy(info->title,  (char*)(tag + 3), 30); info->title[30] = '\0';
        strncpy(info->artist, (char*)(tag + 33), 30); info->artist[30] = '\0';
        strncpy(info->album,  (char*)(tag + 63), 30); info->album[30] = '\0';
        snprintf(info->genre, sizeof(info->genre), "Genre ID %d", tag[127]);
    }
    fclose(f);
}

// --- MP3 ID3v2 ---
static void parse_id3v2(const char *filename, FileInfo *info) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;

    unsigned char header[10];
    if (fread(header, 1, 10, f) < 10) { fclose(f); return; }

    if (memcmp(header, "ID3", 3) == 0) {
        int size = ((header[6] & 0x7F) << 21) |
                   ((header[7] & 0x7F) << 14) |
                   ((header[8] & 0x7F) << 7)  |
                   (header[9] & 0x7F);

        unsigned char *data = malloc(size);
        fread(data, 1, size, f);

        int pos = 0;
        while (pos + 10 < size) {
            char frame_id[5] = {0};
            memcpy(frame_id, data + pos, 4);
            int frame_size = (data[pos+4] << 24) | (data[pos+5] << 16) | (data[pos+6] << 8) | data[pos+7];
            if (frame_size <= 0) break;

            if (strcmp(frame_id, "TIT2") == 0)
                strncpy(info->title,  (char*)(data+pos+11), frame_size-1);
            if (strcmp(frame_id, "TPE1") == 0)
                strncpy(info->artist, (char*)(data+pos+11), frame_size-1);
            if (strcmp(frame_id, "TALB") == 0)
                strncpy(info->album,  (char*)(data+pos+11), frame_size-1);
            if (strcmp(frame_id, "TCON") == 0)
                strncpy(info->genre,  (char*)(data+pos+11), frame_size-1);

            pos += 10 + frame_size;
        }
        free(data);
    }
    fclose(f);
}

// --- M4A / MP4 ---
static void parse_m4a(const char *filename, FileInfo *info) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;

    while (!feof(f)) {
        uint32_t size;
        char type[5] = {0};
        if (fread(&size, 4, 1, f) != 1) break;
        if (fread(type, 4, 1, f) != 1) break;

        size = ntohl(size);
        if (size < 8) break;

        if (strcmp(type, "©nam") == 0) {
            fread(info->title, 1, size-8, f);
        } else if (strcmp(type, "©ART") == 0) {
            fread(info->artist, 1, size-8, f);
        } else if (strcmp(type, "©alb") == 0) {
            fread(info->album, 1, size-8, f);
        } else {
            fseek(f, size-8, SEEK_CUR);
        }
    }
    fclose(f);
}

// --- FLAC / OGG VORBIS ---
static void parse_flac(const char *filename, FileInfo *info) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;

    char sig[4];
    fread(sig, 1, 4, f);
    if (memcmp(sig, "fLaC", 4) != 0) { fclose(f); return; }

    unsigned char header[4];
    fread(header, 1, 4, f); // METADATA_BLOCK_HEADER
    int block_type = (header[0] & 0x7F);

    if (block_type == 4) { // VORBIS_COMMENT
        uint32_t len;
        fread(&len, 4, 1, f);
        len = le32toh(len);
        char *vendor = malloc(len+1);
        fread(vendor, 1, len, f);
        vendor[len] = '\0';

        uint32_t count;
        fread(&count, 4, 1, f);
        count = le32toh(count);

        for (uint32_t i = 0; i < count; i++) {
            fread(&len, 4, 1, f);
            len = le32toh(len);
            char *entry = malloc(len+1);
            fread(entry, 1, len, f);
            entry[len] = '\0';

            if (strncasecmp(entry, "TITLE=", 6) == 0) strcpy(info->title, entry+6);
            if (strncasecmp(entry, "ARTIST=", 7) == 0) strcpy(info->artist, entry+7);
            if (strncasecmp(entry, "ALBUM=", 6) == 0) strcpy(info->album, entry+6);
            if (strncasecmp(entry, "GENRE=", 6) == 0) strcpy(info->genre, entry+6);

            free(entry);
        }
        free(vendor);
    }
    fclose(f);
}

// --- WAV (RIFF INFO) ---
static void parse_wav(const char *filename, FileInfo *info) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;

    char riff[4];
    fread(riff, 1, 4, f);
    if (memcmp(riff, "RIFF", 4) != 0) { fclose(f); return; }

    fseek(f, 12, SEEK_SET); // Skip RIFF header
    char chunkId[5] = {0};
    uint32_t chunkSize;

    while (fread(chunkId, 1, 4, f) == 4) {
        fread(&chunkSize, 4, 1, f);
        if (strcmp(chunkId, "LIST") == 0) {
            char type[5] = {0};
            fread(type, 1, 4, f);
            if (strcmp(type, "INFO") == 0) {
                char subId[5] = {0};
                uint32_t subSize;
                fread(subId, 1, 4, f);
                fread(&subSize, 4, 1, f);
                if (strcmp(subId, "INAM") == 0) fread(info->title, 1, subSize, f);
                if (strcmp(subId, "IART") == 0) fread(info->artist, 1, subSize, f);
            }
        } else {
            fseek(f, chunkSize, SEEK_CUR);
        }
    }
    fclose(f);
}

// --- Main dispatcher ---
int get_metadata(const char *filename, FileInfo *info) {
    memset(info, 0, sizeof(*info));

    const char *format = get_file_format(filename);
    strncpy(info->format, format, sizeof(info->format)-1);

    if (strcmp(format, "MP3") == 0) {
        parse_id3v2(filename, info);
        parse_id3v1(filename, info);
    } else if (strcmp(format, "M4A") == 0) {
        parse_m4a(filename, info);
    } else if (strcmp(format, "FLAC") == 0 || strcmp(format, "OGG") == 0 || strcmp(format, "OPUS") == 0) {
        parse_flac(filename, info);
    } else if (strcmp(format, "WAV") == 0) {
        parse_wav(filename, info);
    } else {
        return -1; 
    }

    return 0;
}
