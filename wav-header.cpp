#include "wav-header.h"
#include "readfile.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace id
{
  const uint32_t RIFF = 0x52494646;
  const uint32_t WAVE = 0x57415645;
  const uint32_t fmt  = 0x666D7420;
  const uint32_t data = 0x64617461;
  const uint32_t fact = 0x66616374;
}

namespace type
{
  const std::string UINT8_T   = "uint8_t";
  const std::string UINT16_T  = "uint16_t";
  const std::string UINT32_T  = "uint32_t";
  const std::string FLOAT     = "float";
  const std::string DOUBLE    = "double";
}

using std::string;

uint32_t _4x8_to_32_be(const std::vector<uint8_t> &byte_file, size_t idx)
{
  return byte_file[idx] << 24 | (byte_file[idx + 1] << 16) | (byte_file[idx + 2] << 8) | (byte_file[idx + 3]);
}

uint32_t _4x8_to_32_le(const std::vector<uint8_t> &byte_file, size_t idx)
{
  return byte_file[idx] | (byte_file[idx + 1] << 8) | (byte_file[idx + 2] << 16) | (byte_file[idx + 3] << 24);
}

uint32_t _2x8_to_16_le(const std::vector<uint8_t> &byte_file, size_t idx)
{
  return byte_file[idx] | (byte_file[idx + 1] << 8);
}

WavHeader::WavHeader(const std::vector<uint8_t> &byte_file)
{
  if (byte_file.size() < 44)
  {
    throw std::invalid_argument("Error: Bad file - File is too small!");
  }

  size_t offset = 0;
  chunk_ID   = _4x8_to_32_be(byte_file, 0);
  chunk_size = _4x8_to_32_le(byte_file, 4);
  format     = _4x8_to_32_be(byte_file, 8);

  subchunk1_ID    = _4x8_to_32_be(byte_file, 12);
  subchunk1_size  = _4x8_to_32_le(byte_file, 16);
  audio_format    = _2x8_to_16_le(byte_file, 20);
  num_of_channels = _2x8_to_16_le(byte_file, 22);
  samples_per_sec = _4x8_to_32_le(byte_file, 24);
  bytes_per_sec   = _4x8_to_32_le(byte_file, 28);
  block_align     = _2x8_to_16_le(byte_file, 32);
  bits_per_sample = _2x8_to_16_le(byte_file, 34);
  offset = 36;

  if (audio_format != format::WAVE_FORMAT_PCM)
  {
    extension_size = _2x8_to_16_le(byte_file, offset);
    if (audio_format == format::WAVE_FORMAT_EXTENSIBLE)
    {
      subformat = _2x8_to_16_le(byte_file, offset + 8);
    }
    offset += extension_size + 2;
  }

  uint32_t next_subchunk_ID = 0, next_subchunk_size = 0;
  while (next_subchunk_ID != id::data && offset < chunk_size)
  {
    next_subchunk_ID = _4x8_to_32_be(byte_file, offset);
    next_subchunk_size = _4x8_to_32_le(byte_file, offset + 4);
    
    offset += 8;
    data_offset = offset;
    offset += next_subchunk_size + next_subchunk_size % 2;
  }
   
  subchunk2_ID = next_subchunk_ID;
  subchunk2_size = next_subchunk_size;

  if (!check_validity())
  {
    throw std::invalid_argument("Error: Invalid header!");
  }
}

WavHeader::WavHeader(const char* file_path) : WavHeader(readfile(file_path)) {}

bool WavHeader::check_validity()
{
  if (chunk_ID != id::RIFF || format != id::WAVE || subchunk1_ID != id::fmt || subchunk2_ID != id::data)
  {
    return false;
  }

  if (audio_format == format::WAVE_FORMAT_PCM && subchunk1_size != 16)
  {
    return false;
  }

  if (audio_format == format::WAVE_FORMAT_EXTENSIBLE && subchunk1_size != 40)
  {
    return false;
  }

  if (bits_per_sample > 64)
  {
    return false;
  }

  return true;
}

uint16_t WavHeader::get_audio_format()
{
  if (audio_format != format::WAVE_FORMAT_EXTENSIBLE)
  {
    return audio_format;
  }
  return subformat;
}

uint16_t WavHeader::get_num_of_channels()
{
 return num_of_channels; 
}

uint32_t WavHeader::get_frequency()
{
  return samples_per_sec;
}

// Can return 0, if bits_per_sample is not used in format
uint16_t WavHeader::get_bit_depth()
{
  return bits_per_sample; 
}

std::string WavHeader::get_sample_type()
{
  if (audio_format == format::WAVE_FORMAT_IEEE_FLOAT || subformat == format::WAVE_FORMAT_IEEE_FLOAT)
  {
    if (bits_per_sample <= 32)
    {
      return type::FLOAT;
    }
    return type::DOUBLE;
  }
  if (bits_per_sample <= 8)
  {
    return type::UINT8_T;
  }
  if (bits_per_sample <= 16)
  {
    return type::UINT16_T;
  } 
  return type::UINT32_T;
}

uint32_t WavHeader::get_bits_per_sec()
{
  return bytes_per_sec * 8;
}

uint32_t WavHeader::get_length_ms()
{
  return subchunk2_size * 1000 / bytes_per_sec; 
}

uint32_t WavHeader::get_data_offset()
{
  return data_offset; 
}

uint32_t WavHeader::get_data_size()
{
  return subchunk2_size; 
}

uint32_t WavHeader::get_file_size()
{
  return chunk_size + 8;
}

uint16_t WavHeader::get_block_align()
{
  return block_align;
}

std::string WavHeader::to_string()
{
  std::stringstream str;
  str << "RIFF chunk size: "            << chunk_size      << "\n"
      << "FMT  subchunk size: "         << subchunk1_size  << "\n"
      << "Audio compression format: "   << audio_format    << "\n"
      << "Number of channels: "         << num_of_channels << "\n"
      << "Sampling frequency in Hz: "   << samples_per_sec << "\n"
      << "Bytes per second: "           << bytes_per_sec   << "\n" 
      << "Bytes per block of samples: " << block_align     << "\n"
      << "Number of bits per sample: "  << bits_per_sample << "\n" 
      << "Size of extension for non-PCM formats: "
                                        << extension_size  << "\n" 
      << "Audio subformat of extensible format: "
                                        << subformat       << "\n"
      << "Sampled data length: "        << subchunk2_size;
  return str.str(); 
}
