#include <fstream>
#include <iterator>
#include "writefile.h"

void writefile(std::vector<uint8_t>& bytes ,std::string filename)
{
  std::ofstream file (filename, std::ios::out | std::ios::binary | std::ios::trunc);
  std::ostream_iterator<uint8_t> out_itr(file);
  std::copy(bytes.begin(), bytes.end(), out_itr);
}
