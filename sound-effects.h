#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <vector>
#include <cstdint>

namespace effects
{
  void cut(std::vector<uint8_t>& bytes, uint32_t start, uint32_t end);

  void cut(std::vector<uint8_t>& bytes, uint32_t start);

  void levels(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256);

  void reverb(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256);
}

#endif
