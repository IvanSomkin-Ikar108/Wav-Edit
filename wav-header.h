#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <string>
#include <cstdint>
#include <vector>

namespace cn
{
    const uint32_t RIFF = 0x52494646;
    const uint32_t WAVE = 0x57415645;
    const uint32_t fmt  = 0x666D7420;
    const uint32_t data = 0x64617461;
}

class WavHeader
{
    private:
    /*supply variables*/
    bool ok_flag = false; 
    /* The "RIFF" chunk descriptor */
    uint32_t  chunk_ID = cn::RIFF;      // RIFF Header Magic header
    uint32_t  chunk_size;               // RIFF Chunk Size
    uint32_t  format = cn::WAVE;        // "WAVE" string
    /* The "fmt" sub-chunk */
    uint32_t  subchunk1_ID = cn::fmt;   // "fmt " string
    uint32_t  subchunk1_size;           // Size of the fmt chunk
    uint16_t  audio_format;             // Audio format 1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t  num_of_channels;          // Number of channels 1=Mono 2=Sterio
    uint32_t  samples_per_sec;          // Sampling Frequency in Hz
    uint32_t  bytes_per_sec;            // Bytes per second
    uint16_t  block_align;              // 2=16-bit mono, 4=16-bit stereo
    uint16_t  bits_per_sample;          // Number of bits per sample
    /* The "data" sub-chunk */
    uint32_t  subchunk2_ID = cn::data;  // "data" string
    uint32_t  subchunk2_size;           // Sampled data length

    public:
    WavHeader (const std::vector<uint8_t>& byte_file);
    WavHeader (const std::string& filepath);
    bool check_validity();
    uint16_t get_num_of_channels();
    uint32_t get_frequency();
    uint16_t get_depth_bit();
    string get_samples_type();
    uint32_t get_bit_per_second();
    double get_length();
};

#endif
