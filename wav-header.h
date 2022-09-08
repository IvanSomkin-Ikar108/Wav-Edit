#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <string>
#include <cstdint>
#include <vector>

namespace format
{
  const uint16_t WAVE_FORMAT_PCM        = 0x0001;
  const uint16_t WAVE_FORMAT_IEEE_FLOAT = 0x0003;
  const uint16_t WAVE_FORMAT_EXTENSIBLE = 0xFFFE;
}

// Can work incorrectly with GSM 6.10 or other such compressed formats
// If format doesn't use bits_per_sample, can't calculate bit depth
//
// Throws std::invalid_argument exception if file is too small to contain WAVE header 
// Throws std::invalid_argument exception if file contains invalid WAVE header 
//
// The file_path constructor:
// Throws std::invalid_argument exception if file_path does not exist
// Throws std::runtime_error if error while reading file
class WavHeader
{
private:
  /* The "RIFF" chunk descriptor */
  uint32_t chunk_ID;            // RIFF Header Magic header
  uint32_t chunk_size;          // RIFF Chunk Size
  uint32_t format;              // "WAVE" string
  /* The "fmt " sub-chunk */
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
  /* Offset off the data chunk */
  uint32_t data_offset;

public:
  WavHeader(const std::vector<uint8_t> &byte_file);
  WavHeader(const char* file_path);
  bool check_validity();
  uint16_t get_audio_format();
  uint16_t get_num_of_channels();
  uint32_t get_frequency();
  uint16_t get_bit_depth();
  std::string get_sample_type();
  uint32_t get_bits_per_sec();
  uint32_t get_length_ms();
  uint32_t get_data_offset();
  uint32_t get_data_size();
  uint32_t get_file_size();
  uint16_t get_block_align();
  std::string to_string();
};

#endif
