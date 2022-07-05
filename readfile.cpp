#include "readfile.h"
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
    
size_t get_file_size(const char* filePath)
{
  struct stat results;
  if (stat(filePath, &results) == 0)
  {
    return results.st_size;
  }
  else
  {
    throw std::runtime_error("Error: Couldn't get stat file size.");
  }
}

std::vector<uint8_t> readfile_count(const char* filePath, size_t readByteCount)
{
  std::ifstream infile(filePath, std::ios_base::in | std::ios_base::binary);
  if (!infile.is_open())
  {
    throw std::invalid_argument("Error: File path (" + std::string(filePath) + ") could not be opened.");
  }

  std::vector<uint8_t> bytes;

  bytes.resize(readByteCount);
  infile.read((char*)&bytes[0], readByteCount);
  if (infile.fail())
  {
    throw std::runtime_error("Error: Failed to read (" + std::to_string(readByteCount) + ") bytes from (" + filePath + ").");
  }

  infile.close();

  return bytes;
}

std::vector<uint8_t> readfile(const char* filePath, size_t maxByteRead)
{
  size_t fileByteCount = get_file_size(filePath);
  std::streamsize readByteCount = (std::streamsize)fileByteCount;
  if (maxByteRead < fileByteCount)
  {
    readByteCount = (std::streamsize)maxByteRead;
  }
  return readfile_count(filePath, readByteCount);
}

std::vector<uint8_t> readfile(const char* filePath)
{
  return readfile_count(filePath, get_file_size(filePath));
}
