#ifndef READFILE_H
#define READFILE_H

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> readfile(std::string filePath, size_t neededLength);

#endif
