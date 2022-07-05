#include "sound-effects.h"
#include "wav-header.h"
#include <stdexcept>


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

template<typename T>
void write_to_bytes(std::vector<uint8_t>& bytes, uint32_t pos, T val)
{
  memcpy(&bytes[pos], &val, sizeof(T));
}

template<typename T>
T read_from_bytes (std::vector<uint8_t>& bytes, uint32_t pos)
{
  T val = 0;
  memcpy(&val, &bytes[pos], sizeof(T));
  return val;
}

struct FadeAway {
  template<typename T>
  void operator() (std::vector<uint8_t>& bytes, WavHeader& header, FadeOptions& options)
  {  
    uint32_t data_offset   = header.get_data_offset();
    uint16_t block_align   = header.get_block_align();

    uint16_t sample_size = block_align / header.get_num_of_channels();

    uint32_t start_offset = data_offset + ms_to_bytes(header, options.start_ms);
    uint32_t end_offset   = data_offset + ms_to_bytes(header, options.end_ms);

    double step_length = ((1. - options.end_lvl_01) * static_cast<double>(block_align)
                                            / static_cast<double>(end_offset - start_offset));
    double step, revers;
    if (start_offset < end_offset)
    {    
      step = 0;
    }
    else
    {
      step = options.end_lvl_01;
    }

    T old_sample, new_sample;
    for (uint32_t cur_off = start_offset; cur_off < end_offset; cur_off += block_align, step += step_length)
    {
      for (int chan_off = 0; chan_off < block_align; chan_off += sample_size)
      {
        old_sample = read_from_bytes<T>(bytes, cur_off + chan_off);

        new_sample = old_sample - static_cast<T>(static_cast<double>(old_sample) * step);

        write_to_bytes<T>(bytes, cur_off + chan_off, new_sample);
      }
    }
  }
};

struct Reverberation {
  template<typename T>
  void operator() (std::vector<uint8_t>& bytes, WavHeader& header, ReverbOptions& options) {
    uint32_t delay = ms_to_bytes(header,options.delay_ms);

    uint16_t block_align = header.get_block_align();
    uint32_t start_offset = header.get_data_offset(); 

    uint32_t last_block_offset = start_offset + header.get_data_size() - block_align;
    uint32_t reverb_start = start_offset + delay;
    uint16_t sample_size = block_align / header.get_num_of_channels();

    T old_sample, new_sample, delay_sample;
    for (uint32_t cur_off = last_block_offset; cur_off >= reverb_start; cur_off -= block_align)
    {
      for (int chan_off = 0; chan_off < block_align; chan_off += sample_size)
      {
        old_sample = read_from_bytes<T>(bytes, cur_off + chan_off);
        delay_sample = read_from_bytes<T>(bytes, cur_off - delay + chan_off);

        new_sample = old_sample + static_cast<T>(options.decay_01 * static_cast<double>(delay_sample));

        write_to_bytes<T>(bytes, cur_off + chan_off, new_sample);
      }
    }
  }
};

template<typename T, typename F>
void sample_type_selector (std::vector<uint8_t>& bytes, T& options)
{
  WavHeader header = WavHeader(bytes);
  uint16_t sample_size = header.get_block_align() / header.get_num_of_channels();
  
  switch (sample_size)
  {
  case 1:
    F<uint8_t>(bytes, header, options);
    break;
  
  case 2:
    F<uint16_t>(bytes, header, options);
    break;
  
  case 4:
    if (header.get_audio_format() != format::WAVE_FORMAT_IEEE_FLOAT)
    {
      F<uint32_t>(bytes, header, options);
    }
    else
    {
      F<float>(bytes, header, options);
    }
    break;
  
  case 8:
    if (header.get_audio_format() != format::WAVE_FORMAT_IEEE_FLOAT)
    {
      F<uint64_t>(bytes, header, options);
    }
    else
    {
      F<double>(bytes, header, options);
    }

  default:
    throw std::invalid_argument("Error: Invalid sample size or unsupported data format!");
    break;
  } 
}


void effects::trim(std::vector<uint8_t>& bytes, TrimOptions& options)
{
  WavHeader header = WavHeader(bytes);
  uint32_t data_offset = header.get_data_offset();
  uint32_t data_size = header.get_data_size();

  if (!options.end_flag)
  {
    options.end_ms = header.get_length_ms();
  }

  uint32_t start_offset = data_offset + ms_to_bytes(header, options.start_ms);
  uint32_t end_offset   = data_offset + ms_to_bytes(header, options.end_ms);

  bytes.erase(bytes.begin() + end_offset, bytes.begin() + data_offset + data_size);
  bytes.erase(bytes.begin() + data_offset, bytes.begin() + start_offset);

  uint32_t size_change = data_size - (end_offset - start_offset);

  if (data_size % 2 == 1)
  {
    bytes.insert(bytes.begin() + data_offset + data_size - size_change, (uint8_t)0);
  }
  
  write_to_bytes<uint32_t>(bytes, data_offset - 4, data_size - size_change); // Changing Data Subchunk Size 
  write_to_bytes<uint32_t>(bytes, 4, header.get_file_size() - 8 - size_change); // Changing RIFF Chunk Size
};

void effects::fade(std::vector<uint8_t>& bytes, FadeOptions& options)
{
  sample_type_selector<FadeOptions,FadeAway>(bytes,options);
};

void effects::reverb(std::vector<uint8_t>& bytes, ReverbOptions& options)
{
  sample_type_selector<ReverbOptions,Reverberation>(bytes,options);
};
