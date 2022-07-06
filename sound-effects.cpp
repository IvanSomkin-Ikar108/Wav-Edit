#include "sound-effects.h"
#include "wav-header.h"
#include "mode-options.h"

#include <stdexcept>
#include <limits>
#include <type_traits>


/* Support functions */

template<typename T>
T read_from_bytes(std::vector<uint8_t>& bytes, uint32_t pos);

template<typename T>
void write_to_bytes(std::vector<uint8_t>& bytes, uint32_t pos, T val);

uint32_t ms_to_byte_count(WavHeader& header, uint32_t time_ms);


/* How we launch effects */

// Trim effect.
void effect(std::vector<uint8_t>& bytes, WavHeader& header, TrimOptions& options);

// Fade effect.
template <typename T>
void effect(std::vector<uint8_t>& bytes, WavHeader& header, FadeOptions& options);

// Reverb effect.
template <typename T>
void effect(std::vector<uint8_t>& bytes, WavHeader& header, ReverbOptions& options);

// Switch function for effects that can work with different sample types.
// The function gets sample size from WAV header and passes the sample type to effect template.
// The effect template is then chosen based on the O type of the options.
template <typename O>
void effect_with_type_switch(std::vector<uint8_t>& bytes, O& options)
{
  WavHeader header = WavHeader(bytes);
  uint16_t sample_size = header.get_block_align() / header.get_num_of_channels();

  switch (sample_size)
  {
  case 1:
    effect<uint8_t>(bytes, header, options);
    break;

  case 2:
    effect<int16_t>(bytes, header, options);
    break;

  case 4:
    if (header.get_audio_format() != format::WAVE_FORMAT_IEEE_FLOAT)
    {
      effect<int32_t>(bytes, header, options);
    }
    else
    {
      effect<float>(bytes, header, options);
    }
    break;

  case 8:
    if (header.get_audio_format() != format::WAVE_FORMAT_IEEE_FLOAT)
    {
      effect<int64_t>(bytes, header, options);
    }
    else
    {
      effect<double>(bytes, header, options);
    }

  default:
    throw std::invalid_argument("Error: Invalid sample size or unsupported data format!");
    break;
  }
}

// Trim effect launcher.
void effects::trim(std::vector<uint8_t>& bytes, TrimOptions& options)
{
  WavHeader header = WavHeader(bytes);
  effect(bytes, header, options);
};

// Fade effect launcher.
void effects::fade(std::vector<uint8_t>& bytes, FadeOptions& options)
{
  effect_with_type_switch<FadeOptions>(bytes, options);
};

// Reverb effect launcher.
void effects::reverb(std::vector<uint8_t>& bytes, ReverbOptions& options)
{
  effect_with_type_switch<ReverbOptions>(bytes, options);
};


/* How effects work */

// Trim effect implementation.
void effect(std::vector<uint8_t>& bytes, WavHeader& header, TrimOptions& options)
{
  uint32_t data_off = header.get_data_offset();
  uint32_t data_size = header.get_data_size();

  if (!options.end_flag)
  {
    options.end_ms = header.get_length_ms();
  }

  uint32_t start_off = data_off + ms_to_byte_count(header, options.start_ms);
  uint32_t end_off = data_off + ms_to_byte_count(header, options.end_ms);

  bytes.erase(bytes.begin() + end_off, bytes.begin() + data_off + data_size);
  bytes.erase(bytes.begin() + data_off, bytes.begin() + start_off);

  uint32_t size_change = data_size - (end_off - start_off);

  if (data_size % 2 == 1)
  {
    bytes.insert(bytes.begin() + data_off + data_size - size_change, (uint8_t)0);
  }

  // Changing Data Subchunk Size
  write_to_bytes<uint32_t>(bytes, data_off - 4, data_size - size_change);
  // Changing RIFF Chunk Size
  write_to_bytes<uint32_t>(bytes, 4, header.get_file_size() - 8 - size_change);
}

