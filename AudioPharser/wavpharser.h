#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

struct Twavheader
{
    char chunk_ID[4];              //  4  riff_mark[4];
    uint32_t chunk_size;           //  4  file_size;
    char format[4];                //  4  wave_str[4];
 
    char sub_chunk1_ID[4];         //  4  fmt_str[4];
    uint32_t sub_chunk1_size;      //  4  pcm_bit_num;
    uint16_t audio_format;         //  2  pcm_encode;
    uint16_t num_channels;         //  2  sound_channel;
    uint32_t sample_rate;          //  4  pcm_sample_freq;
    uint32_t byte_rate;            //  4  byte_freq;
    uint16_t block_align;          //  2  block_align;
    uint16_t bits_per_sample;      //  2  sample_bits;
 
    char sub_chunk2_ID[4];         //  4  data_str[4];
    uint32_t sub_chunk2_size;      //  4  sound_size;
};                                 // 44  bytes TOTAL
 
// int getSampleRate(std::string fname){
//     std::ifstream wavfile(fname, std::ios::binary);
 
//     if(wavfile.is_open())
//     {
//         // Read the WAV header
//         Twavheader wav;
//         wavfile.read(reinterpret_cast<char*>(&wav), sizeof(Twavheader));
 
//         // If the file is a valid WAV file
//         if (std::string(wav.format, 4) != "WAVE" || std::string(wav.chunk_ID, 4) != "RIFF")
//         {
//             wavfile.close();
//             std::cerr << "Not a WAVE or RIFF!" << std::endl;
//             return 1;
//         }
//         return wav.sample_rate;

//     }
 
// }
// int getBitsPerSample(std::string fname){
//     std::ifstream wavfile(fname, std::ios::binary);
 
//     if(wavfile.is_open())
//     {
//         // Read the WAV header
//         Twavheader wav;
//         wavfile.read(reinterpret_cast<char*>(&wav), sizeof(Twavheader));
 
//         // If the file is a valid WAV file
//         if (std::string(wav.format, 4) != "WAVE" || std::string(wav.chunk_ID, 4) != "RIFF")
//         {
//             wavfile.close();
//             std::cerr << "Not a WAVE or RIFF!" << std::endl;
//             return 1;
//         }
//         return wav.bits_per_sample;

//     }
 
// }