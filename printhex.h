#ifndef PRINTHEX_H
#define PRINTHEX_H

#include <vector>
#include <cstdint>

// Couts one byte as hex with separator at the end.
void cout_as_hex(uint8_t byte, char separator = '\0');

// Prints vector of bytes as hex using cout_as_hex.
// Parameter columnsCount can be set to change number of printed columns.
// Parameter maxPrintCount can be set to change the max amount of bytes to print.
void print_as_hex_columns(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256);

#endif
