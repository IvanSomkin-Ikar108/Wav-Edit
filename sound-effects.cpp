#include "sound-effects.h"
#include "wav-header.h"
#include <stdexcept>

template<typename T>
void write_to_bytes(std::vector<uint8_t>& bytes, uint32_t pos, T val)
{
  for (int i = 0; i < sizeof(T); ++i)
  {
    bytes[pos + i] = (val >> (8 * i)) & 0xFF;
  }
}

template<typename T>
T read_from_bytes (std::vector<uint8_t>& bytes, uint32_t pos)
{
  T result = 0;
  for (int i = sizeof(T) - 1; i > 0; ++i)
  {
    result = (result + bytes[pos + i]) << 8
  }

  result += bytes[pos + i]

  return result;
}

template<typename T>
void level(std::vector<uint8_t>& bytes, WavHeader& header, uint32_t start_ms, uint32_t end_ms, double levels)
{  
  uint32_t data_offset   = header.get_data_offset();
  uint16_t channel_count = header.get_num_of_channels();
  uint16_t block_align   = header.get_block_align();

  uint32_t start_offset = data_offset + ms_to_bytes(header, start_ms);
  uint32_t end_offset   = data_offset + ms_to_bytes(header, end_ms);

  double step_lenght = ((1.f - levels) * static_cast<double>(block_align)
                                     / static_cast<double>(end_offset - start_offset));
  double step, revers;
  if (start_offset < end_offset)
  {    
    step = 0;
  }
  else
  {
    step = level;
  }

  T old_sample, new_sample;
  for (uint32_t i = start_offset; i < end_offset; i += block_align, step += step_lenght)
  {
    for (int channel = 0; channel < channel_count; ++channel)
    {
      old_sample = read_from_bytes(bytes, i);
      new_sample = old_sample - static_cast<T>(static_cast<double>(old_sample) * step);
      write_to_bytes<T>(bytes, i, new_sample);
    }
  }
}

uint32_t ms_to_bytes (WavHeader& header, uint32_t point_ms)
{
  uint32_t samples_count = header.get_frequency() * point_ms / 1000;
  uint32_t bytes_count = samples_count * header.get_block_align();

  if (bytes_count > header.get_data_size())
  {
    throw std::invalid_argument("Error: Time point out of data range!");
  }

  return bytes_count;
}

void effects::cut(std::vector<uint8_t>& bytes, uint32_t start_ms, uint32_t end_ms)
{
  WavHeader header = WavHeader(bytes);
  uint32_t data_offset = header.get_data_offset();
  uint32_t data_size = header.get_data_size();

  uint32_t start_offset = data_offset + ms_to_bytes(header, start_ms);
  uint32_t end_offset   = data_offset + ms_to_bytes(header, end_ms);

  bytes.erase(bytes.begin() + end_offset, bytes.begin() + data_offset + data_size);
  bytes.erase(bytes.begin() + data_offset, bytes.begin() + start_offset);

  uint32_t size_change = data_size - (end_offset - start_offset);

  if (data_size % 2 == 1)
  {
    bytes.insert(bytes.begin() + data_offset + data_size - size_change, (uint8_t)0);
  }
  
  write_to_bytes<uint32_t>(bytes, data_offset - 4, data_size - size_change);
  write_to_bytes<uint32_t>(bytes, 4, header.get_file_size() - 8 - size_change); //8 is RIFF + size field

  //Chunk_size offset = 4, Data_chunk_size offset = data_offset - 4
};

void effects::cut(std::vector<uint8_t>& bytes, uint32_t start_ms)
{
  WavHeader header = WavHeader(bytes);
  cut(bytes, start_ms, header.get_length_ms());
};

void effects::levels(std::vector<uint8_t>& bytes, uint32_t start_ms, uint32_t end_ms, double levels)
{
  WavHeader header = WavHeader(bytes);
  uint16_t sample_size = header.get_block_align() / header.get_num_of_channels();

  level<uint16_t>(bytes, header, start_ms, end_ms, levels);
  switch (sample_size)
  {
  case 1:
    level<uint8_t>(bytes, header, start_ms, end_ms, levels);
    break;
  
  case 2:
    level<uint16_t>(bytes, header, start_ms, end_ms, levels);
    break;
  
  case 4:
    if (header.get_audio_format() !=3)
    {
      level<uint32_t>(bytes, header, start_ms, end_ms, levels);
    }
    else
    {
      level<float>(bytes, header, start_ms, end_ms, levels);
    }
    break;
  
  default:
    break;
  } 
};

void effects::reverb(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256)
{

};