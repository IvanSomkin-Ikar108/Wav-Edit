#include "readfile.h"
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> readfile(const std::string& filePath, size_t maxRead8 = 0)
{
  if (maxRead8 > 0) 
  {
    std::ifstream infile(filePath, std::ios_base::in | std::ios_base::binary);
    if (infile.fail())
    {
      throw std::invalid_argument("Error: File path (" + filePath + ") could not be opened.");
    }

    infile.seekg(0, std::ios::end);
    size_t fileLength = (size_t)infile.tellg();
    if (fileLength < maxRead8)
    {
      throw std::invalid_argument("Error: File length (" + std::to_string(fileLength) + ") is smaller than needed length of byte vector (" + std::to_string(maxRead8) + ").");
    }
    infile.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> bytes;
    bytes.resize(maxRead8);

    infile.read((char*)&bytes[0], (std::streamsize)maxRead8);
     
    infile.close();
    return bytes;
  }
  else
  {
    throw std::invalid_argument("Error: Max count of read bytes vector (" + std::to_string(maxRead8) + ") is less than 1.");
  }
}
