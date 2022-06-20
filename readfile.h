#ifndef READFILE_H
#define READFILE_H

#include <vector>
#include <cstdint>
#include <string>

std::vector<uint8_t> readfile(const std::string& filePath, size_t neededLength8);
// throw invalid_argument exception if arguments are incorrect

#endif