// Fade effect implementation.
template <typename T>
void effect(std::vector<uint8_t>& bytes, WavHeader& header, FadeOptions& options)
{
  uint32_t data_off = header.get_data_offset();
  uint16_t block_size = header.get_block_align();
  uint16_t sample_size = block_size / header.get_num_of_channels();

  if (!options.end_flag)
  {
    options.end_ms = header.get_length_ms();
  }

  uint32_t start_off = data_off + ms_to_byte_count(header, options.start_ms);
  uint32_t end_off = data_off + ms_to_byte_count(header, options.end_ms);

  uint32_t length;
  double ratio = 1., reverse;
  if (start_off < end_off)
  {
    length = static_cast<double>(end_off - start_off);
    ratio = 1.;
    reverse = 1;
  }
  else
  {
    length = static_cast<double>(start_off - end_off);
    ratio = options.end_lvl_01;
    reverse = -1;
    std::swap(start_off, end_off);
  }

  double diff = (1. - options.end_lvl_01) * static_cast<double>(block_size) / length;

  T smpl;

  if (std::is_unsigned<T>::value)
  {
    T smpl_amp, mid_smpl = std::numeric_limits<T>::max() / 2;

    for (uint32_t block_off = start_off; block_off < end_off; block_off += block_size)
    {
      for (uint32_t channel_off = 0; channel_off < block_size; channel_off += sample_size)
      {
        smpl = read_from_bytes<T>(bytes, block_off + channel_off);

        if (smpl < mid_smpl)
        {
          smpl_amp = mid_smpl - smpl;
          smpl_amp = static_cast<T>(static_cast<double>(smpl_amp) * ratio);
          smpl = mid_smpl - smpl_amp;
        }
        else
        {
          smpl_amp = smpl - mid_smpl;
          smpl_amp = static_cast<T>(static_cast<double>(smpl_amp) * ratio);
          smpl = mid_smpl + smpl_amp;
        }

        write_to_bytes<T>(bytes, block_off + channel_off, smpl);
      }
      ratio -= diff * reverse;
    }
  }
  else
  {
    for (uint32_t block_off = start_off; block_off < end_off; block_off += block_size)
    {
      for (uint32_t channel_off = 0; channel_off < block_size; channel_off += sample_size)
      {
        smpl = read_from_bytes<T>(bytes, block_off + channel_off);
        smpl = static_cast<T>(static_cast<double>(smpl) * ratio);
        write_to_bytes<T>(bytes, block_off + channel_off, smpl);
      }
      ratio -= diff * reverse;
    }
  }
}

// Reverb effect implementation.
template <typename T>
void effect(std::vector<uint8_t>& bytes, WavHeader& header, ReverbOptions& options)
{
  uint16_t block_size = header.get_block_align();
  uint16_t num_of_chan = header.get_num_of_channels();
  uint16_t sample_size = block_size / num_of_chan;
  uint32_t start_off = header.get_data_offset();
  uint32_t end_off = start_off + header.get_data_size() - block_size;

  uint32_t delay = ms_to_byte_count(header, options.delay_ms);
  uint32_t delay_start = start_off + delay;

  T smpl, delay_smpl;

  if (std::is_unsigned<T>::value)
  {
    T mid_smpl = std::numeric_limits<T>::max() / 2;

    for (uint32_t block_off = end_off; block_off >= delay_start; block_off -= block_size)
    {
      for (uint32_t channel_off = 0; channel_off < block_size; channel_off += sample_size)
      {
        smpl = read_from_bytes<T>(bytes, block_off + channel_off);
        delay_smpl = read_from_bytes<T>(bytes, block_off - delay + channel_off);

        if (delay_smpl < mid_smpl)
        {
          delay_smpl = mid_smpl - delay_smpl;
          smpl -= static_cast<T>(options.decay_01 * static_cast<double>(delay_smpl));
        }
        else
        {
          delay_smpl = delay_smpl - mid_smpl;
          smpl += static_cast<T>(options.decay_01 * static_cast<double>(delay_smpl));
        }

        write_to_bytes<T>(bytes, block_off + channel_off, smpl);
      }
    }
  }
  else
  {
    for (uint32_t block_off = end_off; block_off >= delay_start; block_off -= block_size)
    {
      for (uint32_t channel_off = 0; channel_off < block_size; channel_off += sample_size)
      {
        smpl = read_from_bytes<T>(bytes, block_off + channel_off);
        delay_smpl = read_from_bytes<T>(bytes, block_off - delay + channel_off);

        smpl += static_cast<T>(options.decay_01 * static_cast<double>(delay_smpl));;

        write_to_bytes<T>(bytes, block_off + channel_off, smpl);
      }
    }
  }
}

/* Support functions implementation */

template<typename T>
T read_from_bytes(std::vector<uint8_t>& bytes, uint32_t pos)
{
  T val = 0;
  memcpy(&val, &bytes[pos], sizeof(T));
  return val;
}

template<typename T>
void write_to_bytes(std::vector<uint8_t>& bytes, uint32_t pos, T val)
{
  memcpy(&bytes[pos], &val, sizeof(T));
}

uint32_t ms_to_byte_count(WavHeader& header, uint32_t time_ms)
{
  uint32_t samples_count = header.get_frequency() * time_ms / 1000;
  uint32_t bytes_count = samples_count * header.get_block_align();
  if (bytes_count > header.get_data_size())
  {
    throw std::invalid_argument("Error: Time point " + std::to_string(time_ms) + " is not in data range!");
  }
  return bytes_count;
}
