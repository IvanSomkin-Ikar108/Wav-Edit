#include "readfile.h"
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> readfile(const char* filePath, size_t maxByteRead)
{
  std::ifstream infile(filePath, std::ios_base::in | std::ios_base::binary);
  if (!infile.is_open())
  {
    throw std::invalid_argument("Error: File path (" + std::string(filePath) + ") could not be opened.");
  }

  infile.seekg(0, std::ios::end);
  size_t fileByteCount = (size_t)infile.tellg();
  infile.seekg(0, std::ios::beg);

  std::streamsize readByteCount = (std::streamsize)fileByteCount;

  if (maxByteRead > 0 && maxByteRead < fileByteCount)
  {
    readByteCount = (std::streamsize)maxByteRead;
  }
  
  std::vector<uint8_t> bytes;

  if (readByteCount > 0)
  {
    bytes.resize(readByteCount);
    infile.read((char*)&bytes[0], readByteCount);
    if(infile.fail())
    {
      throw std::runtime_error("Error: Failed to read (" + std::to_string(readByteCount) + ") bytes from (" + filePath + ").");
    }
  }
    
  infile.close();
  
  return bytes;
}
