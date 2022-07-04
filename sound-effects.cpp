#include "sound-effects.h"
#include "wav-header.h"
#include <stdexcept>

void write_int32_to_bytes(std::vector<uint8_t>& bytes, uint32_t pos, uint32_t i)
{
  bytes[pos] =  i & 0xFF;
  bytes[pos + 1] = (i >> 8) & 0xFF;
  bytes[pos + 2] = (i >> 16) & 0xFF;
  bytes[pos + 3] = (i >> 24) & 0xFF;
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
  uint32_t end_offset = data_offset + ms_to_bytes(header, end_ms);

  bytes.erase(bytes.begin() + end_offset, bytes.begin() + data_offset + data_size);
  bytes.erase(bytes.begin() + data_offset, bytes.begin() + start_offset);

  uint32_t size_change = data_size - (end_offset - start_offset);

  if (data_size % 2 == 1)
  {
    bytes.insert(bytes.begin() + data_offset + data_size - size_change, (uint8_t)0);
  }
  
  write_int32_to_bytes(bytes, data_offset - 4, data_size - size_change);
  write_int32_to_bytes(bytes, 4, header.get_file_size() - 8 - size_change); //8 is RIFF + size field

  //Chunk_size offset = 4, Data_chunk_size offset = data_offset - 4
};

void effects::cut(std::vector<uint8_t>& bytes, uint32_t start_ms)
{
  WavHeader header = WavHeader(bytes);
  cut(bytes, start_ms, header.get_length_ms());
};

void effects::levels(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256)
{

};

void effects::reverb(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256)
{

};