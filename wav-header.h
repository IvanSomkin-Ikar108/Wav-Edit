#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <string>
#include <cstdint>
#include <vector>

// Can work incorrectly with GSM 6.10 or other such compressed formats
// If format doesn't use bits_per_sample, can't calculate bit depth
class WavHeader
{
private:
  /* The "RIFF" chunk descriptor */
  uint32_t chunk_ID;            // RIFF Header Magic header
  uint32_t chunk_size;          // RIFF Chunk Size
  uint32_t format;              // "WAVE" string
  /* The "fmt" sub-chunk */
  uint32_t subchunk1_ID;        // "fmt " string
  uint32_t subchunk1_size;      // Size of the fmt chunk
  uint16_t audio_format;        // Audio format 1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
  uint16_t num_of_channels;     // Number of channels 1=Mono 2=Sterio
  uint32_t samples_per_sec;     // Sampling Frequency in Hz
  uint32_t bytes_per_sec;       // Bytes per second
  uint16_t block_align;         // Bytes per block of samples (sample size * num of channels)
  uint16_t bits_per_sample;     // Number of bits per sample
  uint16_t extension_size = 0;  // Size of extension for non-PCM formats
  uint16_t subformat = 0;       // Audio subformat of WAVE_FORMAT_EXTENSIBLE
  /* The "data" sub-chunk */
  uint32_t subchunk2_ID;        // "data" string
  uint32_t subchunk2_size;      // Sampled data length
  
public:
  WavHeader(const std::vector<uint8_t> &byte_file);
  WavHeader(const std::string &filepath);
  bool check_validity();
  uint16_t get_num_of_channels();
  uint32_t get_frequency();
  uint16_t get_bit_depth();
  std::string get_sample_type();
  uint32_t get_bits_per_sec();
  double get_length();
  std::string to_string();
};

#endif