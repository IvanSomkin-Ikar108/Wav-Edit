#include "readfile.h"

#include <fstream>
#include <stdexcept>
#include <sys/stat.h>

/* Support functions */

size_t get_file_size(const char* file_path)
{
  struct stat results;
  if (stat(file_path, &results) == 0)
  {
    return results.st_size;
  }
  else
  {
    throw std::runtime_error("Error: Couldn't get stat file size.");
  }
}

/* Header functions */

bool file_exists(const char* file_path)
{
  struct stat results;
  if (stat(file_path, &results) != -1)
  {
    return true;
  }
  return false;
}

std::vector<uint8_t> readfile_count(const char* file_path, size_t read_byte_count)
{
  std::ifstream infile(file_path, std::ios_base::in | std::ios_base::binary);
  if (!infile.is_open())
  {
    throw std::invalid_argument("Error: File path '" + std::string(file_path) + "' could not be opened.");
  }

  std::vector<uint8_t> bytes;

  bytes.resize(read_byte_count);
  infile.read((char*)&bytes[0], read_byte_count);

  if (infile.fail())
  {
    throw std::runtime_error("Error: Failed to read '" + std::to_string(read_byte_count) + "' bytes from '" + file_path + "'.");
  }

  infile.close();
  return bytes;
}

std::vector<uint8_t> readfile(const char* file_path, size_t max_byte_read)
{
  size_t file_byte_count = get_file_size(file_path);
  std::streamsize read_byte_count = (std::streamsize)file_byte_count;
  if (max_byte_read < file_byte_count)
  {
    read_byte_count = (std::streamsize)max_byte_read;
  }
  return readfile_count(file_path, read_byte_count);
}

std::vector<uint8_t> readfile(const char* file_path)
{
  return readfile_count(file_path, get_file_size(file_path));
}
