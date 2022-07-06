#include "readfile.h"
#include <iostream>
#include <iomanip>

void cout_as_hex(uint8_t byte, char separator)
{
  std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)byte << separator;
}

void print_as_hex_columns(std::vector<uint8_t>& bytes, size_t columnsCount, size_t maxPrintCount)
{
  int printLength = maxPrintCount;
  if (bytes.size() < maxPrintCount)
  {
    printLength = bytes.size();
  }

  int columns = columnsCount;
  int rows = printLength / columns;
  int rest = printLength % columns;
  int i, j, readLength = 0;

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < columns - 1; j++)
    {
      cout_as_hex(bytes[readLength], ' ');
      readLength++;
    }
    cout_as_hex(bytes[readLength], '\n');
    readLength++;
  }
  if (rest != 0)
  {
    for (j = 0; j < rest - 1; j++)
    {
      cout_as_hex(bytes[readLength], ' ');
      readLength++;
    }
    cout_as_hex(bytes[readLength], '\n');
  }
}
