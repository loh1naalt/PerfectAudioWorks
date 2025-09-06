#include <stdio.h>
#include <string.h>

const char* get_file_data(const char* filename) {
    FILE *file = fopen(filename, "rb");   
    if (!file) return "Unknown";

    unsigned char buffer[1024];  
    size_t bytesRead = 0;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if(bytesRead >= 4){
            if (memcmp(buffer, "RIFF", 4) == 0) return "WAV";


            if (memcmp(buffer, "fLaC", 4) == 0) return "FLAC";


            if (memcmp(buffer, "OggS", 4) == 0) return "OGG";


            if (memcmp(buffer, "FORM", 4) == 0) return "AIFF";

            if (memcmp(buffer, "ID3", 3) == 0) return "MP3";

            if ((buffer[0] == 0xFF) && ((buffer[1] & 0xE0) == 0xE0)) return "MP3";

            if (memcmp(buffer, "MThd", 4) == 0) return "MIDI";

            if (bytesRead >= 8 && memcmp(buffer, "OpusHead", 8) == 0) return "OPUS";

            if (bytesRead >= 36 && memcmp(buffer + 28, "OpusHead", 8) == 0) return "OPUS";

            if (bytesRead >= 4 && memcmp(buffer, "\x30\x26\xB2\x75", 4) == 0) return "WMA";

            return "Unknown";
            }
    }
    

    fclose(file);
    return "Unknown";
}

// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         printf("Usage: %s <audio_file>\n", argv[0]);
//         return 1;
//     }

//     const char* filename = argv[1];
//     const char* format = get_file_data(filename);

//     printf("File: %s\nDetected format: %s\n", filename, format);

//     return 0;
// }
