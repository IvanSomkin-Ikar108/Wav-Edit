#include <iostream>
#include "wav-header.h"
#include "readfile.h"

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
  if (byte_file.size() < 80)
  {
    throw std::invalid_argument("Error: Bad file - File is too small!");
  }

  size_t offset = 0;
  chunk_ID = _4x8_to_32_be(byte_file, 0);
  chunk_size = _4x8_to_32_le(byte_file, 4);
  format = _4x8_to_32_be(byte_file, 8);

  subchunk1_ID = _4x8_to_32_be(byte_file, 12);
  subchunk1_size = _4x8_to_32_le(byte_file, 16);
  audio_format = _2x8_to_16_le(byte_file, 20);
  num_of_channels = _2x8_to_16_le(byte_file, 22);
  samples_per_sec = _4x8_to_32_le(byte_file, 24);
  bytes_per_sec = _4x8_to_32_le(byte_file, 28);
  block_align = _2x8_to_16_le(byte_file, 32);
  bits_per_sample = _2x8_to_16_le(byte_file, 34);
  extension_size = 0;

  if (audio_format != format::WAVE_FORMAT_PCM)
  {
    extension_size = _2x8_to_16_le(byte_file, 36);
  }

  offset += extension_size;

  fact_size = 0;
  uint32_t next_subchunk_ID = _4x8_to_32_be(byte_file, 36 + offset);
  if (next_subchunk_ID == id::fact)
  {
    fact_ID = next_subchunk_ID;
    fact_size = _4x8_to_32_le(byte_file, 36 + offset);
    sample_length = _4x8_to_32_le(byte_file, 40 + offset);
    next_subchunk_ID = _4x8_to_32_be(byte_file, 36 + offset + 8 + fact_size);
  }

  offset += fact_size;
  
  subchunk2_ID = next_subchunk_ID;
  subchunk2_size = _4x8_to_32_le(byte_file, 40 + offset);
}

WavHeader::WavHeader(const string& filepath) : WavHeader(readfile(filepath, 80)) {}

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

  if (20 + subchunk1_size + subchunk2_size + subchunk2_size % 2 != chunk_size)
  {
    return false;
  }

  return true;
}

uint16_t WavHeader::get_num_of_channels()
{
  if (check_validity())
  {
    return num_of_channels;
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}

uint32_t WavHeader::get_frequency()
{
  if (check_validity())
  {
    return samples_per_sec;
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}

uint16_t WavHeader::get_depth_bit()
{
  if (check_validity())
  {
    return bits_per_sample;
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}

std::string WavHeader::get_samples_type()
{
  if (check_validity())
  {
    if (block_align == 2)
    {
      return "uint16_t";
    }
    if (block_align == 4)
    {
      return "uint32_t";
    }
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}

uint32_t WavHeader::get_bit_per_second()
{
  if (check_validity())
  {
    return bytes_per_sec * 8;
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}

double WavHeader::get_length()
{
  if (check_validity())
  {
    return double(subchunk2_size) / double(bytes_per_sec);
  }
  throw std::invalid_argument("Error: Can't read invalid header!");
}
