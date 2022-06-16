#include "readfile.h"
#include <iostream>

std::vector<uint8_t> readfile(std::string filePath, size_t neededLength)
{
  if (neededLength > 0) 
  {
    std::ifstream infile(filePath, std::ios_base::in | std::ios_base::binary);
    if (infile.fail())
    {
      throw std::invalid_argument("Error: File path (" + filePath + ") could not be opened.");
    }

    infile.seekg(0, std::ios::end);
    size_t fileLength = (size_t)infile.tellg();
    if (fileLength < neededLength)
    {
      throw std::invalid_argument("Error: File length (" + std::to_string(fileLength) + ") is smaller than needed length of byte vector (" + std::to_string(neededLength) + ").");
    }
    infile.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> bytes(neededLength);
    bytes.resize(neededLength);
    size_t readLength = 0, BUFFER_SIZE = 4096;

    while (neededLength - readLength > BUFFER_SIZE) 
    {
      infile.read((char*)&bytes[readLength], (std::streamsize)BUFFER_SIZE);
      readLength += BUFFER_SIZE;
    }
    if (neededLength - readLength > 0)
    {
      infile.read((char*)&bytes[readLength], (std::streamsize)(neededLength - readLength));
    }

    infile.close();
    return bytes;
  }
  else
  {
    throw std::invalid_argument("Error: Needed length of byte vector (" + std::to_string(neededLength) + ") is less than 1.");
  }
}
